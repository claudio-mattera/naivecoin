#include <naivecoin/core/serialize.h>

#include <sstream>
#include <chrono>
#include <iomanip>
#include <ctime>

#include <jsoncpp/json/json.h>

namespace {

std::string to_string(std::wstring const & input)
{
    std::string output;
    output.assign(input.begin(), input.end());
    return output;
}

std::wstring to_wstring(std::string const & input)
{
    std::wstring output;
    output.assign(input.begin(), input.end());
    return output;
}

std::string to_string(std::chrono::system_clock::time_point const & input)
{
    std::time_t const time = std::chrono::system_clock::to_time_t(input);
    std::ostringstream stream;
    stream << std::put_time(std::gmtime(& time), "%FT%TZ");
    return stream.str();
}

std::chrono::system_clock::time_point timepoint_from_string(std::string const & input)
{
    std::tm tm = {};
    std::istringstream stream(input);
    stream >> std::get_time(& tm, "%Y-%m-%dT%TZ");
    return std::chrono::system_clock::from_time_t(std::mktime(& tm));
}

} // unnamed namespace

namespace naivecoin {

std::wstring serialize_block(Block const & block)
{
    Json::Value root;

    root["index"] = block.index;
    root["hash"] = to_string(block.hash);
    root["previous_hash"] = to_string(block.previous_hash);
    root["timestamp"] = to_string(block.timestamp);
    root["data"] = to_string(block.data);

    std::ostringstream stream;
    stream << root;

    return to_wstring(stream.str());
}

std::wstring serialize_blockchain(std::list<Block> const & blockchain)
{
}

Block deserialize_block(std::wstring const & text)
{
}

std::list<Block> deserialize_blockchain(std::wstring const & text)
{
}

} // namespace naivecoin
