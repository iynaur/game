cmake_minimum_required(VERSION 3.5)

project(xlib LANGUAGES C)

add_executable(xlib hello.c)

target_link_libraries(xlib
    -lX11
    -lpthread
    )
