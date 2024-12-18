#pragma once

#include <string>
#include <map>
#include <tuple>
#include <filesystem>
#include "ChatterIAuthenticator.hpp"
#include "ChatterAlfanumericGenerator.hpp"

namespace Chatter
{

class Authenticator : public IAuthenticator
{
public:
    bool addUser(const std::string& p_user, const std::string& p_password) override;
    void removeUser(const std::string& p_user) override;
    bool authenticate(const std::string& p_user, const std::string& p_password) override;
    std::list<std::string> activeUsers() override;

private:
    using User = std::string;
    using Password = std::string;
    using PasswordHash = std::size_t;
    using Salt = std::string;
    struct UserData
    {
        PasswordHash hash;
        Salt salt;
    };

    AlfanumericGenerator generator;
    std::map<User, UserData> users;
};

class AuthenticatorWithStorage : public IAuthenticator
{
public:
    AuthenticatorWithStorage(const std::filesystem::path& p_dir);
    bool addUser(const std::string& p_user, const std::string& p_password) override;
    void removeUser(const std::string& p_user) override;
    bool authenticate(const std::string& p_user, const std::string& p_password) override;
    std::list<std::string> activeUsers() override;

private:
    using User = std::string;
    using Password = std::string;
    using PasswordHash = std::size_t;
    using Salt = std::string;

    bool existsIn(const std::filesystem::path& p_file, const User& p_user) const;
    std::tuple<User, Salt, PasswordHash> readUser(const std::string& p_line) const;
    void appendPending(const User& p_user, const Salt& p_salt, const PasswordHash& p_hash) const;
    void removeFrom(const std::filesystem::path& p_file, const User& p_user);

    //TODO: consider using databse to store users
    std::filesystem::path users;
    std::filesystem::path pendingUsers;
    AlfanumericGenerator generator;
};

}
