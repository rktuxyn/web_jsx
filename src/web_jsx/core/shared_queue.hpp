/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
//11:42 AM 3/4/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined( _shared_queue_h)
#	define _shared_queue_h
#if defined(WEB_JSX_MULTI_THREAD)
#   include <condition_variable>
#   include <mutex>
#   include <queue>
#   include <stdexcept>
//9:13 PM 3/15/2020
template <typename T>
class shared_queue {
private:
    std::mutex _mx;
    std::condition_variable _waitrd;
    std::condition_variable _waitwr;
    std::queue<T> _queue;
    size_t _capacity;
public:
    shared_queue(
        size_t max_items_per_thread = 1,
        unsigned int concurrency = std::thread::hardware_concurrency()
    ) : _capacity{ concurrency * max_items_per_thread } {
        if (not concurrency)
            throw std::invalid_argument("Concurrency must be non-zero");
        if (not max_items_per_thread)
            throw std::invalid_argument("Max items per thread must be non-zero");
    }
    shared_queue(shared_queue&&) = default;
    shared_queue& operator=(shared_queue&&) = delete;
    ~shared_queue() {  }
    void try_queue(T && value) {
        std::unique_lock<std::mutex> lock(_mx);
        while (_queue.size() >= _capacity) _waitwr.wait(lock);
        _queue.push(std::forward<T>(value));
        _waitrd.notify_one();//notify to try_deque
    }
    template <typename Function>
    void try_deque(Function process) {
        std::unique_lock<std::mutex> lock(_mx);
        while (true) {
            if (not _queue.empty()) {
                T item{ std::move(_queue.front()) };
                _queue.pop();
                _waitwr.notify_one();//notify to try_queue
                lock.unlock();
                process(item);
                lock.lock();
            }
            else {
                _waitrd.wait(lock);
            }
        }
    }
};
#endif//!WEB_JSX_MULTI_THREAD
#endif//!_shared_queue_h