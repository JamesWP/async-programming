cmake_minimum_required(VERSION 3.13)
project(async)

find_package(CURL CONFIG REQUIRED)
find_package(unofficial-libuv CONFIG REQUIRED)

add_executable(single_threaded single_threaded.cpp)
target_link_libraries(single_threaded PRIVATE CURL::libcurl)
set_target_properties(single_threaded PROPERTIES CXX_STANDARD 17)

add_executable(multi_threaded multi_threaded.cpp)
target_link_libraries(multi_threaded PRIVATE CURL::libcurl unofficial::libuv::libuv)
set_target_properties(multi_threaded PROPERTIES CXX_STANDARD 17)

add_executable(curl_libuv_example curl_libuv_example.cpp curl_libuv.cpp)
target_link_libraries(curl_libuv_example PRIVATE CURL::libcurl unofficial::libuv::libuv)
target_include_directories(curl_libuv_example PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(curl_libuv_example PROPERTIES CXX_STANDARD 17)

add_executable(simple_timer simple-timer-async.cpp)
target_link_libraries(simple_timer PRIVATE unofficial::libuv::libuv)
target_include_directories(simple_timer PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(simple_timer PROPERTIES CXX_STANDARD 20)

add_executable(simple_timer_coro simple-timer-coro.cpp)
target_link_libraries(simple_timer_coro PRIVATE unofficial::libuv::libuv)
target_include_directories(simple_timer_coro PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(simple_timer_coro PROPERTIES CXX_STANDARD 20)
target_compile_options(simple_timer_coro PUBLIC -fcoroutines)