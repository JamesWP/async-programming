#include <uv.h>
#include <iostream>
#include <iomanip>
#include "curl_libuv.h"
#include "task.hpp"

constexpr const int NUM_REQUESTS = 100;

void curl_libuv_complete(CURL* handle);

// Counts the number of newline characters in `data`.
size_t count_lines(std::string_view data) {
  return std::count(data.begin(), data.end(), '\n');
}

// The callback reads `count` elements of data, each `size` bytes long, from the stream pointed to by `ptr`, and processes them.
size_t write_callback(char *ptr, size_t size, size_t count, void *userdata) {
  std::string& response = *reinterpret_cast<std::string*>(userdata);
  response.append(ptr, size * count);
  return count;
}

struct curl_download {
  std::string_view _url;
  std::string _result;
  bool await_ready() const noexcept { return false; }
  void await_suspend(std::coroutine_handle<> continuation) {
    std::cout << "Requesting: " << std::quoted(_url) << "\n";

    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &_result);
    curl_easy_setopt(handle, CURLOPT_PRIVATE, continuation.address());
    curl_easy_setopt(handle, CURLOPT_URL, _url.data());
    curl_libuv_add(handle);
  }
  std::string await_resume() const { return _result; }
};

task process_download(std::string_view url) {
  std::cout << "About to start download\n";
  auto content = co_await curl_download(url);

  std::cout << "Finished download\n";
  std::cout << count_lines(content) << "\n";
}

task application(std::string_view url){
  std::vector<task> tasks;

  std::cout<<"initial loop\n";
  for(int i = 0; i < NUM_REQUESTS; i++) {
    tasks.emplace_back(process_download(url));
  }

  return task::await_all(tasks);
}

int main(int argc, char* argv[]) {
  uv_loop_t *loop = uv_default_loop();

  curl_libuv_init(loop, curl_libuv_complete);

  std::string url = argc < 2 ? "https://example.com": argv[1];

  auto app = application(url);

  uv_run(loop, UV_RUN_DEFAULT);

  curl_libuv_cleanup();

  return 0;
}

void curl_libuv_complete(CURL* handle) {
  void* d;
  curl_easy_getinfo(handle, CURLINFO_PRIVATE, &d);

  auto c = std::coroutine_handle<>::from_address(d);

  c();
}