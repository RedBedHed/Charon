cmake_minimum_required(VERSION 3.19)
project(Charon)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(Charon
        Board.h
        ChaosMagic.cpp
        ChaosMagic.h
        main.cpp
        Move.h
        MoveMake.cpp
        MoveMake.h
        Paths.h
        Rays.h
        Utility.h Fen.cpp Fen.h)
