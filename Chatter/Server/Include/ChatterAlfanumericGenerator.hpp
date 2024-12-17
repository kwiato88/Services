#pragma once

#include <string>
#include <random>

namespace Chatter
{

class AlfanumericGenerator
{
public:
    AlfanumericGenerator();

    char generate();
    std::string generate(std::size_t p_length);

private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis;
    static const std::string chars;
};

}
