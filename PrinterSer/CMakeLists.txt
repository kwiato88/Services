cmake_minimum_required(VERSION 2.8)

project(PrinterService)

add_executable(
    PrinterSer
    Source/PrinterSerMain.cpp
    Source/PrinterSer.cpp
)

target_include_directories(
    PrinterSer
    PUBLIC
    Include
    ../PrinterSerCodec/Include
    ../ServiceProviderSerCodec/Include
    ../ServiceProviderClient/Include
    ../Utils/Include
    ../../sock/Include
    ../../msg/Include
    C:/Users/pikwiatk/boost_1_61_0
)

add_dependencies(
    PrinterSer
    PrinterSerCodecLib
    JsonCodecLib
    ServiceProviderCodecLib
    ServiceProviderClientLib
)

target_link_libraries(
    PrinterSer
    PrinterSerCodecLib
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
