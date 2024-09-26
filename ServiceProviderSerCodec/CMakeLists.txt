cmake_minimum_required(VERSION 2.8)

project(ServiceProviderCodec)

add_library(
    ServiceProviderCodecLib
    STATIC
    Source/ServiceProviderSerMsg.cpp
    Source/ServiceProviderSerCodec.cpp
)

target_include_directories(
    ServiceProviderCodecLib
    PUBLIC
    Include
    ../Utils/Include
    C:/Users/pikwiatk/boost_1_85_0
)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
