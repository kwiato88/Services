cmake_minimum_required(VERSION 3.0)

project(Utils)

add_library(
    JsonCodecLib
    STATIC
    Source/JsonCodec.cpp
)

target_include_directories(
    JsonCodecLib
    PUBLIC
    Include
    C:/Users/pikwiatk/boost_1_85_0
)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
