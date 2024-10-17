cmake_minimum_required(VERSION 3.0)

project(Services)

add_subdirectory(msg)
add_subdirectory(Utils)
add_subdirectory(ServiceProviderSerCodec)
add_subdirectory(ServiceProviderSer)
add_subdirectory(ServiceProviderClient)
add_subdirectory(PrinterSerCodec)
add_subdirectory(PrinterSer)
add_subdirectory(PrinterClient)
add_subdirectory(Chatter)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
