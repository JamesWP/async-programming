#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include <curl/curl.h>

#include <string>
#include <string_view>
#include <iostream>
#include <array>
#include <functional>
#include <iomanip>
#include <algorithm>

#include "curl_libuv.h"

constexpr const int NUM_REQUESTS = 100;

void curl_libuv_complete(CURL* handle);

void add_download(const char *url,  std::function<void(const std::string&)> callback);

// Counts the number of newline characters in `data`.
size_t count_lines(std::string_view data) {
  return std::count(data.begin(), data.end(), '\n');
}

int main(int argc, char **argv)
{
  uv_loop_t *loop = uv_default_loop();

  curl_libuv_init(loop, curl_libuv_complete);

  std::string url = argc < 2 ? "https://example.com": argv[1];

  for(int i = 0; i < NUM_REQUESTS; i++) {
    add_download(url.c_str(), [](const std::string& response){
      std::cout << "Response lines: " << count_lines(response) << "\n";
    });
  }

  uv_run(loop, UV_RUN_DEFAULT);

  curl_libuv_cleanup();

  return 0;
}

// The callback reads `count` elements of data, each `size` bytes long, from the stream pointed to by `ptr`, and processes them.
size_t write_callback(char *ptr, size_t size, size_t count, void *userdata) {
  std::string& response = *reinterpret_cast<std::string*>(userdata);
  response.append(ptr, size * count);
  return count;
}

struct continuation_context {
  using cb = std::function<void(const std::string&)>;

  cb          callback;
  std::string response;
};

void add_download(const char *url,  std::function<void(const std::string&)> callback)
{
  CURL *handle;

  std::cout << "Requesting: " << std::quoted(url) << "\n";

  continuation_context *ctx = new continuation_context();
  ctx->callback = callback;

  handle = curl_easy_init();
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, &ctx->response);
  curl_easy_setopt(handle, CURLOPT_PRIVATE, ctx);
  curl_easy_setopt(handle, CURLOPT_URL, url);
  curl_libuv_add(handle);
}

void curl_libuv_complete(CURL* handle) {
  continuation_context *ctx;
  curl_easy_getinfo(handle, CURLINFO_PRIVATE, &ctx);

  ctx->callback(ctx->response);
  delete ctx;
}
