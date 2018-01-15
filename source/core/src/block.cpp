#include <naivecoin/core/block.h>

#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

#include <naivecoin/core/utils.h>

#include <naivecoin/crypto/crypto.h>

namespace naivecoin {

Block::Block(
        uint64_t const index,
        std::string const hash,
        std::string const previous_hash,
        std::time_t const timestamp,
        std::string const data,
        uint16_t const difficulty,
        uint64_t const nonce
    )
: index(index)
, hash(hash)
, previous_hash(previous_hash)
, timestamp(timestamp)
, data(data)
, difficulty(difficulty)
, nonce(nonce)
{
}

// static
Block Block::make_block(
        uint64_t const index,
        std::string const previous_hash,
        std::time_t const timestamp,
        std::string const data,
        uint16_t const difficulty,
        uint64_t const nonce
    )
{
    std::string const hash = compute_block_hash(
        index,
        previous_hash,
        timestamp,
        data,
        difficulty,
        nonce
    );

    return Block(index, hash, previous_hash, timestamp, data, difficulty, nonce);
}

// static
Block Block::genesis()
{
    auto timestamp = naivecoin::parse_timestamp("2017-12-28T15:00:00Z");

    uint64_t const index = 0;
    std::string const previous_hash = "";
    std::string const data = "";
    uint16_t const difficulty = 0;
    uint64_t const nonce = 0;
    std::string const hash = compute_block_hash(
        index,
        previous_hash,
        timestamp,
        data,
        difficulty,
        nonce
    );

    return Block(index, hash, previous_hash, timestamp, data, difficulty, nonce);
}

std::ostream & operator<<(std::ostream & stream, Block const & block)
{
    stream
        << "Block index: " << block.index << ", "
        << "Timestamp: " << naivecoin::format_timestamp(block.timestamp) << ", "
        << "Previous hash: " << block.previous_hash << ", "
        << "Hash: " << block.hash << ", "
        << "Data: " << block.data << ", "
        << "Difficulty: " << block.difficulty << ", "
        << "Nonce: " << block.nonce << ", ";
    return stream;
}

std::string compute_block_hash(
        uint64_t const index,
        std::string const & previous_hash,
        std::time_t const & timestamp,
        std::string const & data,
        uint16_t const difficulty,
        uint64_t const nonce
    )
{
    std::ostringstream stream;

    stream
        << index << ','
        << previous_hash << ','
        << naivecoin::format_timestamp(timestamp) << ','
        << data << ','
        << difficulty << ','
        << nonce;

    std::string const whole_string = stream.str();

    return naivecoin::compute_hash(whole_string);
}

bool is_new_block_valid(Block const & new_block, Block const & previous_block)
{
    if (new_block.index != previous_block.index + 1) {
        return false;
    }

    std::string previous_hash = compute_block_hash(
        previous_block.index,
        previous_block.previous_hash,
        previous_block.timestamp,
        previous_block.data,
        previous_block.difficulty,
        previous_block.nonce
    );
    if (new_block.previous_hash != previous_hash) {
        return false;
    }

    std::string hash = compute_block_hash(
        new_block.index,
        new_block.previous_hash,
        new_block.timestamp,
        new_block.data,
        new_block.difficulty,
        new_block.nonce
    );
    if (new_block.hash != hash) {
        return false;
    }

    return true;
}

bool is_blockchain_valid(std::list<Block> const & blockchain)
{
    auto iterator = std::begin(blockchain);

    if (iterator == std::end(blockchain)) {
        return false;
    }

    auto previous_iterator = iterator++;

    while (iterator != std::end(blockchain)) {
        if (!is_new_block_valid(*iterator, *previous_iterator)) {
            return false;
        }

        previous_iterator = iterator++;
    }

    return true;
}

bool hash_matches_difficulty(std::string const & hash, uint16_t const difficulty)
{
    int const N = 20;
    std::array<bool, 4 * N> const hash_in_binary = hex_to_binary<N>(hash);
    for (int i = 0; i < difficulty; ++i) {
        if (hash_in_binary[i]) {
            return false;
        }
    }
    return true;
}

uint64_t compute_cumulative_difficulty(std::list<Block> const & blockchain)
{
    std::vector<uint64_t> difficulties(blockchain.size());
    std::transform(
        std::begin(blockchain),
        std::end(blockchain),
        std::begin(difficulties),
        [](auto const & block) {
            return block.difficulty;
        }
    );
    return std::accumulate(
        std::begin(difficulties),
        std::end(difficulties),
        0,
        [](uint64_t const accumulator, uint64_t const difficulty) {
            return accumulator + std::pow(2, difficulty);
        }
    );
}

} // namespace naivecoin
