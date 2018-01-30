#include "sender.h"

#include <simple-web-server/client_http.hpp>

namespace {

} // unnamed namespace

namespace naivecoin::node {

Sender::Sender()
: logger(spdlog::get("sender"))
{
}

void Sender::start()
{
    while (true) {
        Tuple next_tuple;
        bool queue_is_empty;
        {
            std::lock_guard<std::mutex> lock_guard(this->mutex);

            if (this->queue.empty()) {
                queue_is_empty = true;
            } else {
                next_tuple = this->queue.front();
                this->queue.pop();
                queue_is_empty = false;
            }
        }

        if (queue_is_empty) {
            this->logger->debug("Queue is empty, waiting for messages");
            std::unique_lock<std::mutex> unique_lock(this->mutex);
            this->condition_variable.wait(unique_lock);
        } else {
            std::string const message = std::get<0>(next_tuple);
            std::string const receiver = std::get<1>(next_tuple);
            std::function<void()> const on_success = std::get<2>(next_tuple);
            std::function<void()> const on_error = std::get<3>(next_tuple);
            this->logger->debug("Sending a message to {}", receiver);
            SimpleWeb::Client<SimpleWeb::HTTP> client(receiver);
            client.request(
                "POST",
                "/", message,
                [this, &receiver, &on_error, &on_success](auto /*response*/, auto error_code) {
                    if(error_code) {
                        this->logger->error(
                            "Sending message to {} failed: {}",
                            receiver,
                            error_code.message()
                        );
                        on_error();
                    } else {
                        on_success();
                    }
                }
            );
            client.io_service->run();
            /*auto request = client.request("POST", "/", message);
            request->content.string();*/
        }
    }
}

void Sender::enqueue_message(
    std::string const & message,
    std::string const & receiver,
    std::function<void()> on_success,
    std::function<void()> on_error
)
{
    {
        std::lock_guard<std::mutex> lock_guard(this->mutex);
        this->queue.push(std::make_tuple(message, receiver, on_success, on_error));
    }
    this->condition_variable.notify_one();
}

} // namespace naivecoin::node
