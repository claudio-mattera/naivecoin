#include <naivecoin/core/utils.h>

#include <sstream>
#include <iomanip>
#include <mutex>

namespace naivecoin {

std::mutex time_mutex;

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
    stream >> std::get_time(& tm, "%Y-%m-%dT%H:%M:%SZ");
    return std::mktime(& tm);
}

std::string format_timestamp(std::time_t const & timestamp)
{
    std::lock_guard<std::mutex> guard(time_mutex);

    std::ostringstream stream;
    stream << std::put_time(std::localtime(& timestamp), "%Y-%m-%dT%H:%M:%SZ");
    return stream.str();
}

template <int N>
std::array<bool, 4 * N> hex_to_binary(std::string const & text)
{
    std::array<bool, 4 * N> binary;

    for (int i = 0; i < N; ++i) {
        char const digit = text[i];
        std::array<char, 2> const s{digit, 0};
        uint8_t number = std::strtoul(s.data(), nullptr, 16);
        binary[4*i + 3] = number % 2;
        number /= 2;
        binary[4*i + 2] = number % 2;
        number /= 2;
        binary[4*i + 1] = number % 2;
        number /= 2;
        binary[4*i] = number % 2;
    }

    return binary;
}


// Explicit template instantiations

// For SHA1
template
std::array<bool, 4 * 20> hex_to_binary<20>(std::string const & text);

// For tests
template
std::array<bool, 4 * 1> hex_to_binary<1>(std::string const & text);
template
std::array<bool, 4 * 8> hex_to_binary<8>(std::string const & text);

} // namespace naivecoin
