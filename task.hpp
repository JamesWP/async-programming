#ifndef TASK
#define TASK

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
    auto final_suspend() const noexcept { return final_awaitable{}; }

    auto get_return_object() noexcept { return task{handle::from_promise(*this)}; }
    void unhandled_exception() { std::terminate(); }

    void set_continuation(handle h) { _continuation = h; }

    friend struct final_awaitable;

  private:
    handle _continuation;
  };

  struct final_awaitable
  {
    bool await_ready() const noexcept { return false; }

    handle await_suspend(handle coro) noexcept { return coro.promise()._continuation; }

    void await_resume() noexcept {}
  };

  auto operator co_await() const &noexcept
  {
    struct awaitable
    {
      void await_resume() { return; }
      bool await_ready() const noexcept { return !_coro || _coro.done(); }

      handle await_suspend(handle awaiting_handle) noexcept
      {
        _coro.promise().set_continuation(awaiting_handle);
        return _coro;
      }

      awaitable(handle handle) noexcept : _coro(handle) { }

    private:
      std::coroutine_handle<promise_type> _coro;
    };

    return awaitable{_coro};
  }

  task(task const &) = delete;
  task &operator=(task &&other) = delete;

  task(task &&rhs) = delete;

private:
  handle _coro = nullptr;
  task(handle coro) : _coro{coro}
  {
    std::cout << "Created task\n";
  }
};

#endif
