
#pragma once

#ifndef NAIVECOIN_NODE_PEERSMANAGER_H
#define NAIVECOIN_NODE_PEERSMANAGER_H

#include <string>
#include <set>
#include <map>

#include <spdlog/spdlog.h>

#include "sender.h"

namespace naivecoin::node {

class PeersManager
{
public:
    PeersManager();

    void start();

    void add_peer(std::string const & peer);
    void send_message(std::string const & message, std::string const & peer);
    void send_message_to_all(std::string const & message);

    uint16_t const MAX_FAILURES = 5;

private:
    void success(std::string const & peer);
    void error(std::string const & peer);

private:
    std::map<std::string, uint16_t> failures;
    std::set<std::string> peers;

    Sender sender;

    std::shared_ptr<spdlog::logger> logger;
};

} // namespace naivecoin::node

#endif // NAIVECOIN_NODE_PEERSMANAGER_H
