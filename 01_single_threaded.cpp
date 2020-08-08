#include <curl/curl.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <string_view>
#include <algorithm>

#include "curl_libuv.h"

// Perform HTTP get syncronously
void single_thread(std::string_view url);

int main(int argc, char* argv[])
{
  curl_libuv_init();

  std::string url = argc < 2 ? "https://example.com": argv[1];

  for(int i = 0; i < NUM_REQUESTS; i++) {
    single_thread(url);
  }

  return 0;
}

// Counts the number of newline characters in `data`.
size_t count_lines(std::string_view data) {
  return std::count(data.begin(), data.end(), '\n');
}

void single_thread(std::string_view url) {
  std::cout << "Requesting: " << std::quoted(url) << "\n";

  CURL *curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, url.data());

  std::string response = curl_libuv_sync(curl);

  std::cout << "Response lines: " << count_lines(response) << "\n";
}