#include "security/PasswordHasher.h"

#include <stdexcept>
#include <vector>

extern "C"
{
int sodium_init(void);
std::size_t crypto_pwhash_strbytes(void);
unsigned long long crypto_pwhash_opslimit_interactive(void);
std::size_t crypto_pwhash_memlimit_interactive(void);
int crypto_pwhash_str(char*, const char*, unsigned long long, unsigned long long, std::size_t);
int crypto_pwhash_str_verify(const char*, const char*, unsigned long long);
}

SodiumPasswordHasher::SodiumPasswordHasher()
{
    if (sodium_init() < 0) throw std::runtime_error("Failed to initialize libsodium.");
}

std::string SodiumPasswordHasher::hash(const std::string& password) const
{
    std::vector<char> encoded(crypto_pwhash_strbytes());
    if (crypto_pwhash_str(encoded.data(), password.data(), password.size(),
                          crypto_pwhash_opslimit_interactive(),
                          crypto_pwhash_memlimit_interactive()) != 0)
    {
        throw std::runtime_error("Password hashing failed.");
    }
    return encoded.data();
}

bool SodiumPasswordHasher::verify(
    const std::string& password,
    const std::string& encodedHash) const
{
    return isEncodedHash(encodedHash) &&
           crypto_pwhash_str_verify(encodedHash.c_str(), password.data(), password.size()) == 0;
}

bool SodiumPasswordHasher::isEncodedHash(const std::string& value) const
{
    return value.rfind("$argon2", 0) == 0;
}
