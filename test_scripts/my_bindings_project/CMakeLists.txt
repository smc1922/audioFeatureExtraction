cmake_minimum_required(VERSION 3.14)
project(example)

# Find Python and pybind11 include paths
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# compile example.cpp into example to create the Python extension module
pybind11_add_module(example example.cpp)