cmake_minimum_required(VERSION 3.20)

project(ZTerm)

add_definitions(-D_UNICODE -DUNICODE -DNOMINMAX)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(win)
add_subdirectory(wt)

