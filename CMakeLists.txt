##
## Copyright (c) 2025 Théo Garcia
## Licensed under the MIT License
##

cmake_minimum_required(VERSION 3.0)

project(hestia)

include_directories(
        include
)

add_executable(hestia
        src/main.c
        src/connection.c
        include/hestia.h
        include/return_code.h
        src/password.c
)

target_link_libraries(hestia PRIVATE sodium)