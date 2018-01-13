#include "node.h"

#include <naivecoin/core/serialize.h>

#include <sstream>

namespace {

} // unnamed namespace


namespace naivecoin {

Node::Node(std::vector<std::string> const & peers, uint64_t const seed)
: peers(peers)
, miner(seed)
, miner_thread(
    std::thread(
        std::bind(& Miner::start, std::ref(miner))
    )
)
, logger(spdlog::get("node"))
{
    this->logger->info("Node active");
}

void Node::process_message(std::string const & message)
{
    using namespace std::placeholders;

    naivecoin::process_message(
        message,
        std::bind(& Node::process_send_block_message, this, _1, _2),
        std::bind(& Node::process_send_blockchain_message, this, _1, _2),
        std::bind(& Node::process_query_latest_block_message, this, _1),
        std::bind(& Node::process_query_blockchain_message, this, _1),
        std::bind(& Node::process_unknown_message, this, _1, _2),
        std::bind(& Node::process_invalid_message, this, _1)
    );
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
