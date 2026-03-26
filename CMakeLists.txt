cmake_minimum_required(VERSION 3.30)

project(CppProgrammerMindset VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

add_executable(Understand src/04_understandMachine.cpp)
target_include_directories(Understand PRIVATE ${CMAKE_SOURCE_DIR}/include)
set_property(TARGET Understand PROPERTY CXX_STANDARD 23)
