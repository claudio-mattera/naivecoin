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

} // namespace naivecoin
