cmake_minimum_required(VERSION 3.15)
project(SearchEngine C)

set(CMAKE_C_STANDARD 99)

add_executable(SearchEngine main.c hash.c hash.h utils.c utils.h bool_search.h bool_search.c stack.c stack.h free_text_search.c free_text_search.h heapSort.c heapSort.h)