#include <iostream>
#include <coroutine>

class task
{
public:
  struct promise_type;
  struct final_awaitable;
  using handle = std::coroutine_handle<promise_type>;
  struct promise_type
  {
    std::suspend_never initial_suspend() const noexcept { return {}; }
    auto final_suspend() const noexcept {
      struct final_awaitable
      {
        bool await_ready() const noexcept { return false; }
        handle await_suspend(handle coro) noexcept { 
          return coro.promise()._continuation; 
        }
        void await_resume() noexcept {}
      };
      return final_awaitable{}; 
    }
    auto get_return_object() noexcept { return task{handle::from_promise(*this)}; }
    void unhandled_exception() { std::terminate(); }

    handle _continuation;
  };
    
  auto operator co_await() const &noexcept
  {
    struct awaitable
    {
      void await_resume() { return; }
      bool await_ready() const noexcept { return !_coro || _coro.done(); }

      handle await_suspend(handle awaiting_handle) noexcept
      {
        _coro.promise()._continuation = awaiting_handle;
        return _coro;
      }

      awaitable(handle handle) noexcept : _coro(handle) { }

    private:
      std::coroutine_handle<promise_type> _coro;
    };

    return awaitable{_coro};
  }

  task(task const &) = delete; // Not implemented for brevity...
  task &operator=(task &&other) = delete;
  task(task &&rhs) = delete;

private:
  handle _coro = nullptr;
  task(handle coro) : _coro{coro} { std::cout << "Created task\n"; }
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
  std::cout << "Go: enter\n";
  co_await pause();
  std::cout << "Go: exit\n";
}

task other()
{
  std::cout << "Other: enter\n";
  co_await go();
  std::cout << "Other: exit\n";
}

task other2()
{
  std::cout << "Other2: enter\n";
  co_await other();
  std::cout << "Other2: exit\n";
}

int main(int argc, char *argv[])
{
  auto g = other2();

  resume_handle();

  return 0;
}