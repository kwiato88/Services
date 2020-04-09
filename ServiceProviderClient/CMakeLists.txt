cmake_minimum_required(VERSION 2.8)

project(ServiceProviderClient)

add_library(
    ServiceProviderClientLib
    Source/ServiceProviderClient.cpp
    Source/ClientStore.cpp
)

target_include_directories(
    ServiceProviderClientLib
    PUBLIC
    Include
    ../ServiceProviderSerCodec/Include
    ../Utils/Include
    ../../sock/Include
    ../../msg/Include
    C:/Users/pikwiatk/boost_1_61_0
)

add_executable(
    ClientEx
    Source/ClientAppEx.cpp
)

target_include_directories(
    ClientEx
    PUBLIC
    Include
    ../ServiceProviderSerCodec/Include
    ../Utils/Include
    ../../sock/Include
    ../../msg/Include
    C:/Users/pikwiatk/boost_1_61_0
)

add_dependencies(
    ClientEx
    JsonCodecLib
    ServiceProviderCodecLib
    ServiceProviderClientLib
)

target_link_libraries(
    ClientEx
    JsonCodecLib
    ServiceProviderCodecLib
    ServiceProviderClientLib
    libMsg.lib
    libSock.lib
    ws2_32
)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
