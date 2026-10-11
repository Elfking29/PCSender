
cmake_minimum_required(VERSION 3.16)

project(PCSender LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/sources/*.cpp
    src/lib/*.cpp
)

add_executable(PCSender
    src/sender.cpp
    ${SOURCES}
)

target_include_directories(PCSender PRIVATE 
    include
    include/lib
    include/sources
)

find_package(nlohmann_json REQUIRED)
find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(UDEV REQUIRED IMPORTED_TARGET libudev)

target_link_libraries(PCSender PRIVATE 
    PkgConfig::UDEV 
    CURL::libcurl
    nlohmann_json::nlohmann_json
)