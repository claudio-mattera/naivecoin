#include "peersmanager.h"

namespace naivecoin::node {

PeersManager::PeersManager()
: logger(spdlog::get("peersmanager"))
{
}

void PeersManager::start()
{
    this->sender.start();
}

void PeersManager::add_peer(std::string const & peer)
{
    this->peers.insert(peer);
}

void PeersManager::send_message(std::string const & message, std::string const & peer)
{
    this->sender.enqueue_message(
        message,
        peer,
        std::bind(& PeersManager::success, this, peer),
        std::bind(& PeersManager::error, this, peer)
    );
}

void PeersManager::send_message_to_all(std::string const & message)
{
    for (auto peer: this->peers) {
        this->send_message(message, peer);
    }
}

void PeersManager::success(std::string const & peer)
{
    this->failures[peer] = 0;
}

void PeersManager::error(std::string const & peer)
{
    auto failures_it = this->failures.find(peer);

    uint16_t const f = failures_it == std::end(this->failures)
        ? 1
        : 1 + failures_it->second;

    this->logger->error("Sending message to {} failed for the {}th time", peer, f);

    if (f > PeersManager::MAX_FAILURES) {
        this->logger->error("Too many failures, forgetting peer {}", peer);

        this->failures.erase(failures_it);
        auto peers_it = this->peers.find(peer);
        if (peers_it != std::end(this->peers)) {
            this->peers.erase(peers_it);
        }
    } else {
        this->failures[peer] = f;
    }
}

} // namespace naivecoin::node
