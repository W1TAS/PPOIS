    cmake_minimum_required(VERSION 3.29)
    project(pizzeria_v2)

    set(CMAKE_CXX_STANDARD 20)
    set(SOURCE_FILES main.cpp)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")


    add_executable(pizzeria_run ${SOURCE_FILES})

    include_directories(pizzeria_lib)
    add_subdirectory(pizzeria_lib)

    target_link_libraries(pizzeria_run pizzeria_lib)
    add_subdirectory(Google_tests)

    add_library(pizzeria_v2 STATIC main.cpp)
