#include "ChatterAlfanumericGenerator.hpp"

namespace Chatter
{

const std::string AlfanumericGenerator::chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

AlfanumericGenerator::AlfanumericGenerator()
 : gen(rd()),
   dis(0, chars.size() - 1)
{
}

char AlfanumericGenerator::generate()
{
   return chars[dis(gen)];
}

std::string AlfanumericGenerator::generate(std::size_t p_length) 
{
    std::string result;
    result.reserve(p_length);
    for(std::size_t i = 0; i < p_length; ++i)
    {
        result.push_back(generate());
    }
    return result;
}

}
