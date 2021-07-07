#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

#include <iostream> // @DEBUG

namespace sepia {
    /// camera is a common base type for buffered cameras.
    class camera {
        public:
        camera() {}
        camera(const camera&) = default;
        camera(camera&& other) = default;
        camera& operator=(const camera&) = default;
        camera& operator=(camera&& other) = default;
        virtual ~camera() {}
    };

    /// parametric_camera adds parameters update support to camera.
    template <typename Parameters>
    class parametric_camera : public virtual camera {
        public:
        parametric_camera(Parameters parameters) : _parameters(parameters) {}
        parametric_camera(const parametric_camera&) = delete;
        parametric_camera(parametric_camera&& other) = delete;
        parametric_camera& operator=(const parametric_camera&) = delete;
        parametric_camera& operator=(parametric_camera&& other) = delete;
        virtual ~parametric_camera() {}

        /// update_parameters sends a bias update request.
        virtual void update_parameters(const Parameters& parameters) {
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _update_required = true;
                _parameters = parameters;
            }
            _condition_variable.notify_one();
        }

        protected:
        Parameters _parameters;
        bool _update_required;
        std::mutex _mutex;
        std::condition_variable _condition_variable;
    };

    /// fifo implements a thread-safe buffer FIFO.
    class fifo {
        public:
        fifo(const std::chrono::steady_clock::duration& timeout) : _timeout(timeout) {}
        fifo(const fifo&) = delete;
        fifo(fifo&& other) = delete;
        fifo& operator=(const fifo&) = delete;
        fifo& operator=(fifo&& other) = delete;
        virtual ~fifo() {}

        /// pop removes and returns the next buffer.
        /// It returns false if the specified timeout is reached before a buffer is
        /// available.
        virtual bool pop(std::vector<uint8_t>& buffer) {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_buffers.empty()) {
                if (!_condition_variable.wait_for(lock, _timeout, [this] { return !_buffers.empty(); })) {
                    return false;
                }
            }
            buffer.swap(_buffers.front());
            _buffers.pop_front();
            return true;
        }

        /// push inserts a buffer.
        virtual void push(std::vector<uint8_t>& buffer) {
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _buffers.emplace_back();
                buffer.swap(_buffers.back());
            }
            _condition_variable.notify_one();
        }

        protected:
        const std::chrono::steady_clock::duration& _timeout;
        std::deque<std::vector<uint8_t>> _buffers;
        std::mutex _mutex;
        std::condition_variable _condition_variable;
    };

    /// buffered_camera represents a template-specialized generic camera.
    template <typename HandleBuffer, typename HandleException>
    class buffered_camera : public virtual camera {
        public:
        buffered_camera(
            HandleBuffer&& handle_buffer,
            HandleException&& handle_exception,
            const std::chrono::steady_clock::duration& timeout) :
            _handle_buffer(std::forward<HandleBuffer>(handle_buffer)),
            _handle_exception(std::forward<HandleException>(handle_exception)),
            _fifo(timeout),
            _running(true) {
            _buffer_loop = std::thread([this]() {
                try {
                    std::vector<uint8_t> buffer;
                    while (_running.load(std::memory_order_relaxed)) {
                        if (_fifo.pop(buffer)) {
                            _handle_buffer(buffer);
                        }
                    }
                } catch (...) {
                    if (_running.exchange(false)) {
                        _handle_exception(std::current_exception());
                    }
                }
            });
        }
        buffered_camera(const buffered_camera&) = delete;
        buffered_camera(buffered_camera&&) = delete;
        buffered_camera& operator=(const buffered_camera&) = delete;
        buffered_camera& operator=(buffered_camera&&) = delete;
        virtual ~buffered_camera() {
            _running.store(false, std::memory_order_relaxed);
            _buffer_loop.join();
        }

        protected:
        /// push inserts a buffer.
        virtual void push(std::vector<uint8_t>& buffer) {
            _fifo.push(buffer);
        }

        HandleBuffer _handle_buffer;
        HandleException _handle_exception;
        fifo _fifo;
        std::atomic_bool _running;
        std::thread _buffer_loop;
    };
}