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

void async_download(const char *url);

int main(int argc, char **argv)
{
  uv_loop_t *loop = uv_default_loop();

  curl_libuv_init(loop);

  std::string url = argc < 2 ? "https://example.com": argv[1];

  for(int i = 0; i < NUM_REQUESTS; i++) {
    async_download(url.c_str());
  }

  uv_run(loop, UV_RUN_DEFAULT);

  curl_libuv_cleanup();

  return 0;
}

// Counts the number of newline characters in `data`.
size_t count_lines(std::string_view data) {
  return std::count(data.begin(), data.end(), '\n');
}

void async_download(const char *url)
{
  std::cout << "Requesting: " << std::quoted(url) << "\n";

  CURL *handle = curl_easy_init();

  curl_easy_setopt(handle, CURLOPT_URL, url);

  auto callback = [](const std::string& response){
    std::cout << "Response lines: " << count_lines(response) << "\n";
  };

  curl_libuv_async(handle, callback);
}