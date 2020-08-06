#include <curl/curl.h>

#include <uv.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <string_view>
#include <array>
#include <algorithm>

#include "curl_libuv.h"

constexpr const int NUM_REQUESTS = 100;

// Perform HTTP get syncronously
void single_thread(void* arg);

int main(int argc, char* argv[])
{
  curl_libuv_init();

  std::string url = argc < 2 ? "https://example.com": argv[1];

  std::array<uv_thread_t, NUM_REQUESTS> threads;

  for(int i = 0; i < NUM_REQUESTS; i++) {
    uv_thread_create(&threads[i], single_thread, (void*)url.c_str());
  }

  for( int i = 0; i < NUM_REQUESTS; i++) {
    uv_thread_join(&threads[i]);
  }

  return 0;
}

// Counts the number of newline characters in `data`.
size_t count_lines(std::string_view data) {
  return std::count(data.begin(), data.end(), '\n');
}

void single_thread(void* data) {
  const char* url = (const char*)data;

  std::cout << "Requesting: " << std::quoted(url) << "\n";

  CURL *curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, url);

  std::string response = curl_libuv_sync(curl);

  std::cout << "Response lines: " << count_lines(response) << "\n";
}