
#pragma once

#ifndef NAIVECOIN_NODE_SENDER_H
#define NAIVECOIN_NODE_SENDER_H

#include <string>
#include <utility>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <spdlog/spdlog.h>

namespace naivecoin::node {

class Sender
{
public:
    Sender();

    void start();

    void enqueue_message(std::string const & message, std::string const & receiver);

private:
    std::queue<std::pair<std::string, std::string>> queue;

    std::mutex mutex;
    std::condition_variable condition_variable;

    std::shared_ptr<spdlog::logger> logger;
};

} // namespace naivecoin::node

#endif // NAIVECOIN_NODE_SENDER_H
