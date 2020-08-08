#include <uv.h>
#include <iostream>
#include <iomanip>
#include "curl_libuv.h"
#include "task.hpp"
#include "when_all_task.hpp"

struct curl_download {
  std::string_view _url;
  std::string _result;
  bool await_ready() const noexcept { return false; }
  void await_suspend(std::coroutine_handle<> continuation) {
    std::cout << "Requesting: " << std::quoted(_url) << "\n";

    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, _url.data());

    auto callback = [continuation, this](const std::string& response){
      this->_result = response;
      continuation();
    };

    curl_libuv_async(handle, callback);
  }
  std::string await_resume() const { return _result; }
};

task application(std::string_view url);

int main(int argc, char* argv[]) {
  uv_loop_t *loop = uv_default_loop();

  curl_libuv_init(loop);

  std::string url = argc < 2 ? "https://example.com": argv[1];

  auto app = application(url);
  app.start();

  uv_run(loop, UV_RUN_DEFAULT);

  curl_libuv_cleanup();

  return 0;
}

// Counts the number of newline characters in `data`.
size_t count_lines(std::string_view data) {
  return std::count(data.begin(), data.end(), '\n');
}

task coro_download(std::string_view url) {
  auto content = co_await curl_download(url);

  std::cout << "Response lines: " << count_lines(content) << "\n";
}

task application(std::string_view url){
  std::vector<task> tasks;

  for(int i = 0; i < NUM_REQUESTS; i++) {
    tasks.emplace_back(coro_download(url));
  }

  co_await when_all_ready(std::move(tasks));

}


