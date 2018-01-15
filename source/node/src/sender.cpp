#include "sender.h"

#include <simple-web-server/client_http.hpp>

namespace {

} // unnamed namespace

namespace naivecoin {

Sender::Sender()
: logger(spdlog::get("sender"))
{
}

void Sender::start()
{
    while (true) {
        std::pair<std::string, std::string> next_pair;
        bool queue_is_empty;
        {
            std::lock_guard<std::mutex> lock_guard(this->mutex);

            if (this->queue.empty()) {
                queue_is_empty = true;
            } else {
                next_pair = this->queue.front();
                this->queue.pop();
                queue_is_empty = false;
            }
        }

        if (queue_is_empty) {
            this->logger->info("Queue is empty, waiting for messages");
            std::unique_lock<std::mutex> unique_lock(this->mutex);
            this->condition_variable.wait(unique_lock);
        } else {
            std::string const message = next_pair.first;
            std::string const receiver = next_pair.second;
            this->logger->info("Sending a message to {}", receiver);
            SimpleWeb::Client<SimpleWeb::HTTP> client(receiver);
            client.request("POST", "/", message, [this, &receiver](auto /*response*/, auto error_code) {
                if(error_code) {
                    this->logger->error("Sending message to {} failed: {}", receiver, error_code.message());
                }
            });
            client.io_service->run();
            /*auto request = client.request("POST", "/", message);
            request->content.string();*/
        }
    }
}

void Sender::enqueue_message(std::string const & message, std::string const & receiver)
{
    {
        std::lock_guard<std::mutex> lock_guard(this->mutex);
        this->queue.push(std::make_pair(message, receiver));
    }
    this->condition_variable.notify_one();
}

} // namespace naivecoin
