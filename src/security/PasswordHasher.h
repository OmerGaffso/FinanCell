#pragma once

#include <string>

/** Provides one-way password hashing and verification. */
class PasswordHasher
{
public:
    virtual ~PasswordHasher() = default;
    virtual std::string hash(const std::string& password) const = 0;
    virtual bool verify(const std::string& password, const std::string& encodedHash) const = 0;
    virtual bool isEncodedHash(const std::string& value) const = 0;
};

/** Argon2id password hashing backed by libsodium. */
class SodiumPasswordHasher final : public PasswordHasher
{
public:
    SodiumPasswordHasher();
    std::string hash(const std::string& password) const override;
    bool verify(const std::string& password, const std::string& encodedHash) const override;
    bool isEncodedHash(const std::string& value) const override;
};
