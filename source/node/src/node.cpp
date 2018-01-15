#include "node.h"

#include <naivecoin/core/serialize.h>


#include <sstream>
#include <algorithm>

namespace {

} // unnamed namespace


namespace naivecoin {

Node::Node(
    uint16_t const port,
    std::string const & public_key,
    std::vector<std::string> const & peers,
    uint64_t const seed
)
: address(std::string("localhost:") + std::to_string(port))
, peers()
, blockchain()
, miner(public_key, seed)
, sender()
, miner_thread(
    std::thread(
        std::bind(& Miner::start, std::ref(miner))
    )
)
, logger(spdlog::get("node"))
{
    this->blockchain.push_back(naivecoin::Block::genesis());

    this->server.config.port = port;

    this->server.resource["^/$"]["POST"] = [this](auto response, auto request) {
        std::string const content = request->content.string();
        using namespace std::placeholders;
        try {
            naivecoin::process_message(
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
        response->write(serialize_blockchain(this->blockchain));
    };
    this->server.on_error = [this](auto /*request*/, auto /*error_code*/) {
    };

    this->sender_thread = std::thread([this]() {
        this->sender.start();
    });

    this->server_thread = std::thread([this]() {
        this->server.start();
    });

    for (std::string const peer: peers) {
        this->connect_to_peer(peer);
    }
}

void Node::start()
{
    this->logger->info("Node active with {} peers", this->peers.size());
    while (true) {
        Block const & latest_block = * this->blockchain.crbegin();

        this->miner.request_mine_next_block(latest_block);

        Block const & next_block = this->miner.get_next_block();

        this->blockchain.push_back(next_block);

        this->send_block_to_peers(next_block);
    }
}

void Node::connect_to_peer(std::string const & peer)
{
    std::string const message = create_query_latest_block_message(this->address);

    this->logger->info("Querying peer {} for its latest block", peer);
    this->sender.enqueue_message(message, peer);
}

void Node::send_block_to_peers(Block const & block)
{
    return;
    for (std::string const peer: this->peers) {
        std::string const message = create_send_block_message(block, this->address);

        this->logger->info("Sending block to peer {}", peer);
        this->sender.enqueue_message(message, peer);
    }
}

void Node::add_block_to_blockchain(Block const & /*block*/)
{
}

void Node::replace_blockchain(std::list<Block> const & /*blockchain*/)
{
}

void Node::process_send_block_message(Block const & block, std::string const & sender)
{
    this->logger->info("Received block {} from sender {}", block.index, sender);

    Block const & latest_block = * this->blockchain.crbegin();

    if (block.index <= latest_block.index) {
        // Do nothings
    } else {
        if (block.index == latest_block.index + 1) {
            if (block.previous_hash == latest_block.hash) {
                this->logger->info("This is a valid next block (index: {}), adding it to our blockchain", block.index);
                this->add_block_to_blockchain(block);
            } else {
                this->logger->warn("Block does not belong to our blockchain, ignoring it");
            }
        } else {
            this->logger->info("This block comes from a blockchain longer than ours, querying for full blockchain");

            std::string const message = create_query_blockchain_message(this->address);
            this->sender.enqueue_message(message, sender);
        }
    }
}

void Node::process_send_blockchain_message(std::list<Block> const & other_blockchain, std::string const & sender)
{
    this->logger->info("Received blockchain of {} blocks from sender {}", other_blockchain.size(), sender);

    if (is_blockchain_valid(other_blockchain)) {
        this->logger->info("Blockchain is valid");
        uint64_t const other_cumulative_difficulty = compute_cumulative_difficulty(other_blockchain);
        uint64_t const this_cumulative_difficulty = compute_cumulative_difficulty(this->blockchain);

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

    Block const & latest_block = * this->blockchain.crbegin();
    std::string const message = create_send_block_message(latest_block, this->address);
    this->sender.enqueue_message(message, sender);
}

void Node::process_query_blockchain_message(std::string const & sender)
{
    this->logger->info("Sender {} requested blockchain", sender);
    std::string const message = create_send_blockchain_message(this->blockchain, this->address);
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

} // namespace naivecoin
