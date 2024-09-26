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
    ../PrinterSerCodec/Include
    ../ServiceProviderSerCodec/Include
    ../PrinterSer/Include
    ../Utils/Include
    ../sock/Include
    ../msg/Include
    C:/Users/pikwiatk/boost_1_85_0
)

add_dependencies(
    PrinterClientEx
    PrinterSerCodecLib
    JsonCodecLib
    ServiceProviderCodecLib
    ServiceProviderClientLib
    libMsg
    libSock
)

target_link_libraries(
    PrinterClientEx
    PrinterSerCodecLib
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
