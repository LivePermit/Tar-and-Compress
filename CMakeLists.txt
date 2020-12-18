cmake_minimum_required(VERSION 3.17)
project(FileTarManager)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(FileTarManager
        init.cpp
        init.h
        main.cpp
        mainwindow.cpp
        mainwindow.h
        tar.cpp
        tar.h
        utils.h)
