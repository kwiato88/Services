#pragma once

#include <string>
#include <map>
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

    PasswordHash hash(const Password& p_pass, const Salt& p_salt) const;

    AlfanumericGenerator generator;
    std::map<User, UserData> users;
};

}
