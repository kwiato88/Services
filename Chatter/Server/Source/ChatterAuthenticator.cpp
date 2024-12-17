#include "ChatterAuthenticator.hpp"

namespace Chatter
{

Authenticator::PasswordHash Authenticator::hash(const Password& p_pass, const Salt& p_salt) const
{
    //TODO: consider using library to use better hash function and generate salt
    return std::hash<Password>{}(p_pass + p_salt); 
}

bool Authenticator::addUser(const std::string& p_user, const std::string& p_password)
{
    static const std::size_t saltLength = 16;
    auto salt = generator.generate(saltLength);
    return users.emplace(p_user, UserData{hash(p_password, salt), salt}).second;
}

void Authenticator::removeUser(const std::string& p_user)
{
    users.erase(p_user);
}

bool Authenticator::authenticate(const std::string& p_user, const std::string& p_password)
{
    auto it = users.find(p_user);
    return it != users.end() && it->second.hash == hash(p_password, it->second.salt);
}

}
