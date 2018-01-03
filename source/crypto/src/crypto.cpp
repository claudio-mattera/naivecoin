#include <naivecoin/crypto/crypto.h>

#include <sstream>
#include <iomanip>

#include <openssl/sha.h>

namespace {

std::string array_to_hex(unsigned char const * const array, std::size_t const length)
{
    std::ostringstream stream;

    for(std::size_t i = 0; i < length; ++i) {
        stream
            << std::setfill('0')
            << std::setw(2)
            << std::hex
            << static_cast<unsigned int>(array[i]);
    }

    return stream.str();
}

} // unnamed namespace

namespace naivecoin {

std::string compute_hash(std::string const & data)
{
    unsigned char hash[SHA_DIGEST_LENGTH]; // == 20 for SHA1
    SHA1(
        reinterpret_cast<const unsigned char*>(data.c_str()),
        data.size(),
        hash
    );

    return array_to_hex(hash, sizeof(hash));
}

} // namespace naivecoin
