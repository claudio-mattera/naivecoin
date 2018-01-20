#include <naivecoin/core/utils.h>

namespace naivecoin::core {

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

std::string replace(std::string text, std::string const & pattern, std::string const & after)
{
    std::size_t pos = 0;
    while ((pos = text.find(pattern, pos)) != std::string::npos) {
        text.replace(pos, pattern.length(), after);
        pos += pattern.length();
    }
    return text;
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

} // namespace naivecoin::core
