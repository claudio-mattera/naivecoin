#include "miner.h"

#include <chrono>
#include <iostream>

namespace naivecoin {

Miner::Miner()
{
}

void Miner::start()
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::cout << "Haha" << '\n';
    }
}

void Miner::request_mine_next_block()
{

}

void Miner::mine_next_block()
{

}

} // namespace naivecoin
