#ifndef TASK
#define TASK

#include <coroutine>

class [[nodiscard]] task
{
public:
  struct promise_type;
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

    auto get_return_object() noexcept { return task{*this}; }
    void unhandled_exception() { std::terminate(); }

    void set_continuation(std::coroutine_handle<> h) { _continuation = h; }

    friend struct final_awaitable;

  private:
    std::coroutine_handle<> _continuation;
  };

  auto operator co_await() const &noexcept
  {
    struct awaitable
    {
      void await_resume() { return; }
      bool await_ready() const noexcept { return !_coro || _coro.done(); }

      handle await_suspend(std::coroutine_handle<> awaiting_handle) noexcept
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

  void start(std::coroutine_handle<> continuation = std::noop_coroutine()) const {
    _coro.promise().set_continuation(continuation);
    _coro();
  }

  task(task const &) = delete;
  task(task &&rhs):_coro{rhs._coro} { rhs._coro = nullptr; };
  task &operator=(task &&rhs) { if(_coro) { _coro.destroy(); } _coro = rhs._coro; rhs._coro = nullptr; return *this; };

  ~task(){ if(_coro) { _coro.destroy(); } }

private:
  handle _coro = nullptr;
  task(promise_type& promise) : _coro{handle::from_promise(promise)} { }
};

#endif
