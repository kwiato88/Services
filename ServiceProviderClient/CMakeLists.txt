cmake_minimum_required(VERSION 3.0)

project(ServiceProviderClient)

add_library(
    ServiceProviderClientLib
    Source/ServiceProviderClient.cpp
    Source/ClientStore.cpp
    Source/CachedClientStore.cpp
)

target_include_directories(
    ServiceProviderClientLib
    PUBLIC
    Include
    ../ServiceProviderSerCodec/Include
    ../Utils/Include
    ../msg/Include
    ../msg/sock/Include
    C:/Users/pikwiatk/boost_1_85_0
)

add_executable(
    ServiceProviderClientEx
    Source/ClientAppEx.cpp
)

target_include_directories(
    ServiceProviderClientEx
    PUBLIC
    Include
    ../ServiceProviderSerCodec/Include
    ../Utils/Include
    ../sock/Include
    ../msg/Include
    C:/Users/pikwiatk/boost_1_85_0
)

add_dependencies(
    ServiceProviderClientEx
    JsonCodecLib
    ServiceProviderCodecLib
    ServiceProviderClientLib
    libMsg
    libSock
)

target_link_libraries(
    ServiceProviderClientEx
    JsonCodecLib
    ServiceProviderCodecLib
    ServiceProviderClientLib
    libMsg
    libSock
    ws2_32
)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
