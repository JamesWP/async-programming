#include <iostream>
#include <uv.h>

uv_loop_t *loop;
uv_timer_t timer;

void on_timer_fire(uv_timer_t *timer)
{
  std::cout << "Fire!\n";
}

int main(int argc, char *argv[])
{
  std::cout << "Simple timer demo\n";

  loop = uv_default_loop();

  uv_timer_init(loop, (uv_timer_t *)&timer);

  uv_timer_start(&timer, on_timer_fire, 1000, 500);

  signal(SIGINT, [](int) -> void { uv_timer_stop(&timer); });

  int rc = uv_run(loop, UV_RUN_DEFAULT);

  std::cout << "\rExiting\n";

  return rc;
}
