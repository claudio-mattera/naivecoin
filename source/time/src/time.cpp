#include <naivecoin/time/time.h>

#include <mutex>
#include <sstream>
#include <iomanip>

namespace {

// Functions std::get_time and std::put_time are not thread safe!
// Use a mutex to ensure there are no race conditions.
std::mutex time_mutex;

constexpr char const * const TIME_FORMAT = "%Y-%m-%dT%H:%M:%SZ";

} // unnamed namespace

namespace naivecoin::time {

std::time_t now()
{
    std::lock_guard<std::mutex> guard(time_mutex);

    return std::time(nullptr);
}

std::time_t parse_timestamp(std::string const & text)
{
    std::lock_guard<std::mutex> guard(time_mutex);

    std::tm tm = {};
    std::istringstream stream(text);
    stream >> std::get_time(& tm, TIME_FORMAT);
    return std::mktime(& tm);
}

std::string format_timestamp(std::time_t const & timestamp)
{
    std::lock_guard<std::mutex> guard(time_mutex);

    std::ostringstream stream;
    stream << std::put_time(std::localtime(& timestamp), TIME_FORMAT);
    return stream.str();
}

} // namespace naivecoin::time
