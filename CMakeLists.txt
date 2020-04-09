cmake_minimum_required(VERSION 2.8)

project(Services)

link_directories(
    C:/Users/pikwiatk/boost_1_61_0/binMsvc
    C:/Users/pikwiatk/sock/BuildMS/Release
    C:/Users/pikwiatk/msg/BuildMS/Release
)

add_subdirectory(Utils)
add_subdirectory(ServiceProviderSerCodec)
add_subdirectory(ServiceProviderSer)
add_subdirectory(ServiceProviderClient)
add_subdirectory(PrinterSerCodec)
add_subdirectory(PrinterSer)
add_subdirectory(PrinterClient)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
