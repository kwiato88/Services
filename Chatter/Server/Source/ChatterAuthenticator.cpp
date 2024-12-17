#include "ChatterAuthenticator.hpp"

namespace Chatter
{

Authenticator::PasswordHash Authenticator::hash(const Password& p_pass) const
{
    //TODO: use sha-256
    //TODO: add salt
    return std::hash<Password>{}(p_pass); 
}

bool Authenticator::addUser(const std::string& p_user, const std::string& p_password)
{
    return users.emplace(p_user, hash(p_password)).second;
}

void Authenticator::removeUser(const std::string& p_user)
{
    users.erase(p_user);
}

bool Authenticator::authenticate(const std::string& p_user, const std::string& p_password)
{
    auto it = users.find(p_user);
    return it != users.end() && it->second == hash(p_password);
}

}
