#include <stdexcept>
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

Cookie CookieStore::generateCookie()
{
    static const std::size_t cookieLength = 16;
    return Cookie(generator.generate(cookieLength));
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
