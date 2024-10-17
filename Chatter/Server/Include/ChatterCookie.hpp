#pragma once

#include <string>
#include <vector>

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
    CookieStore();
    Cookie allocateCookie();
    void releaseCookie(const Cookie& p_cookie);

private:
    Cookie generateCookie() const;
    bool exists(const Cookie& p_cookie) const;

    std::vector<Cookie> cookies;
};

}
