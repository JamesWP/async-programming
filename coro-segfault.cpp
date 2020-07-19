#include <iostream>
#include <coroutine>

class task
{
public:
  struct promise_type;
  using handle = std::coroutine_handle<promise_type>;
  struct promise_type
  {
    std::suspend_never initial_suspend() const noexcept { return {}; }
    std::suspend_never final_suspend() const noexcept { return {}; }
    auto get_return_object() noexcept { return task{handle::from_promise(*this)}; }
    void unhandled_exception() { std::terminate(); }
  };

  ~task()
  {
    if (_coro != nullptr)
    {
      std::cout << "Destroying task\n";
      _coro.destroy();
    }
  }

  task(task const &) = delete;
  task &operator=(task &&other) = delete;

  task(task &&rhs)
  {
    std::cout << "Copied task\n";
    _coro = rhs._coro;
    rhs._coro = nullptr;
  };

private:
  handle _coro = nullptr;
  task(handle coro) : _coro{coro}
  {
    std::cout << "Created task\n";
  }
};

std::coroutine_handle<> resume_handle;

struct pause : std::suspend_always
{
  void await_suspend(std::coroutine_handle<> h)
  {
    resume_handle = h;
  }
};

task go()
{
  co_await pause();

  std::cout << "Finished firing\n";
}

int main(int argc, char *argv[])
{
  auto g = go();

  resume_handle();

  return 0;
}
