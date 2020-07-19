#ifndef TASK
#define TASK

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

  task &operator=(task &&other)
  {
    if (std::addressof(other) != this)
    {
      if (_coro != nullptr)
      {
        std::cout << "Destroying task via =\n";
        _coro.destroy();
      }

      std::cout << "Copied task via = \n";
      _coro = other._coro;
      other._coro = nullptr;
    }
    return *this;
  };

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

#endif
