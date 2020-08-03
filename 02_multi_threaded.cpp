#include <curl/curl.h>

#include <uv.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <string_view>
#include <array>
#include <algorithm>

constexpr const int NUM_REQUESTS = 100;

// Perform HTTP get syncronously
void single_thread(void* arg);

int main(int argc, char* argv[])
{
  if(curl_global_init(CURL_GLOBAL_ALL)) {
    std::cerr << "Could not init curl\n";
    return 1;
  }

  std::string response;
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

// The callback reads `count` elements of data, each `size` bytes long, from the stream pointed to by `ptr`, and processes them.
size_t write_callback(char *ptr, size_t size, size_t count, void *userdata) {
  std::string& response = *reinterpret_cast<std::string*>(userdata);
  response.append(ptr, size * count);
  return count;
}

// Counts the number of newline characters in `data`.
size_t count_lines(std::string_view data) {
  return std::count(data.begin(), data.end(), '\n');
}

void single_thread(void* data) {
  const char* url = (const char*)data;

  std::cout << "Requesting: " << std::quoted(url) << "\n";

  CURL *curl = curl_easy_init();

  if(!curl) {
    exit(1);
  }

  std::string response;

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  CURLcode res = curl_easy_perform(curl);

  if(res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed:" << curl_easy_strerror(res) << "\n";
      exit(1);
  }

  curl_easy_cleanup(curl);

#ifdef VERBOSE
  std::cout << "Response:\n";

  std::cout << response;

  std::cout << "End of response\n";
#endif

  std::cout << "Response lines: " << count_lines(response) << "\n";
}