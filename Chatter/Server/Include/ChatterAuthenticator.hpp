#pragma once

#include <string>
#include <map>
#include "ChatterIAuthenticator.hpp"

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

    PasswordHash hash(const Password& p_pass) const;

    std::map<User, PasswordHash> users;
};

}
