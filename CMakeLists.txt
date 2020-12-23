cmake_minimum_required(VERSION 3.16)
project(move__broadcast C)

set(CMAKE_C_STANDARD 99)

#使用 sin,cos等函数需要加入libm.so库
link_libraries(m)
link_libraries(pthread)

add_executable(move__broadcast move-broadcast.c NSP_TDYTH_NAAAS_5G_cJson.c NSP_TDYTH_NAAAS_5G_cJson.h)
add_executable(test_rand test_rand.c)