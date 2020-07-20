#include <iostream>
#include <uv.h>
#include <coroutine>

#include "task.hpp"

struct my_timer : std::suspend_always
{
  struct my_timer_handle
  {
    uv_timer_t timer;
    std::coroutine_handle<> handle;
  };

  my_timer(uv_loop_t *loop, int milliseconds) : _loop{loop}, _milliseconds{milliseconds} {}

  void await_suspend(std::coroutine_handle<> handle)
  {
    my_timer_handle *timer_handle = new my_timer_handle();
    timer_handle->handle = handle;
    uv_timer_init(_loop, std::addressof(timer_handle->timer));
    uv_timer_start(&timer_handle->timer, timer_cb, _milliseconds, 0);
  }

  static void timer_cb(uv_timer_t *t)
  {
    my_timer_handle *handle = (my_timer_handle *)t;
    handle->handle();
    delete handle;
  }

private:
  int _milliseconds{0};
  uv_loop_t *_loop;
};

bool running;

task go(uv_loop_t *loop)
{
  running = true;

  while (running)
  {
    co_await my_timer(loop, 1000);

    std::cout << "go: Fire!\n";
  }
  
  std::cout << "go: Finished firing\n";
}

task go_top(uv_loop_t *loop) {
  std::cout << "go_top: Starting\n";
  co_await go(loop);
  std::cout << "go_top: Done\n";
}

int main(int argc, char *argv[])
{
  std::cout << "Simple timer demo\n";

  uv_loop_t *loop = uv_default_loop();

  auto handle = go_top(loop);

  signal(SIGINT, [](int) -> void { running = false; });

  int rc = uv_run(loop, UV_RUN_DEFAULT);

  std::cout << "\rExiting\n";

  return rc;
}
