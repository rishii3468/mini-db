cmake_minimum_required(VERSION 3.15)
project(mini_db VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(mini_db
    src/main.cpp
    src/engine.cpp
    src/parser.cpp
    src/storage.cpp
    src/btree.cpp
)

target_include_directories(mini_db PRIVATE include)
