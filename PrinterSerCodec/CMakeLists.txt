cmake_minimum_required(VERSION 2.8)

project(PrinterSerCodec)

add_library(
    PrinterSerCodecLib
    STATIC
    Source/PrinterSerMsg.cpp
    Source/PrinterSerCodec.cpp
)

target_include_directories(
    PrinterSerCodecLib
    PUBLIC
    Include
    ../Utils/Include
    C:/Users/pikwiatk/boost_1_85_0
)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
