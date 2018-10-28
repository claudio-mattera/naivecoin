#include <naivecoin/time/time.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/date_facet.hpp>

#include <sstream>
#include <iomanip>

namespace {

constexpr char const * const TIME_FORMAT = "%Y-%m-%dT%H:%M:%SZ";

} // unnamed namespace

namespace naivecoin::time {

instant now()
{
    boost::posix_time::ptime const now = boost::posix_time::second_clock::local_time();
    return now;
}

instant parse_timestamp(std::string const & text)
{
    std::istringstream stream(text);

    boost::posix_time::time_input_facet* facet = new boost::posix_time::time_input_facet(TIME_FORMAT);
    stream.imbue(std::locale(std::locale::classic(), facet));

    instant timestamp;
    stream >> timestamp;

    return timestamp;
}

std::string format_timestamp(instant const & timestamp)
{
    std::ostringstream stream;

    boost::posix_time::time_facet* facet = new boost::posix_time::time_facet(TIME_FORMAT);
    stream.imbue(std::locale(std::locale::classic(), facet));

    stream << timestamp;
    return stream.str();
}

int difference_in_seconds(instant const & first, instant const & second)
{
    return (second - first).total_seconds();
}

} // namespace naivecoin::time
