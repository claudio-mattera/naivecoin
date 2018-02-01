#include "miner.h"

#include <chrono>

#include <naivecoin/transaction/output.h>
#include <naivecoin/transaction/input.h>
#include <naivecoin/transaction/transaction.h>

#include <naivecoin/serialization/transaction.h>

#include <naivecoin/time/time.h>

namespace naivecoin::node {

using namespace naivecoin;

Miner::Miner(
    std::string const & public_key,
    uint64_t const sleep_time,
    uint64_t const seed
)
: public_key(public_key)
, sleep_time(sleep_time)
, latest_blocks()
, next_block()
, input_mutex()
, input_condition_variable()
, output_mutex()
, output_condition_variable()
, mersenne_twister_engine(seed)
, logger(spdlog::get("miner"))
{
}

void Miner::start()
{
    while (true) {
        core::Block const * latest_block_ptr;
        {
            std::unique_lock<std::mutex> unique_lock(this->input_mutex);
            this->input_condition_variable.wait(unique_lock);

            latest_block_ptr = & this->latest_blocks.back();
        }

        this->interrupted = false;

        std::optional<core::Block> const next_block = this->mine_next_block(*latest_block_ptr);

        if (next_block) {
            std::lock_guard<std::mutex> lock_guard(this->output_mutex);
            this->next_block.emplace(next_block.value());
        }

        this->output_condition_variable.notify_one();
    }
}

void Miner::interrupt()
{
    this->interrupted = true;
}

void Miner::request_mine_next_block(
    core::Block const & latest_block,
    std::list<transaction::Transaction> pending_transactions
)
{
    {
        std::lock_guard<std::mutex> lock_guard(this->input_mutex);

        this->latest_blocks.push(latest_block);
        if (this->latest_blocks.size() > Miner::DIFFICULTY_ADJUSTMENT_INTERVAL_IN_BLOCKS) {
            this->latest_blocks.pop();
        }

        this->pending_transactions.clear();
        this->pending_transactions.splice(std::end(this->pending_transactions), pending_transactions);
    }
    this->input_condition_variable.notify_one();
}

std::optional<core::Block> Miner::get_next_block()
{
    {
        std::unique_lock<std::mutex> unique_lock(this->output_mutex);
        this->output_condition_variable.wait(unique_lock);

        if (this->next_block) {
            core::Block const next_block = this->next_block.value();
            this->next_block.reset();
            return std::make_optional(next_block);
        } else {
            return std::nullopt;
        }
    }
}

std::optional<core::Block> Miner::mine_next_block(core::Block const & latest_block)
{
    uint64_t const index = 1 + latest_block.index;

    transaction::Transaction const coinbase_transaction = transaction::create_coinbase_transaction(
        index,
        this->public_key
    );
    std::list<transaction::Transaction> block_transactions{coinbase_transaction};
    block_transactions.splice(std::end(block_transactions), this->pending_transactions);

    std::string const & previous_hash = latest_block.hash;
    std::string const data = serialization::serialize_transactions(block_transactions);
    std::time_t const timestamp= time::now();
    uint16_t const difficulty = this->get_difficulty();

    std::optional<core::Block> const next_block = this->find_next_block(
        index,
        previous_hash,
        timestamp,
        data,
        difficulty
    );

    return next_block;
}

std::optional<core::Block> Miner::find_next_block(
    uint64_t const index,
    std::string const & previous_hash,
    std::time_t const & timestamp,
    std::string const & data,
    uint16_t const difficulty
)
{
    while (! this->interrupted) {
        uint64_t const nonce = this->mersenne_twister_engine();

        std::string const hash = core::compute_block_hash(
            index,
            previous_hash,
            timestamp,
            data,
            difficulty,
            nonce
        );
        if (core::hash_matches_difficulty(hash, difficulty)) {
            return std::make_optional(
                core::Block::make_block(
                    index,
                    previous_hash,
                    timestamp,
                    data,
                    difficulty,
                    nonce
                )
            );
        }

        std::this_thread::sleep_for( std::chrono::milliseconds(this->sleep_time));
    }

    return std::nullopt;
}

uint16_t Miner::get_difficulty()
{
    core::Block const & latest_block = this->latest_blocks.back();
    if (latest_block.index % Miner::DIFFICULTY_ADJUSTMENT_INTERVAL_IN_BLOCKS == 0) {
        return this->get_adjusted_difficulty(latest_block);
    } else {
        return latest_block.difficulty;
    }
}

uint16_t Miner::get_adjusted_difficulty(core::Block const & latest_block)
{
    std::ostringstream log_stream;
    log_stream << "Adjusting difficulty for block " << latest_block.index;

    core::Block const latest_adjustment_block = this->latest_blocks.front();

    auto expected_elapsed_time = std::chrono::seconds(
        Miner::DIFFICULTY_ADJUSTMENT_INTERVAL_IN_BLOCKS * Miner::BLOCK_GENERATION_INTERVAL_IN_SECONDS
    );

    auto actual_elapsed_time = std::chrono::seconds(
        static_cast<int>(
            std::difftime(latest_block.timestamp, latest_adjustment_block.timestamp)
        )
    );

    uint16_t new_difficulty;

    uint16_t const INCREMENT = 1;

    log_stream << ", elapsed time: " << actual_elapsed_time.count() << 's';
    if (actual_elapsed_time < expected_elapsed_time / 2) {
        log_stream << ", increasing difficulty";
        new_difficulty = latest_block.difficulty + INCREMENT;
    } else if (actual_elapsed_time > expected_elapsed_time * 2) {
        log_stream << ", decreasing difficulty";
        new_difficulty = latest_block.difficulty - INCREMENT;
    } else {
        log_stream << ", maintaining difficulty";
        new_difficulty = latest_block.difficulty;
    }

    log_stream << " to " << new_difficulty;

    this->logger->info(log_stream.str());

    return new_difficulty;
}

bool Miner::is_timestamp_valid(core::Block const & new_block)
{
    core::Block const & latest_block = this->latest_blocks.back();
    std::time_t const now = std::time(nullptr);

    if (std::difftime(new_block.timestamp, latest_block.timestamp) < -60) {
        return false;
    } else if (std::difftime(now, new_block.timestamp) < -60) {
        return false;
    } else {
        return true;
    }
}

} // namespace naivecoin::node
