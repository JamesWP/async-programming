cmake_minimum_required(VERSION 3.13)
project(async)

find_path(
    CURL_INCLUDE
    NAMES "curl/curl.h"
)
find_path(
    LIBUV_INCLUDE
    NAMES "curl/curl.h"
)

add_definitions("-DNUM_REQUESTS=100")

add_library(curl_libuv curl_libuv.cpp)
target_include_directories(curl_libuv PUBLIC ${CMAKE_SOURCE_DIR} ${CURL_INCLUDE} ${LIBUV_INCLUDE})
target_link_libraries(curl_libuv PRIVATE curl uv)
set_target_properties(curl_libuv PROPERTIES CXX_STANDARD 17)

add_executable(single_threaded 01_single_threaded.cpp curl_libuv.cpp)
target_link_libraries(single_threaded PRIVATE curl_libuv)
set_target_properties(single_threaded PROPERTIES CXX_STANDARD 17)

add_executable(multi_threaded 02_multi_threaded.cpp curl_libuv.cpp)
target_link_libraries(multi_threaded PRIVATE curl_libuv)
set_target_properties(multi_threaded PROPERTIES CXX_STANDARD 17)

add_executable(concurrent_callbacks 03_concurrent_callbacks.cpp curl_libuv.cpp)
target_link_libraries(concurrent_callbacks PRIVATE curl_libuv)
set_target_properties(concurrent_callbacks PROPERTIES CXX_STANDARD 17)

add_executable(concurrent_coroutines 04_concurrent_coro.cpp curl_libuv.cpp)
target_link_libraries(concurrent_coroutines PRIVATE curl_libuv)
set_target_properties(concurrent_coroutines PROPERTIES CXX_STANDARD 20)
target_compile_options(concurrent_coroutines PUBLIC -fcoroutines)
