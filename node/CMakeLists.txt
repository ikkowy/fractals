cmake_minimum_required(VERSION 3.10)

project(node)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-O2 -Wall -Wextra -pedantic)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

add_executable(node node.cpp websocket.h websocket.cpp)

target_link_libraries(node ${JSONCPP_LIBRARIES} boost_system pthread)

target_compile_options(node PRIVATE ${JSONCPP_CFLAGS})
