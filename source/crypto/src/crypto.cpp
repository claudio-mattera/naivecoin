#include <naivecoin/crypto/crypto.h>

#include <sstream>
#include <array>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <iostream>

#include <openssl/sha.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/pem.h>

namespace {

EVP_MD const * const HASH_FUNCTION = EVP_sha512();

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

std::pair<std::string, std::string> generate_key_pair()
{
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
    EC_KEY * curve = EC_KEY_new_by_curve_name(eccgrp);

    /* -------------------------------------------------------- *
     * For cert signing, we use  the OPENSSL_EC_NAMED_CURVE flag*
     * ---------------------------------------------------------*/
    EC_KEY_set_asn1_flag(curve, OPENSSL_EC_NAMED_CURVE);

    /* -------------------------------------------------------- *
     * Create the public/private EC key pair here               *
     * ---------------------------------------------------------*/
    if (! EC_KEY_generate_key(curve)) {
        // std::cerr << "Error generating the ECC key." << '\n';
    }

    /* -------------------------------------------------------- *
     * Converting the EC key into a PKEY structure let us       *
     * handle the key just like any other key pair.             *
     * ---------------------------------------------------------*/
    EVP_PKEY * pkey = EVP_PKEY_new();
    if (! EVP_PKEY_assign_EC_KEY(pkey, curve)) {
        // std::cerr << "Error assigning ECC key to EVP_PKEY structure." << '\n';
    }

    /* -------------------------------------------------------- *
     * Now we show how to extract EC-specifics from the key     *
     * ---------------------------------------------------------*/
    curve = EVP_PKEY_get1_EC_KEY(pkey);

    // const EC_GROUP *ecgrp = EC_KEY_get0_group(curve);

    /* ---------------------------------------------------------- *
     * Here we print the key length, and extract the curve type.  *
     * ---------------------------------------------------------- */
    // std::cerr << "ECC Key size: " << EVP_PKEY_bits(pkey) << "bits" << '\n';
    // std::cerr << "ECC Key type: " << OBJ_nid2sn(EC_GROUP_get_curve_name(ecgrp)) << '\n';

    std::size_t const BUFFER_SIZE = 3000;

    std::array<char, BUFFER_SIZE> buffer;
    FILE * stream = fmemopen(buffer.data(), BUFFER_SIZE, "w");

    BIO * bio = BIO_new_fp(stream, BIO_NOCLOSE);
    BIO * b64 = bio; //BIO_new(BIO_f_base64());
    //BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    //BIO_push(b64, bio);

    /* ---------------------------------------------------------- *
     * Here we print the private/public key data in PEM format.   *
     * ---------------------------------------------------------- */
    if(! PEM_write_bio_PrivateKey(b64, pkey, nullptr, nullptr, 0, 0, nullptr)) {
        // std::cerr << "Error writing private key data in PEM format" << '\n';
    }

    BIO_flush(b64);

    std::string const private_key{buffer.data()};

    if(!PEM_write_bio_PUBKEY(b64, pkey)) {
        // std::cerr << "Error writing public key data in PEM format" << '\n';
    }
    BIO_flush(b64);

    std::string const public_key{buffer.data() + private_key.size()};

    /* ---------------------------------------------------------- *
     * Free up all structures                                     *
     * ---------------------------------------------------------- */
    EVP_PKEY_free(pkey);
    EC_KEY_free(curve);
    BIO_free_all(b64);

    fclose(stream);

    return std::make_pair(public_key, private_key);
}

std::string sign(std::string const & data, std::string const & private_key)
{
    BIO * bio = BIO_new_mem_buf(private_key.c_str(), private_key.size());
    BIO * b64 = bio; //BIO_new(BIO_f_base64());
    //BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    //BIO_push(bio, b64);

    std::unique_ptr<EVP_PKEY, void(*)(EVP_PKEY*)> pkey(
        PEM_read_bio_PrivateKey(b64, nullptr, nullptr, nullptr),
        EVP_PKEY_free
    );

    if (! pkey) {
        throw std::runtime_error("Read private key failed");
    }

    /* Create the Message Digest Context and wrap it in a unique_ptr */
    std::unique_ptr<EVP_MD_CTX, void(*)(EVP_MD_CTX*)> message_digest_context(
        EVP_MD_CTX_create(),
        EVP_MD_CTX_destroy
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
    BIO * bio = BIO_new_mem_buf(public_key.c_str(), public_key.size());
    BIO * b64 = bio; //BIO_new(BIO_f_base64());
    //BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    //BIO_push(b64, bio);

    std::unique_ptr<EVP_PKEY, void(*)(EVP_PKEY*)> pkey(
        PEM_read_bio_PUBKEY(b64, nullptr, nullptr, nullptr),
        EVP_PKEY_free
    );

    if (! pkey) {
        throw std::runtime_error("Read public key failed");
    }

    /* Create the Message Digest Context and wrap it in a unique_ptr */
    std::unique_ptr<EVP_MD_CTX, void(*)(EVP_MD_CTX*)> message_digest_context(
        EVP_MD_CTX_create(),
        EVP_MD_CTX_destroy
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

} // namespace naivecoin
