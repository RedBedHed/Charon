cmake_minimum_required(VERSION 3.17)
project(Charon)

set(CMAKE_CXX_STANDARD 20)

add_executable(Charon main.cpp ChaosMagic.cpp ChaosMagic.h Move.cpp Move.h List.cpp List.h UniqueList.cpp UniqueList.h Board.cpp Board.h LiteralList.cpp LiteralList.h Position.cpp Position.h Rays.h Utility.h Paths.h MoveMake.cpp MoveMake.h)