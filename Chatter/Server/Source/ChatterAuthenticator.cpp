#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
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

std::list<std::string> Authenticator::activeUsers()
{
    std::list<std::string> userList;
    for (const auto& user : users)
    {
        userList.push_back(user.first);
    }
    return userList;
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
catch (std::exception& e)
{
    std::cerr << "Authenticator add " << p_user << " failed: " << e.what() << std::endl;
    return false;
}

void AuthenticatorWithStorage::removeUser(const std::string& p_user)
{
    removeFrom(pendingUsers, p_user);
    removeFrom(users, p_user);
}

void  AuthenticatorWithStorage::removeFrom(const std::filesystem::path& p_file, const User& p_user)
try
{
    std::filesystem::path tempFilePath = p_file;
    tempFilePath.replace_extension("tmp");
    std::ifstream inputFile(p_file);
    std::ofstream tempFile(tempFilePath);
    std::string line;
    while (std::getline(inputFile, line))
    {
        const auto [user, salt, hash] = readUser(line);
        if (user != p_user)
        {
            tempFile << line << '\n';
        }
    }

    inputFile.close();
    tempFile.close();
    std::filesystem::remove(p_file);
    std::filesystem::rename(tempFilePath, p_file);
}
catch (std::exception& e)
{
    std::cerr << "Authenticator remove " << p_user << " from " << p_file << " failed: " << e.what() << std::endl;
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

std::list<std::string> AuthenticatorWithStorage::activeUsers()
{
    std::ifstream file(users);
    if(!file.is_open())
        return {};
    std::list<std::string> userList;
    std::string line;
    while (std::getline(file, line))
    {
        const auto [user, salt, hash] = readUser(line);
        userList.push_back(user);
    }
    return userList;
}

}
