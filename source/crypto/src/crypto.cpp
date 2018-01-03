#include <naivecoin/crypto/crypto.h>

#include <sstream>
#include <iomanip>

#include <openssl/sha.h>

namespace naivecoin {

std::string compute_hash(std::string const & data)
{
    unsigned char hash[SHA_DIGEST_LENGTH]; // == 20 for SHA1
    SHA1(
        reinterpret_cast<const unsigned char*>(data.c_str()),
        data.size(),
        hash
    );

    std::ostringstream stream;

    for(int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        stream
            << std::setfill('0')
            << std::setw(2)
            << std::hex
            << static_cast<unsigned int>(hash[i]);
    }

    return stream.str();
}

} // namespace naivecoin
