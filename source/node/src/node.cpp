#include "node.h"

#include <naivecoin/core/serialize.h>

#include <sstream>

namespace {

} // unnamed namespace


namespace naivecoin {

Node::Node(
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

std::string Node::process_message(std::string const & path, std::string const & message)
{
    using namespace std::placeholders;

    if (path == "/") {
        naivecoin::process_message(
            message,
            std::bind(& Node::process_send_block_message, this, _1, _2),
            std::bind(& Node::process_send_blockchain_message, this, _1, _2),
            std::bind(& Node::process_query_latest_block_message, this, _1),
            std::bind(& Node::process_query_blockchain_message, this, _1),
            std::bind(& Node::process_unknown_message, this, _1, _2),
            std::bind(& Node::process_invalid_message, this, _1)
        );
        return "";
    } else if (path == "/query/blockchain") {
        return serialize_blockchain(this->blockchain);
    }

    return "";
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

void Node::process_invalid_message(std::string const & error)
{
    this->logger->info("Received invalid message: {}", error);
}

} // namespace naivecoin
