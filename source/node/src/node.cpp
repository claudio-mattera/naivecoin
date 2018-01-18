#include "node.h"

#include <naivecoin/core/serialize.h>

#include <mutex>
#include <sstream>
#include <algorithm>

namespace {

std::mutex blockchain_mutex;

} // unnamed namespace


namespace naivecoin::node {

using namespace naivecoin;

Node::Node(
    uint16_t const port,
    std::string const & public_key,
    std::vector<std::string> const & peers,
    uint64_t const sleep_time,
    uint64_t const seed
)
: address(std::string("localhost:") + std::to_string(port))
, peers(std::begin(peers), std::end(peers))
, blockchain()
, miner(public_key, sleep_time, seed)
, sender()
, miner_thread(
    std::thread(
        std::bind(& Miner::start, std::ref(miner))
    )
)
, logger(spdlog::get("node"))
{
    this->blockchain.push_back(core::Block::genesis());

    this->server.config.port = port;

    this->server.resource["^/$"]["POST"] = [this](auto response, auto request) {
        std::string const content = request->content.string();
        using namespace std::placeholders;
        try {
            core::process_message(
                content,
                std::bind(& Node::process_send_block_message, this, _1, _2),
                std::bind(& Node::process_send_blockchain_message, this, _1, _2),
                std::bind(& Node::process_query_latest_block_message, this, _1),
                std::bind(& Node::process_query_blockchain_message, this, _1),
                std::bind(& Node::process_unknown_message, this, _1, _2),
                std::bind(& Node::process_invalid_message, this, content, _1)
            );
            response->write();
        } catch (std::exception const & exception) {
            response->write(
                SimpleWeb::StatusCode::client_error_bad_request,
                exception.what()
            );
        }
    };
    this->server.resource["^/query/blockchain$"]["GET"] = [this](auto response, auto /*request*/) {
        response->write(this->serialize_blockchain());
    };
    this->server.on_error = [this](auto /*request*/, auto /*error_code*/) {
    };

    this->sender_thread = std::thread([this]() {
        this->sender.start();
    });

    this->server_thread = std::thread([this]() {
        this->server.start();
    });

    for (std::string const peer: this->peers) {
        this->connect_to_peer(peer);
    }
}

void Node::start()
{
    this->logger->info("Node active with {} peers", this->peers.size());
    while (true) {
        core::Block const latest_block = this->get_latest_block();

        this->miner.request_mine_next_block(latest_block);

        core::Block const & next_block = this->miner.get_next_block();

        if (this->try_adding_block_to_blockchain(next_block)) {
            this->logger->info("Mined next block {}", next_block.index);
            this->send_block_to_peers(next_block);
        } else {
            this->logger->info("Could not add mined block to blockchain");
        }
    }
}

void Node::connect_to_peer(std::string const & peer)
{
    std::string const message = core::create_query_latest_block_message(this->address);

    this->logger->info("Querying peer {} for its latest block", peer);
    this->sender.enqueue_message(message, peer);
}

void Node::send_block_to_peers(core::Block const & block)
{
    for (std::string const peer: this->peers) {
        std::string const message = core::create_send_block_message(block, this->address);

        this->logger->debug("Sending block to peer {}", peer);
        this->sender.enqueue_message(message, peer);
    }
}

bool Node::try_adding_block_to_blockchain(core::Block const & block)
{
    std::lock_guard lock_guard(blockchain_mutex);

    core::Block const & latest_block = * this->blockchain.crbegin();

    if (block.index == latest_block.index + 1 && block.previous_hash == latest_block.hash) {
        this->blockchain.push_back(block);
        return true;
    } else {
        return false;
    }
}

void Node::replace_blockchain(std::list<core::Block> new_blockchain)
{
    std::lock_guard lock_guard(blockchain_mutex);

    this->blockchain.clear();

    this->blockchain.splice(std::begin(this->blockchain), new_blockchain);
}

core::Block Node::get_latest_block() const
{
    std::lock_guard lock_guard(blockchain_mutex);

    core::Block const & latest_block = * this->blockchain.crbegin();

    return latest_block;
}

std::string Node::serialize_blockchain() const
{
    std::lock_guard lock_guard(blockchain_mutex);

    return core::serialize_blockchain(
        std::begin(this->blockchain),
        std::end(this->blockchain)
    );
}

uint64_t Node::compute_cumulative_difficulty() const
{
    std::lock_guard lock_guard(blockchain_mutex);

    return core::compute_cumulative_difficulty(
        std::begin(this->blockchain),
        std::end(this->blockchain)
    );
}

std::string Node::create_send_blockchain_message(std::string const & address) const
{
    std::lock_guard lock_guard(blockchain_mutex);

    return core::create_send_blockchain_message(
        std::begin(this->blockchain),
        std::end(this->blockchain),
        address
    );
}

void Node::process_send_block_message(core::Block const & block, std::string const & sender)
{
    this->logger->info("Received block {} from sender {}", block.index, sender);

    this->add_peer(sender);

    core::Block const latest_block = this->get_latest_block();

    if (block.index <= latest_block.index) {
        // Do nothings
    } else {
        if (block.index == latest_block.index + 1) {
            if (block.previous_hash == latest_block.hash) {
                this->logger->info("This is a valid next block (index: {}), adding it to our blockchain", block.index);
                this->try_adding_block_to_blockchain(block);
            } else {
                this->logger->warn("Block does not belong to our blockchain, ignoring it");
            }
        } else {
            this->logger->info("This block comes from a blockchain longer than ours, querying for full blockchain");

            std::string const message = core::create_query_blockchain_message(this->address);
            this->sender.enqueue_message(message, sender);
        }
    }
}

void Node::process_send_blockchain_message(std::list<core::Block> const & other_blockchain, std::string const & sender)
{
    this->logger->info("Received blockchain of {} blocks from sender {}", other_blockchain.size(), sender);

    this->add_peer(sender);

    if (is_blockchain_valid(std::begin(other_blockchain), std::end(other_blockchain))) {
        this->logger->info("Blockchain is valid");
        uint64_t const other_cumulative_difficulty = core::compute_cumulative_difficulty(
            std::begin(other_blockchain),
            std::end(other_blockchain)
        );
        uint64_t const this_cumulative_difficulty = this->compute_cumulative_difficulty();

        if (other_cumulative_difficulty > this_cumulative_difficulty) {
            this->logger->info(
                "Blockchain has higher cumulative difficulty than ours ({} > {}), replacing it",
                other_cumulative_difficulty,
                this_cumulative_difficulty
            );

            this->replace_blockchain(other_blockchain);
        } else {
            this->logger->info(
                "Out blockchain has higher cumulative difficulty ({} > {})",
                this_cumulative_difficulty,
                other_cumulative_difficulty
            );
        }
    } else {
        this->logger->warn("Blockchain is NOT valid, ignoring this message");
    }
}

void Node::process_query_latest_block_message(std::string const & sender)
{
    this->logger->info("Sender {} requested latest block", sender);

    this->add_peer(sender);

    core::Block const latest_block = this->get_latest_block();
    std::string const message = create_send_block_message(latest_block, this->address);
    this->sender.enqueue_message(message, sender);
}

void Node::process_query_blockchain_message(std::string const & sender)
{
    this->logger->info("Sender {} requested blockchain", sender);

    this->add_peer(sender);

    std::string const message = this->create_send_blockchain_message(this->address);
    this->sender.enqueue_message(message, sender);
}

void Node::process_unknown_message(std::string const & message, std::string const & sender)
{
    this->logger->info("Sender {} sent unknown message {}", sender, message);
}

void Node::process_invalid_message(std::string const & /*content*/, std::string const & error)
{
    std::string const prefix = "Hash mismatch";
    this->logger->info("Received invalid message: {}", error);
    if (std::equal(prefix.begin(), prefix.end(), error.begin())) {
    }
}

void Node::add_peer(std::string const & peer)
{
    if (std::find(std::begin(this->peers), std::end(this->peers), peer) == std::end(this->peers)) {
        this->logger->info("Added new peer {}", peer);
        this->peers.push_back(peer);
    }
}

} // namespace naivecoin::node
