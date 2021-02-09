#pragma once

#include <chrono>   // @DEV
#include <iostream> // @DEV
#include <thread>   // @DEV

/// interface controls the external LED and switch.
/// A dummy implementation is provided for non-rpi hosts.
#ifdef RASPBERRY_PI
#include <cstdint>
#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>
class interface {
    public:
    interface() : _memory_file_descriptor(open("/dev/gpiomem", O_RDWR | O_SYNC)), _led_on(true) {
        if (_memory_file_descriptor < 0) {
            throw std::logic_error("'/dev/gpiomem' could not be opened in read and write mode");
        }
        auto map = mmap(nullptr, 180, PROT_READ | PROT_WRITE, MAP_SHARED, _memory_file_descriptor, 0);
        if (map == MAP_FAILED) {
            close(_memory_file_descriptor);
            throw std::logic_error("mmap failed");
        }
        _gpios = reinterpret_cast<volatile uint32_t*>(const_cast<volatile void*>(map));
        //                --999888777666555444333222------
        *(_gpios + 0) = 0b00000000000000000000000000000000u;
        //                --999888777666555444333222111000
        *(_gpios + 1) = 0b00000000000000000000001000000000u;
        //                --------777666555444333222111000
        *(_gpios + 2) = 0b00000000000000000000000000001000u;
        //                 ffeeddccbbaa9988776655443322----
        *(_gpios + 57) = 0b00000000000000000000000000000000u;
        //                 --------bbaa99887766554433221100
        *(_gpios + 58) = 0b00000000000000000000000100000000u;
        *(_gpios + clear_offset) = (1u << led_pin);
    }
    interface(const interface&) = delete;
    interface(interface&& other) = delete;
    interface& operator=(const interface&) = delete;
    interface& operator=(interface&& other) = delete;
    virtual ~interface() {
        *(_gpios + clear_offset) = (1u << led_pin);
        munmap(const_cast<void*>(reinterpret_cast<volatile void*>(_gpios)), 180);
        close(_memory_file_descriptor);
    }

    /// enabled returns true if the external switch is switched on.
    virtual bool enabled() {
        return ((*(_gpios + level_offset) >> switch_pin) & 1) == 0;
    }

    /// blink changes the state of the external LED.
    virtual void blink() {
        if (_led_on) {
            *(_gpios + clear_offset) = (1u << led_pin);
            _led_on = false;
        } else {
            *(_gpios + set_offset) = (1u << led_pin);
            _led_on = true;
        }
    }

    /// on turns on the external LED.
    virtual void on() {
        *(_gpios + set_offset) = (1u << led_pin);
        _led_on = true;
    }

    protected:
    /// set_offset is the gpios set register offset.
    static constexpr uint8_t set_offset = 7;

    /// clear_offset is the gpios clear register offset.
    static constexpr uint8_t clear_offset = 10;

    /// level_offset is the gpios level register offset.
    static constexpr uint8_t level_offset = 13;

    /// led_pin is the external LED pin.
    static constexpr uint32_t led_pin = 21;

    /// switch_pin is the external switch pin.
    static constexpr uint32_t switch_pin = 20;

    int32_t _memory_file_descriptor;
    volatile uint32_t* _gpios;
    bool _led_on;
};
#else
class interface {
    public:
    interface() {}
    interface(const interface&) = delete;
    interface(interface&& other) = delete;
    interface& operator=(const interface&) = delete;
    interface& operator=(interface&& other) = delete;
    virtual ~interface() {}

    /// enabled returns true if the external switch is switched on.
    virtual bool enabled() {
        return true;
    }

    /// blink changes the state of the external LED.
    virtual void blink() {}

    /// on turns on the external LED.
    virtual void on() {}
};
#endif
