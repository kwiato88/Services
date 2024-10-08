cmake_minimum_required(VERSION 3.0)

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
    ../Utils/Include
    ../msg/Include
    ../msg/sock/Include
    C:/Users/pikwiatk/boost_1_85_0
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
    ../Utils/Include
    ../msg/Include
    ../msg/sock/Include
    C:/Users/pikwiatk/boost_1_85_0
)

add_dependencies(
    ServiceProviderSer
    ServiceProviderSerLib
    ServiceProviderCodecLib
    JsonCodecLib
    libMsg
    libSock
)

target_link_libraries(
    ServiceProviderSer
    ServiceProviderSerLib
    ServiceProviderCodecLib
    JsonCodecLib
    libMsg
    libSock
    ws2_32
)

add_executable(
    ServiceProviderSerTest
    Source/AddrRegisterUt.cpp
)

target_include_directories(
    ServiceProviderSerTest
    PUBLIC
    Include
    ../ServiceProviderSerCodec/Include
    ../Utils/Include
    ../msg/Include
    ../msg/sock/Include
    ../../
    C:/Users/pikwiatk/boost_1_85_0
)

add_dependencies(
    ServiceProviderSerTest
    ServiceProviderSerLib
    libSock
)

target_link_libraries(
    ServiceProviderSerTest
    ServiceProviderSerLib
    libSock
    ws2_32
)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
