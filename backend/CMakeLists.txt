cmake_minimum_required(VERSION 3.16)

project(ScientificCalculator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon CONFIG REQUIRED)

add_executable(
    scientific-calculator
    main.cpp
)

target_link_libraries(
    scientific-calculator
    PRIVATE
    Drogon::Drogon
)
