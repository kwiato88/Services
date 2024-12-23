cmake_minimum_required(VERSION 3.0)

add_library(
    ChatterClientLib
    STATIC
    Source/ChatterClientReceiver.cpp
    Source/ChatterClientApp.cpp
)

target_include_directories(
    ChatterClientLib
    PUBLIC
    Include
    ../Codec/Include
    ../../msg/Include
    ../../msg/sock/Include
    ../../Utils/Include
    ../../WinApi/Include
    ../../ServiceProviderSerCodec/Include
    ../../ServiceProviderClient/Include
    C:/Users/pikwiatk/boost_1_85_0
)

add_executable(
    ChatterClientApp
    Source/ChatterClientMain.cpp
    Source/ChatterClientDialog.cpp
    Source/ChatterRegisterDialog.cpp
    Source/ChatterLoginDialog.cpp
    Resource/ChatterClientDialog.rc
    Resource/ChatterRegisterDialog.rc
    Resource/ChatterLoginDialog.rc
    ../../WinApi/Resource/QueryDialog.rc
)

target_include_directories(
    ChatterClientApp
    PUBLIC
    Include
    ../Codec/Include
    ../../msg/Include
    ../../msg/sock/Include
    ../../Utils/Include
    ../../WinApi/Include
    ../../ServiceProviderSerCodec/Include
    ../../ServiceProviderClient/Include
    C:/Users/pikwiatk/boost_1_85_0
)

add_dependencies(
    ChatterClientApp
    ChatterClientLib
    ChatterCodecLib
    ServiceProviderCodecLib
    ServiceProviderClientLib
    JsonCodecLib
    libMsg
    libSock
    winApiUtils
)

target_link_libraries(
    ChatterClientApp
    JsonCodecLib
    ChatterClientLib
    ChatterCodecLib
    ServiceProviderClientLib
    ServiceProviderCodecLib
    libMsg
    libSock
    winApiUtils
    ws2_32
)
