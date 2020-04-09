cmake_minimum_required(VERSION 2.8)

project(PrinterClient)

add_executable(
    PrinterClientEx
    Source/PrinterClientAppEx.cpp
)

target_include_directories(
    PrinterClientEx
    PUBLIC
    Include
    ../ServiceProviderSerCodec/Include
    ../PrinterSer/Include
    ../../sock/Include
    ../../msg/Include
    C:/Users/pikwiatk/boost_1_61_0
)

add_dependencies(
    PrinterClientEx
    ServiceProviderCodecLib
    ServiceProviderClientLib
)

target_link_libraries(
    PrinterClientEx
    ServiceProviderCodecLib
    ServiceProviderClientLib
    libMsg.lib
    libSock.lib
    ws2_32
)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
