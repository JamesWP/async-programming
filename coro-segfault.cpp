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
    std::suspend_always initial_suspend() const noexcept { return {}; }
    auto final_suspend() const noexcept {
      struct final_awaitable
      {
        bool await_ready() const noexcept { return false; }
        auto await_suspend(handle coro) noexcept { return coro.promise()._continuation; }
        void await_resume() noexcept {}
      };
      return final_awaitable{}; 
    }
    auto get_return_object() noexcept { return task{handle::from_promise(*this)}; }
    void unhandled_exception() { std::terminate(); }

    std::coroutine_handle<> _continuation;
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

  task(task const &) = delete;
  task &operator=(task &&other) = delete; // Not implemented for brevity...
  task(task&& rhs) : _coro(rhs._coro) { rhs._coro = nullptr; }
  ~task(){ if(_coro) { _coro.destroy(); }}

  void begin(std::coroutine_handle<> continuation = std::noop_coroutine()) {
    _coro.promise()._continuation = continuation;
    _coro();
  }
private:
  handle _coro = nullptr;
  task(handle coro) : _coro{coro} { }
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
  std::cout << "  Go: enter\n";
  co_await pause();
  std::cout << "  Go: exit\n";
}

task other()
{
  std::cout << " Other: enter\n";
  co_await go();
  std::cout << " Other: exit\n";
}

task otherother()
{
  std::cout << "OtherOther: enter\n";
  co_await other();
  co_await other();
  std::cout << "OtherOther: exit\n";
}

int main(int argc, char *argv[])
{
  auto g = otherother();
  g.begin();

  resume_handle();
  resume_handle();

  return 0;
}