cmake_minimum_required(VERSION 3.0)

add_library(
    ChatterServerLib
    STATIC
    Source/ChatterCookie.cpp
    Source/ChatterServer.cpp
)

target_include_directories(
    ChatterServerLib
    PUBLIC
    Include
    ../Codec/Include
    ../../msg/Include
    ../../msg/sock/Include
    ../../Utils/Include
    C:/Users/pikwiatk/boost_1_85_0
)

add_executable(
    ChatterServerTests
    Source/ChatterServerUt.cpp
)

target_include_directories(
    ChatterServerTests
    PUBLIC
    Include
    ../Codec/Include
    ../../msg/Include
    ../../msg/sock/Include
    ../../Utils/Include
    ../../../
    C:/Users/pikwiatk/boost_1_85_0
)

add_dependencies(
    ChatterServerTests
    ChatterServerLib
    ChatterCodecLib
    libMsg
    libSock
)

target_link_libraries(
    ChatterServerTests
    ChatterServerLib
    ChatterCodecLib
    libMsg
    libSock
    ws2_32
)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
