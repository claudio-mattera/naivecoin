#include <naivecoin/crypto/crypto.h>

#include <sstream>
#include <array>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <mutex>

#include <openssl/sha.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/pem.h>

#if OPENSSL_VERSION_NUMBER < 0x10100000
# define EVP_MD_CTX_new EVP_MD_CTX_create
# define EVP_MD_CTX_free EVP_MD_CTX_destroy
#endif


namespace {

std::mutex hash_mutex;

EVP_MD const * const HASH_FUNCTION = EVP_sha1();

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

std::vector<unsigned char> hex_to_array(std::string const & hex)
{
    std::vector<unsigned char> array(hex.size() / 2);

    std::istringstream stream(hex);

    for (std::size_t i = 0; i < hex.size(); i += 2) {
        char high;
        stream >> high;
        char low;
        stream >> low;
        char const tmp[3] = {high, low, '\0'};
        unsigned int const value = std::strtoul(tmp, nullptr, 16);
        array[i / 2] = value;
    }

    return array;
}

inline std::string read_from_bio(BIO * const bio)
{
    std::size_t const length = BIO_get_mem_data(bio, nullptr);
    std::vector<char> vector(length);
    BIO_read(bio, vector.data(), length);
    return std::string{vector.data()};
}

} // unnamed namespace

namespace naivecoin::crypto {

std::string compute_hash(std::string const & data)
{
    std::lock_guard<std::mutex> guard(hash_mutex);

    OpenSSL_add_all_digests();

    /* Create the Message Digest Context and wrap it in a unique_ptr */
    std::unique_ptr<EVP_MD_CTX, void(*)(EVP_MD_CTX*)> message_digest_context(
        EVP_MD_CTX_new(),
        EVP_MD_CTX_free
    );

    /* Initialise the Digest operation */
    if(1 != EVP_DigestInit_ex(message_digest_context.get(), HASH_FUNCTION, nullptr)) {
        throw std::runtime_error("Digest initialization failed");
    }

    /* Call update with the message */
    if(1 != EVP_DigestUpdate(message_digest_context.get(), data.c_str(), data.size())) {
        throw std::runtime_error("Digest update failed");
    }

    unsigned int slen;
    std::array<unsigned char, EVP_MAX_MD_SIZE> sig;

    /* Obtain the signature */
    if(1 != EVP_DigestFinal_ex(message_digest_context.get(), sig.data(), & slen)) {
        throw std::runtime_error("Digest final failed");
    }

    return array_to_hex(sig.data(), slen);
}

std::pair<std::string, std::string> generate_key_pair()
{
    std::lock_guard<std::mutex> guard(hash_mutex);

    std::string const ECCTYPE = "secp521r1";

    // Inspired by http://fm4dd.com/openssl/eckeycreate.htm

    /* ---------------------------------------------------------- *
     * These function calls initialize openssl for correct work.  *
     * ---------------------------------------------------------- */
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();

    /* ---------------------------------------------------------- *
     * Create the Input/Output BIO's.                             *
     * ---------------------------------------------------------- */

    /* ---------------------------------------------------------- *
     * Create a EC key structure, setting the group type from NID *
     * ---------------------------------------------------------- */
    int const eccgrp = OBJ_txt2nid(ECCTYPE.c_str());
    EC_KEY * const curve = EC_KEY_new_by_curve_name(eccgrp);

    /* -------------------------------------------------------- *
     * For cert signing, we use  the OPENSSL_EC_NAMED_CURVE flag*
     * ---------------------------------------------------------*/
    EC_KEY_set_asn1_flag(curve, OPENSSL_EC_NAMED_CURVE);

    /* -------------------------------------------------------- *
     * Create the public/private EC key pair here               *
     * ---------------------------------------------------------*/
    if (! EC_KEY_generate_key(curve)) {
        throw std::runtime_error("Error generating the ECC key.");
    }

    /* -------------------------------------------------------- *
     * Converting the EC key into a PKEY structure let us       *
     * handle the key just like any other key pair.             *
     * ---------------------------------------------------------*/
    std::unique_ptr<EVP_PKEY, void(*)(EVP_PKEY*)> pkey(
        EVP_PKEY_new(),
        EVP_PKEY_free
    );
    if (! EVP_PKEY_assign_EC_KEY(pkey.get(), curve)) {
        throw std::runtime_error("Error assigning ECC key to EVP_PKEY structure.");
    }

    std::unique_ptr<BIO, void(*)(BIO*)> bio(
        BIO_new(BIO_s_mem()),
        BIO_free_all
    );

    /* ---------------------------------------------------------- *
     * Here we print the private/public key data in PEM format.   *
     * ---------------------------------------------------------- */
    if(! PEM_write_bio_PrivateKey(bio.get(), pkey.get(), nullptr, nullptr, 0, 0, nullptr)) {
        throw std::runtime_error("Error writing private key data in PEM format");
    }
    BIO_flush(bio.get());

    std::string const private_key = read_from_bio(bio.get());

    if(!PEM_write_bio_PUBKEY(bio.get(), pkey.get())) {
        throw std::runtime_error("Error writing public key data in PEM format");
    }
    BIO_flush(bio.get());

    std::string const public_key = read_from_bio(bio.get());

    return std::make_pair(public_key, private_key);
}

std::string sign(std::string const & data, std::string const & private_key)
{
    std::lock_guard<std::mutex> guard(hash_mutex);

    std::unique_ptr<BIO, void(*)(BIO*)> bio(
        BIO_new_mem_buf(private_key.c_str(), private_key.size()),
        BIO_free_all
    );

    std::unique_ptr<EVP_PKEY, void(*)(EVP_PKEY*)> pkey(
        PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, nullptr),
        EVP_PKEY_free
    );

    if (! pkey) {
        throw std::runtime_error("Read private key failed");
    }

    /* Create the Message Digest Context and wrap it in a unique_ptr */
    std::unique_ptr<EVP_MD_CTX, void(*)(EVP_MD_CTX*)> message_digest_context(
        EVP_MD_CTX_new(),
        EVP_MD_CTX_free
    );

    /* Initialise the DigestSign operation - SHA-256 has been selected as the message digest function in this example */
    if(1 != EVP_DigestSignInit(message_digest_context.get(), nullptr, HASH_FUNCTION, nullptr, pkey.get())) {
        throw std::runtime_error("Digest initialization failed");
    }

    /* Call update with the message */
    if(1 != EVP_DigestSignUpdate(message_digest_context.get(), data.c_str(), data.size())) {
        throw std::runtime_error("Digest update failed");
    }

    /* Finalise the DigestSign operation */
    /* First call EVP_DigestSignFinal with a nullptr sig parameter to obtain the length of the
     * signature. Length is returned in slen */
    std::size_t slen;
    if(1 != EVP_DigestSignFinal(message_digest_context.get(), nullptr, & slen)) {
        throw std::runtime_error("Digest final failed");
    }

    /* Allocate memory for the signature based on size in slen */
    std::vector<unsigned char> sig(slen);

    /* Obtain the signature */
    if(1 != EVP_DigestSignFinal(message_digest_context.get(), sig.data(), & slen)) {
        throw std::runtime_error("Digest final failed");
    }

    return array_to_hex(sig.data(), slen);
}

bool verify(std::string const & data, std::string const & signature, std::string const & public_key)
{
    std::lock_guard<std::mutex> guard(hash_mutex);

    std::unique_ptr<BIO, void(*)(BIO*)> bio(
        BIO_new_mem_buf(public_key.c_str(), public_key.size()),
        BIO_free_all
    );

    std::unique_ptr<EVP_PKEY, void(*)(EVP_PKEY*)> pkey(
        PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr),
        EVP_PKEY_free
    );

    if (! pkey) {
        throw std::runtime_error("Read public key failed");
    }

    /* Create the Message Digest Context and wrap it in a unique_ptr */
    std::unique_ptr<EVP_MD_CTX, void(*)(EVP_MD_CTX*)> message_digest_context(
        EVP_MD_CTX_new(),
        EVP_MD_CTX_free
    );

    /* Initialize `key` with a public key */
    if(1 != EVP_DigestVerifyInit(message_digest_context.get(), nullptr, HASH_FUNCTION, nullptr, pkey.get())) {
        throw std::runtime_error("Digest initialization failed");
    }
    /* Initialize `key` with a public key */
    if(1 != EVP_DigestVerifyUpdate(message_digest_context.get(), data.c_str(), data.size())) {
        throw std::runtime_error("Digest update failed");
    }

    std::vector<unsigned char> const array = hex_to_array(signature);

    if(1 == EVP_DigestVerifyFinal(message_digest_context.get(), array.data(), array.size())) {
        return true;
    } else {
        return false;
    }
}

} // namespace naivecoin::crypto
