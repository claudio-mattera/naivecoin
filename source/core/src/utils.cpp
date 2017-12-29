#include <naivecoin/core/utils.h>

#include <sstream>
#include <iomanip>

namespace naivecoin {

std::time_t now()
{
    return std::time(nullptr);
}

std::time_t parse_timestamp(std::string const & text)
{
    std::tm tm = {};
    std::istringstream stream(text);
    stream >> std::get_time(& tm, "%Y-%m-%dT%TZ");
    return std::mktime(& tm);
}


std::string format_timestamp(std::time_t const & timestamp)
{
    std::ostringstream stream;
    stream << std::put_time(std::localtime(& timestamp), "%FT%TZ");
    return stream.str();
}

} // namespace naivecoin
