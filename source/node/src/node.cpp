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
: peers(peers)
, blockchain()
, miner(public_key, seed)
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

    this->server_thread = std::thread([this]() {
        this->server.start();
    });
}

void Node::start()
{
    this->logger->info("Node active");
    while (true) {
        Block const & latest_block = * this->blockchain.crbegin();

        this->miner.request_mine_next_block(latest_block);

        Block const & next_block = this->miner.get_next_block();

        this->blockchain.push_back(next_block);
    }
}

void Node::request_new_block()
{
}

void Node::process_send_block_message(Block const & block, std::string const & sender)
{
    std::ostringstream stream;
    stream << block;
    this->logger->info("Received block {} from sender {}", stream.str(), sender);
}

void Node::process_send_blockchain_message(std::list<Block> const & blockchain, std::string const & sender)
{
    this->logger->info("Received blockchain of {} blocks from sender {}", blockchain.size(), sender);
}

void Node::process_query_latest_block_message(std::string const & sender)
{
    this->logger->info("Sender {} requested latest block", sender);
}

void Node::process_query_blockchain_message(std::string const & sender)
{
    this->logger->info("Sender {} requested blockchain", sender);
}

void Node::process_unknown_message(std::string const & message, std::string const & sender)
{
    this->logger->info("Sender {} sent unknown message {}", sender, message);
}

void Node::process_invalid_message(std::string const & content, std::string const & error)
{
    std::string const prefix = "Hash mismatch";
    this->logger->info("Received invalid message: {}", error);
    if (std::equal(prefix.begin(), prefix.end(), error.begin())) {
        this->logger->info("Content: \"{}\"", content);
    }
}

} // namespace naivecoin
