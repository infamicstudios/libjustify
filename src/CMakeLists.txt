add_library(cprintf SHARED cprintf.c)

target_include_directories(cprintf PUBLIC ${CMAKE_SOURCE_DIR}/include)