#include <cctype>
#include "ChatterCookie.hpp"
#include "simpleUt/SimpleUt.hpp"

TEST(cookieHas16AlfanumerifChars)
{
    Chatter::CookieStore cookies;
    auto cookie = cookies.allocateCookie();
    IS_EQ(16, cookie.toString().size());
    for(auto c : cookie.toString())
    {
        IS_TRUE(std::isalnum(c));
    }
}

TEST(genearesDifferentCookies)
{
    Chatter::CookieStore cookies;
    auto cookie1 = cookies.allocateCookie();
    auto cookie2 = cookies.allocateCookie();
    auto cookie3 = cookies.allocateCookie();
    IS_NOT_EQ(cookie1.toString(), cookie2.toString());
    IS_NOT_EQ(cookie1.toString(), cookie3.toString());
    IS_NOT_EQ(cookie2.toString(), cookie3.toString());
}

int main()
{
    MAIN_START;
    
    RUN_TEST(cookieHas16AlfanumerifChars);
    RUN_TEST(genearesDifferentCookies);
    
    MAIN_END;
    return 0;
}
