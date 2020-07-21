#include <iostream>
#include <uv.h>

void on_timer_fire(uv_timer_t *timer)
{
  std::cout << "Fire!\n";

  uv_timer_start(timer, on_timer_fire, 1000, 0);
}

int main(int argc, char *argv[])
{
  std::cout << "Simple timer demo\n";

  uv_loop_t *loop = uv_default_loop();
  static uv_timer_t timer;
  uv_timer_init(loop, &timer);

  uv_timer_start(&timer, on_timer_fire, 1000, 0);

  signal(SIGINT, [](int) -> void { uv_timer_stop(&timer); });

  int rc = uv_run(loop, UV_RUN_DEFAULT);

  std::cout << "\rExiting\n";

  return rc;
}
