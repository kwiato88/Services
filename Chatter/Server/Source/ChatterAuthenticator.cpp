#include <fstream>
#include <sstream>
#include <algorithm>
#include "ChatterAuthenticator.hpp"

namespace Chatter
{
namespace
{
std::size_t hash(const std::string& p_pass, const std::string& p_salt)
{
    //TODO: consider using library to use better hash function and generate salt
    return std::hash<std::string>{}(p_pass + p_salt); 
}
std::string generateSalt(AlfanumericGenerator& p_generator)
{
    static const std::size_t saltLength = 16;
    return p_generator.generate(saltLength);
}
bool isUserNameValid(const std::string& p_name)
{
    static const std::string alloweChars = "@._-";
    return !p_name.empty() && std::all_of(p_name.begin(), p_name.end(), [](auto c)
    {
        return std::isalnum(c) || (alloweChars.find(c) != std::string::npos);
    });
}
bool isPasswordValid(const std::string& p_pass)
{
    static const std::string allowedChars = "!@#$%^&*_-?.";
    return !p_pass.empty() && std::all_of(p_pass.begin(), p_pass.end(), [](auto c)
    {
        return std::isalnum(c) || (allowedChars.find(c) != std::string::npos);
    });
}
}

bool Authenticator::addUser(const std::string& p_user, const std::string& p_password)
{
    if(!isUserNameValid(p_user) || !isPasswordValid(p_password))
    {
        return false;
    }
    auto salt = generateSalt(generator);
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

AuthenticatorWithStorage::AuthenticatorWithStorage(const std::filesystem::path& p_dir)
    : users(p_dir / "users.txt"),
      pendingUsers(p_dir / "pendingUsers.txt")
{}

bool AuthenticatorWithStorage::addUser(const std::string& p_user, const std::string& p_password)
try
{
    if(existsIn(pendingUsers, p_user) || existsIn(users, p_user))
    {
        return false;
    }
    if(!isUserNameValid(p_user) || !isPasswordValid(p_password))
    {
        return false;
    }
    auto salt = generateSalt(generator);
    appendPending(p_user, salt, hash(p_password, salt));
    return true;
}
catch (std::exception&)
{
    return false;
}

void AuthenticatorWithStorage::removeUser(const std::string& p_user)
{
    //remove from pending and users
}

bool AuthenticatorWithStorage::authenticate(const std::string& p_user, const std::string& p_password)
{
    std::ifstream file(users);
    if(!file.is_open())
        return false;
    std::string line;
    while (std::getline(file, line))
    {
        const auto [user, salt, passHash] = readUser(line);
        if (user == p_user && passHash == hash(p_password, salt))
        {
            return true;
        }
    }
    return false;
}

void AuthenticatorWithStorage::appendPending(const User& p_user, const Salt& p_salt, const PasswordHash& p_hash) const
{
    std::ofstream file(pendingUsers, std::ios::app);
    if(!file.is_open())
        std::runtime_error("Cant' open file " + pendingUsers.string());
    file << p_user << ' ' << p_salt << ' ' << p_hash << '\n';
}

bool AuthenticatorWithStorage::existsIn(const std::filesystem::path& p_file, const User& p_user) const
{
    std::ifstream file(p_file);
    if(!file.is_open())
        std::runtime_error("Cant' open file " + p_file.string());
    std::string line;
    while (std::getline(file, line))
    {
        const auto [user, salt, hash] = readUser(line);
        if (user == p_user)
        {
            return true;
        }
    }
    return false;
}

std::tuple<AuthenticatorWithStorage::User, AuthenticatorWithStorage::Salt, AuthenticatorWithStorage::PasswordHash>
AuthenticatorWithStorage::readUser(const std::string& p_line) const
{
    std::istringstream iss(p_line);
    User user;
    Salt salt;
    PasswordHash hash;
    if (iss >> user >> salt >> hash)
    {
        return {user, salt, hash};
    }
    return {};
}

}
