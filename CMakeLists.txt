cmake_minimum_required(VERSION 3.19)
project(Charon)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(Charon
        Board.cpp
        Board.h
        ChaosMagic.cpp
        ChaosMagic.h
        List.cpp
        List.h
        main.cpp
        Move.cpp
        Move.h
        MoveMake.cpp
        MoveMake.h
        Paths.h
        Position.cpp
        Position.h
        Rays.h
        UniqueList.cpp
        UniqueList.h
        Utility.h)
