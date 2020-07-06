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

add_executable(curl_libuv_example curl_libuv_example.cpp)
target_link_libraries(curl_libuv_example PRIVATE CURL::libcurl unofficial::libuv::libuv)
set_target_properties(curl_libuv_example PROPERTIES CXX_STANDARD 17)