cmake_minimum_required(VERSION 3.30)

project(CppProgrammerMindset VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

add_executable(Think src/01_think.cpp)
set_property(TARGET Think PROPERTY CXX_STANDARD 23)

add_executable(Abstraction src/02_abstraction.cpp)
set_property(TARGET Abstraction PROPERTY CXX_STANDARD 23)
