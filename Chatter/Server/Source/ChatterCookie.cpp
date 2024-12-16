#include <stdexcept>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include "ChatterCookie.hpp"

namespace Chatter
{

Cookie::Cookie(const std::string& p_cookie)
    : cookie(p_cookie)
{
}

bool Cookie::operator==(const Cookie& p_other) const
{
    return cookie == p_other.cookie;
}

bool Cookie::operator!=(const Cookie& p_other) const
{
    return cookie != p_other.cookie;
}

bool Cookie::operator<(const Cookie& p_other) const
{
    return cookie < p_other.cookie;
}

std::string Cookie::toString() const
{
    return cookie;
}

CookieStore::CookieStore()
{
    std::srand(std::time(nullptr));
}

Cookie CookieStore::generateCookie() const
{
    static const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static const size_t cookieLength = 16;
    std::string cookie;
    cookie.reserve(cookieLength);
    for(size_t i = 0; i < cookieLength; ++i)
    {
        cookie.push_back(chars[std::rand() % chars.size()]);
    }
    return Cookie(cookie);
}

bool CookieStore::exists(const Cookie& p_cookie) const
{
    return cookies.count(p_cookie) == 1;
}

Cookie CookieStore::allocateCookie()
{
    auto cookie = generateCookie();
    for(std::size_t i = 0; exists(cookie) && i < 5; ++i)
    {
        cookie = generateCookie();
    }
    if(exists(cookie))
    {
        throw std::runtime_error("Failed to generate unique cookie");
    }
    cookies.insert(cookie);
    return cookie;
}

void CookieStore::releaseCookie(const Cookie& p_cookie)
{
    cookies.erase(p_cookie);
}

}
