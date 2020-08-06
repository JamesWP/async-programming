#ifndef CURL_LIBUV
#define CURL_LIBUV

#include <curl/curl.h>
#include <uv.h>
#include <functional>
#include <string>

void curl_libuv_init(uv_loop_t *loop_p = nullptr);
void curl_libuv_async(CURL *handle, std::function<void(const std::string&)> callback);
std::string curl_libuv_sync(CURL *handle);
void curl_libuv_cleanup();

#endif
