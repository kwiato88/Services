#pragma once

#include <string>

namespace Chatter
{

class IAuthenticator
{
public:
    virtual ~IAuthenticator() = default;
    virtual bool addUser(const std::string& p_user, const std::string& p_password) = 0;
    virtual void removeUser(const std::string& p_user) = 0;
    virtual bool authenticate(const std::string& p_user, const std::string& p_password) = 0;
};

}
