
#pragma once

#ifndef NAIVECOIN_NODE_SENDER_H
#define NAIVECOIN_NODE_SENDER_H

#include <string>
#include <tuple>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

#include <spdlog/spdlog.h>

namespace naivecoin::node {

class Sender
{
public:
    Sender();

    void start();

    void enqueue_message(
        std::string const & message,
        std::string const & receiver,
        std::function<void()> on_success,
        std::function<void()> on_error
    );

private:
    typedef std::tuple<
        std::string,
        std::string,
        std::function<void()>,
        std::function<void()>
    > Tuple;

    std::queue<Tuple> queue;

    std::mutex mutex;
    std::condition_variable condition_variable;

    std::shared_ptr<spdlog::logger> logger;
};

} // namespace naivecoin::node

#endif // NAIVECOIN_NODE_SENDER_H
