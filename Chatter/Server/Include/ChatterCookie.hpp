#pragma once

#include <string>
#include <set>
#include "ChatterAlfanumericGenerator.hpp"

namespace Chatter
{

class Cookie
{
public:
    Cookie() = default;
    explicit Cookie(const std::string& p_cookie);
    Cookie(const Cookie&) = default;
    Cookie(Cookie&&) = default;
    Cookie& operator=(const Cookie&) = default;
    Cookie& operator=(Cookie&&) = default;
    bool operator==(const Cookie& p_other) const;
    bool operator!=(const Cookie& p_other) const;
    bool operator<(const Cookie& p_other) const;
    std::string toString() const;
        
private:
    std::string cookie;
};

class CookieStore
{
public:
    Cookie allocateCookie();
    void releaseCookie(const Cookie& p_cookie);

private:
    Cookie generateCookie();
    bool exists(const Cookie& p_cookie) const;

    AlfanumericGenerator generator;
    std::set<Cookie> cookies;
};

}
