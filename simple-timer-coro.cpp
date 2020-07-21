#include <iostream>
#include <uv.h>
#include <coroutine>

#include "task.hpp"

struct my_timer
{
  uv_timer_t *_timer{nullptr};
  int _milliseconds{0};
  bool _active = true;

  bool await_ready() const noexcept { return false; }
  void await_resume() const {}
  void await_suspend(std::coroutine_handle<> handle) const noexcept
  {
    this->_timer->data = (void *)handle.address();
    uv_timer_start(this->_timer, timer_cb, _milliseconds, 0);
  }
  static void timer_cb(uv_timer_t *t)
  {
    std::coroutine_handle<>::from_address(t->data)();
  }
  bool active() const { return _active; }
  void cancel(){ _active = false; uv_timer_stop(_timer); timer_cb(_timer); }
};

task app(const my_timer& timer)
{
  while (timer.active())
  {
    co_await timer;
    std::cout << "Fire!\n";
  }

  std::cout << "Finished firing\n";
}

int main(int argc, char *argv[])
{
  std::cout << "Simple timer demo\n";

  uv_loop_t *loop = uv_default_loop();
  uv_timer_t timer;

  uv_timer_init(loop, &timer);
  
  static auto t = my_timer{&timer, 1000};

  auto handle = app(t);
  handle.start();

  signal(SIGINT, [](int) -> void { t.cancel(); });

  int rc = uv_run(loop, UV_RUN_DEFAULT);

  std::cout << "Exiting\n";

  return rc;
}
