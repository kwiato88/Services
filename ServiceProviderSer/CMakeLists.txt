cmake_minimum_required(VERSION 2.8)

project(ServiceProvider)

add_library(
    ServiceProviderSerLib
    STATIC
    Source/ServiceProviderSer.cpp
    Source/AddrRegister.cpp
)

target_include_directories(
    ServiceProviderSerLib
    PUBLIC
    Include
    ../ServiceProviderSerCodec/Include
    ../../sock/Include
    ../../msg/Include
    C:/Users/pikwiatk/boost_1_61_0
)

add_executable(
    ServiceProviderSer
    Source/ServiceProviderSerMain.cpp
)

target_include_directories(
    ServiceProviderSer
    PUBLIC
    Include
    ../ServiceProviderSerCodec/Include
    ../../sock/Include
    ../../msg/Include
    C:/Users/pikwiatk/boost_1_61_0
)

add_dependencies(
    ServiceProviderSer
    ServiceProviderSerLib
    ServiceProviderCodecLib
)

target_link_libraries(
    ServiceProviderSer
    ServiceProviderSerLib
    ServiceProviderCodecLib
    libMsg.lib
    libSock.lib
    ws2_32
)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
