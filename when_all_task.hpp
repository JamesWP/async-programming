#ifndef WHEN_ALL_TASK
#define WHEN_ALL_TASK

#include <cassert>

struct when_all_counter {
    when_all_counter(size_t count):_outstanding(count+1){
    }

    bool is_ready() const noexcept { return static_cast<bool>(_handle);}

    // returning true means caller needs to suspend
    // returning false means caller can continue executing
    bool try_await(std::coroutine_handle<> handle) noexcept {
        _handle = handle;
        _outstanding--;
        return _outstanding > 1;
    }

    void notify_awaitable_completed() noexcept {
        _outstanding--;
        if(_outstanding == 0 && _handle) {
            _handle();
        }
    }
private:
    size_t _outstanding = 0;
    std::coroutine_handle<> _handle = nullptr;
};

struct when_all_task_promise {
    using coroutine_handle_t = std::coroutine_handle<when_all_task_promise>;

    std::suspend_always initial_suspend() const noexcept { return {}; }
    auto final_suspend() const noexcept {
        class completion_notifier
        {
        public:

            bool await_ready() const noexcept { return false; }

            void await_suspend(coroutine_handle_t coro) const noexcept {
                coro.promise()._counter->notify_awaitable_completed();
            }

            void await_resume() const noexcept {}

            };

        return completion_notifier{};
    }
    auto get_return_object() noexcept {return coroutine_handle_t::from_promise(*this);}
    void unhandled_exception() { assert(false); }
    auto yield_value(int) const noexcept {
        return final_suspend();
    }

    void start(when_all_counter& counter) {
        _counter = std::addressof(counter);
        auto handle = coroutine_handle_t::from_promise(*this);
        handle();
    }

private:
    when_all_counter *_counter;
};

struct when_all_task {
    using promise_type = when_all_task_promise;
    using coroutine_handle_t = promise_type::coroutine_handle_t;

    when_all_task(coroutine_handle_t handle):_handle(handle){ }

    when_all_task(when_all_task&& other) noexcept
        : _handle(std::exchange(other._handle, coroutine_handle_t{}))
    {}

    when_all_task(const when_all_task&) = delete;
    when_all_task& operator=(const when_all_task&) = delete;

    ~when_all_task() { if (_handle) _handle.destroy(); }

    void start(when_all_counter& counter){
        _handle.promise().start(counter);
    }

private:
    coroutine_handle_t _handle;
};

struct when_all_ready_awaitable {
    when_all_ready_awaitable(std::vector<when_all_task> tasks):_tasks(std::move(tasks)),_counter(_tasks.size()) {}

    auto operator co_await() && noexcept {
        struct awaiter {
            awaiter(when_all_ready_awaitable& awaitable): _awaitable(awaitable) {}
            bool await_ready() const noexcept { return _awaitable.is_ready(); }
            bool await_suspend(std::coroutine_handle<> handle) noexcept { return _awaitable.try_await(handle); }
            void await_resume() noexcept { }
        private:
            when_all_ready_awaitable& _awaitable;
        };
        return awaiter{*this};
    }

private:
    bool is_ready() const noexcept {
        return _counter.is_ready();
    }

    bool try_await(std::coroutine_handle<> handle) noexcept {
        for(auto& task: _tasks) {
            task.start(_counter);
        }
        return _counter.try_await(handle);
    }


    std::vector<when_all_task> _tasks;
    when_all_counter _counter;
};

when_all_task make_when_all_task(task t) {
    co_await t;
}

when_all_ready_awaitable when_all_ready(std::vector<task> tasks) {
    std::vector<when_all_task> ready_tasks;

    ready_tasks.reserve(tasks.size());

    for(auto& task: tasks) {
        ready_tasks.emplace_back(make_when_all_task(std::move(task)));
    }

    return when_all_ready_awaitable(std::move(ready_tasks));
}

#endif