cmake_minimum_required(VERSION 2.8)

project(ServiceProviderClient)

add_executable(
    ClientEx
    Source/ClientAppEx.cpp
)

target_include_directories(
    ClientEx
    PUBLIC
    Include
    ../ServiceProviderSerCodec/Include
    ../../sock/Include
    ../../msg/Include
    C:/Users/pikwiatk/boost_1_61_0
)

add_dependencies(
    ClientEx
    ServiceProviderCodecLib
)

target_link_libraries(
    ClientEx
    ServiceProviderCodecLib
    libMsg.lib
    libSock.lib
    ws2_32
)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
