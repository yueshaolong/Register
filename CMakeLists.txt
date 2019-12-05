cmake_minimum_required(VERSION 3.14)
project(Register C)

set(CMAKE_C_STANDARD 11)

add_library(Register SHARED library.c library.h)

add_library(Hello SHARED com_ysl_myjni_Register.h com_ysl_myjni_Register.c)

add_library(DyHello SHARED DyRegister.c)