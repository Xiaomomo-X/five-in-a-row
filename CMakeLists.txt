cmake_minimum_required(VERSION 3.31)
project(game)

set(CMAKE_CXX_STANDARD 20)

set(SFML_DIR "D:/Soft/SFML-2.6.2/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

add_executable(game main.cpp)

target_link_libraries(game sfml-graphics sfml-window sfml-system sfml-audio)