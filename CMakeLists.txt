cmake_minimum_required(VERSION 3.26)
project(Implementation)

set(CMAKE_CXX_STANDARD 17)

add_executable(Implementation main.cpp
        Order.cpp
        Order.h
        OrderBook.cpp
        OrderBook.h
        MatchingEngine.cpp
        MatchingEngine.h)
