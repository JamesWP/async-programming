#ifndef CURL_LIBUV
#define CURL_LIBUV

#include <curl/curl.h>
#include <uv.h>
#include <functional>

void curl_libuv_init(uv_loop_t *loop_p, std::function<void(CURL*)> curl_complete_cb);
void curl_libuv_add(CURL *handle);
void curl_libuv_cleanup();


#endif