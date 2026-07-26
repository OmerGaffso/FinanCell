#pragma once

#include <string>

/** @brief Provides one-way password hashing and verification. */
class PasswordHasher
{
public:
    /** @brief Destroys the hashing interface. */
    virtual ~PasswordHasher() = default;
    /** @brief Hashes a password. @param password Plaintext password. @return Salted encoded hash. */
    virtual std::string hash(const std::string& password) const = 0;
    /** @brief Verifies a password. @param password Plaintext password. @param encodedHash Encoded hash. @return True when matching. */
    virtual bool verify(const std::string& password, const std::string& encodedHash) const = 0;
    /** @brief Detects a supported hash. @param value Stored value. @return True for an encoded hash. */
    virtual bool isEncodedHash(const std::string& value) const = 0;
};

/** @brief Argon2id password hashing backed by libsodium. */
class SodiumPasswordHasher final : public PasswordHasher
{
public:
    /** @brief Initializes libsodium. @throws std::runtime_error When initialization fails. */
    SodiumPasswordHasher();
    /** @copydoc PasswordHasher::hash */
    std::string hash(const std::string& password) const override;
    /** @copydoc PasswordHasher::verify */
    bool verify(const std::string& password, const std::string& encodedHash) const override;
    /** @copydoc PasswordHasher::isEncodedHash */
    bool isEncodedHash(const std::string& value) const override;
};
