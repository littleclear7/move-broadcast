cmake_minimum_required(VERSION 3.16)
project(move__broadcast C)

set(CMAKE_C_STANDARD 99)

#使用 sin,cos等函数需要加入libm.so库
link_libraries(m)

add_executable(move__broadcast move-broadcast.c)