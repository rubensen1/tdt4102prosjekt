cmake_minimum_required(VERSION 3.25)

project(CarAI VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)

FetchContent_MakeAvailable(raylib)

add_executable(yuh
    src/main.cpp
    src/car.cpp
    src/neural.cpp
)

target_link_libraries(yuh PRIVATE raylib)

target_compile_options(yuh PRIVATE -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC)
# target_link_options(yuh PRIVATE -fsanitize=address)