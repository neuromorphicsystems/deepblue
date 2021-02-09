#pragma once

#include "camera.hpp"
#include "sepia.hpp"
#include "usb.hpp"
#include <iomanip>
#include <sstream>

namespace sepia {
    namespace ccamatis {
        /// get_serial reads the serial of an interface.
        std::string get_serial(sepia::usb::interface& interface) {
            const auto buffer =
                interface.control_transfer("reading the serial number", 192, 85, 32, 0, std::vector<uint8_t>(8));
            std::stringstream serial;
            serial << std::hex;
            for (uint8_t index = 6; index < 8; ++index) {
                serial << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(buffer[index]);
            }
            return serial.str();
        }

        /// available_serials returns a list of connected devices serials.
        inline std::vector<std::string> available_serials() {
            return usb::available_serials(1204, 244, get_serial);
        }

        /// name is the camera model.
        constexpr char name[] = "Ccam ATIS";

        /// width is the number of pixel columns.
        constexpr uint16_t width = 304;

        /// height is the number of pixel rows.
        constexpr uint16_t height = 240;

        /// parameters lists the camera parameters.
        SEPIA_PACK(struct parameters {
            struct {
                uint8_t cd_reset_switch_bulk_potential;
                uint8_t cd_photoreceptor_feedback;
                uint8_t cd_refractory_period;
                uint8_t cd_follower;
                uint8_t cd_event_source_amplifier;
                uint8_t cd_on_event_threshold;
                uint8_t cd_off_event_threshold;
                uint8_t cd_off_event_inverter;
                uint8_t cd_cascode_photoreceptor_feedback;
                uint8_t em_comparator_tail;
                uint8_t em_comparator_hysteresis;
                uint8_t em_comparator_output_stage;
                uint8_t em_upper_threshold;
                uint8_t em_lower_threshold;
                uint8_t pullup_em_x_request;
                uint8_t pullup_em_y_request;
                uint8_t pullup_cd_x_request;
                uint8_t pullup_cd_y_request;
                uint8_t pullup_x_acknowledge;
                uint8_t pullup_x_encoder;
                uint8_t pullup_y_encoder;
                uint8_t control_em_timeout;
                uint8_t control_sequential_em_timeout;
                uint8_t control_x_acknoledge_timeout;
                uint8_t control_latch_cell_scan_pulldown;
                uint8_t control_x_request_pulldown;
            } biases;
        });

        /// default_parameters provides camera parameters tuned for standard use.
        constexpr parameters default_parameters{{207, 243, 216, 239, 42,  51,  39,  61,  154, 54, 47, 57,  243,
                                                 235, 131, 155, 151, 117, 162, 162, 120, 49,  45, 56, 134, 87}};

        /// base_camera is a common base type for Davis346 cameras.
        class base_camera : public sepia::parametric_camera<parameters> {
            public:
            base_camera(const parameters& camera_parameters) :
                sepia::parametric_camera<parameters>(camera_parameters) {}
            base_camera(const base_camera&) = delete;
            base_camera(base_camera&& other) = delete;
            base_camera& operator=(const base_camera&) = delete;
            base_camera& operator=(base_camera&& other) = delete;
            virtual ~base_camera() {}
        };

        /// buffered_camera is a buffer observable connected to a Davis346 camera.
        template <typename HandleBuffer, typename HandleException>
        class buffered_camera : public base_camera, public sepia::buffered_camera<HandleBuffer, HandleException> {
            public:
            buffered_camera(
                HandleBuffer&& handle_buffer,
                HandleException&& handle_exception,
                const parameters& camera_parameters = default_parameters,
                const std::string& serial = {},
                const std::chrono::steady_clock::duration& timeout = std::chrono::milliseconds(15)) :
                base_camera(camera_parameters),
                sepia::buffered_camera<HandleBuffer, HandleException>(
                    std::forward<HandleBuffer>(handle_buffer),
                    std::forward<HandleException>(handle_exception),
                    timeout) {
                initialise(serial, timeout);
            }
            buffered_camera(
                std::unique_ptr<base_camera>& camera,
                HandleBuffer&& handle_buffer,
                HandleException&& handle_exception,
                const parameters& camera_parameters = default_parameters,
                const std::string& serial = {},
                const std::chrono::steady_clock::duration& timeout = std::chrono::milliseconds(15)) :
                base_camera(camera_parameters),
                sepia::buffered_camera<HandleBuffer, HandleException>(
                    std::forward<HandleBuffer>(handle_buffer),
                    std::forward<HandleException>(handle_exception),
                    timeout) {
                camera.reset(this);
                initialise(serial, timeout);
            }
            buffered_camera(const buffered_camera&) = delete;
            buffered_camera(buffered_camera&& other) = delete;
            buffered_camera& operator=(const buffered_camera&) = delete;
            buffered_camera& operator=(buffered_camera&& other) = delete;
            virtual ~buffered_camera() {
                this->_running.store(false, std::memory_order_relaxed);
                _parameters_loop.join();
                _loop.join();
            }

            protected:
            /// biases_tensions lists the voltage level used by each bias.
            /// false stands for 0x5900, and true for 0x7900.
            static constexpr std::array<bool, 26> biases_voltages{
                {false, false, false, false, true, true,  true,  true, true, true, true, true,  false,
                 false, false, false, false, true, false, false, true, true, true, true, false, true}};

            /// initialise is called by constructors.
            virtual void initialise(const std::string& serial, const std::chrono::steady_clock::duration& timeout) {
                _interface = usb::open(name, 1204, 244, get_serial, serial);
                for (;;) {
                    std::vector<uint8_t> buffer(4, 0);
                    _interface.control_transfer("checking if ready", 0xc0, 0x71, 0x0000, 0x0000, buffer);
                    if (buffer[2] == 1) {
                        break;
                    }
                }
                _interface.control_transfer("setting the role", 0x40, 0x56, 0x001a, 0x0000, {0, 0, 0, 0x01});
                _interface.control_transfer("setting the role", 0x40, 0x56, 0x041a, 0x0000, {0, 0, 0, 0x02});
                send_parameters(_parameters, true);
                _interface.control_transfer("setting the mode", 0x40, 0x56, 0x0008, 0x0000, {0, 0, 0x03, 0x2c});
                _interface.control_transfer("setting the mode", 0x40, 0x56, 0x0408, 0x0000, {0, 0, 0x03, 0x2c});
                {
                    std::vector<uint8_t> buffer(1 << 17);
                    _interface.bulk_transfer_accept_timeout("reading events", 0x81, buffer, 100);
                }
                _interface.control_transfer("starting the camera", 0x40, 0x56, 0x0000, 0x0000, {0, 0, 0x0c, 0x81});
                _interface.control_transfer("starting the camera", 0x40, 0x56, 0x0400, 0x0000, {0, 0, 0x0c, 0x81});
                const auto bulk_timeout =
                    static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
                _loop = std::thread([this, bulk_timeout]() {
                    try {
                        std::vector<uint8_t> buffer;
                        while (this->_running.load(std::memory_order_relaxed)) {
                            buffer.resize(1 << 10);
                            _interface.bulk_transfer_accept_timeout("reading events", 0x81, buffer, bulk_timeout);
                            if (!buffer.empty()) {
                                this->push(buffer);
                            }
                        }
                    } catch (const usb::error&) {
                        if (this->_running.exchange(false)) {
                            this->_handle_exception(std::make_exception_ptr(usb::device_disconnected(name)));
                        }
                    } catch (...) {
                        if (this->_running.exchange(false)) {
                            this->_handle_exception(std::current_exception());
                        }
                    }
                });
                _parameters_loop = std::thread([this, timeout]() {
                    try {
                        while (this->_running.load(std::memory_order_relaxed)) {
                            parameters local_parameters;
                            {
                                std::unique_lock<std::mutex> lock(this->_mutex);
                                if (!this->_update_required) {
                                    if (!this->_condition_variable.wait_for(
                                            lock, timeout, [this] { return this->_update_required; })) {
                                        continue;
                                    }
                                }
                                local_parameters = this->_parameters;
                                this->_update_required = false;
                            }
                            send_parameters(local_parameters, false);
                        }
                    } catch (...) {
                        if (this->_running.exchange(false)) {
                            this->_handle_exception(std::current_exception());
                        }
                    }
                });
            }

            /// send_parameter updates a single camera parameter.
            virtual void
            send_parameter(const std::string& message, uint16_t w_value, uint16_t w_index, uint32_t value) {
                _interface.control_transfer(
                    message,
                    0x40,
                    0xbf,
                    w_value,
                    w_index,
                    {
                        static_cast<uint8_t>((value >> 24) & 0xff),
                        static_cast<uint8_t>((value >> 16) & 0xff),
                        static_cast<uint8_t>((value >> 8) & 0xff),
                        static_cast<uint8_t>(value & 0xff),
                    });
            }

            /// send_parameters updates the camera parameters that changed since the last
            /// call.
            virtual void send_parameters(const parameters& camera_parameters, bool initialise) {
                const std::array<uint8_t, 26> bias_array{{
                    camera_parameters.biases.cd_reset_switch_bulk_potential,
                    camera_parameters.biases.cd_photoreceptor_feedback,
                    camera_parameters.biases.cd_refractory_period,
                    camera_parameters.biases.cd_follower,
                    camera_parameters.biases.cd_event_source_amplifier,
                    camera_parameters.biases.cd_on_event_threshold,
                    camera_parameters.biases.cd_off_event_threshold,
                    camera_parameters.biases.cd_off_event_inverter,
                    camera_parameters.biases.cd_cascode_photoreceptor_feedback,
                    camera_parameters.biases.em_comparator_tail,
                    camera_parameters.biases.em_comparator_hysteresis,
                    camera_parameters.biases.em_comparator_output_stage,
                    camera_parameters.biases.em_upper_threshold,
                    camera_parameters.biases.em_lower_threshold,
                    camera_parameters.biases.pullup_em_x_request,
                    camera_parameters.biases.pullup_em_y_request,
                    camera_parameters.biases.pullup_cd_x_request,
                    camera_parameters.biases.pullup_cd_y_request,
                    camera_parameters.biases.pullup_x_acknowledge,
                    camera_parameters.biases.pullup_x_encoder,
                    camera_parameters.biases.pullup_y_encoder,
                    camera_parameters.biases.control_em_timeout,
                    camera_parameters.biases.control_sequential_em_timeout,
                    camera_parameters.biases.control_x_acknoledge_timeout,
                    camera_parameters.biases.control_latch_cell_scan_pulldown,
                    camera_parameters.biases.control_x_request_pulldown,
                }};
                const std::array<uint8_t, 26> previous_bias_array{{
                    _previous_parameters.biases.cd_reset_switch_bulk_potential,
                    _previous_parameters.biases.cd_photoreceptor_feedback,
                    _previous_parameters.biases.cd_refractory_period,
                    _previous_parameters.biases.cd_follower,
                    _previous_parameters.biases.cd_event_source_amplifier,
                    _previous_parameters.biases.cd_on_event_threshold,
                    _previous_parameters.biases.cd_off_event_threshold,
                    _previous_parameters.biases.cd_off_event_inverter,
                    _previous_parameters.biases.cd_cascode_photoreceptor_feedback,
                    _previous_parameters.biases.em_comparator_tail,
                    _previous_parameters.biases.em_comparator_hysteresis,
                    _previous_parameters.biases.em_comparator_output_stage,
                    _previous_parameters.biases.em_upper_threshold,
                    _previous_parameters.biases.em_lower_threshold,
                    _previous_parameters.biases.pullup_em_x_request,
                    _previous_parameters.biases.pullup_em_y_request,
                    _previous_parameters.biases.pullup_cd_x_request,
                    _previous_parameters.biases.pullup_cd_y_request,
                    _previous_parameters.biases.pullup_x_acknowledge,
                    _previous_parameters.biases.pullup_x_encoder,
                    _previous_parameters.biases.pullup_y_encoder,
                    _previous_parameters.biases.control_em_timeout,
                    _previous_parameters.biases.control_sequential_em_timeout,
                    _previous_parameters.biases.control_x_acknoledge_timeout,
                    _previous_parameters.biases.control_latch_cell_scan_pulldown,
                    _previous_parameters.biases.control_x_request_pulldown,
                }};
                if (initialise || bias_array != previous_bias_array) {
                    std::vector<uint8_t> message;
                    message.reserve(12 * (3 + bias_array.size()));
                    message.insert(
                        message.end(), {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00});
                    message.insert(
                        message.end(), {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0x00, 0x00, 0x00, 0x00, 0x01});
                    for (uint8_t index = 0; index < bias_array.size(); ++index) {
                        message.insert(message.end(), {0, 0, 0, bias_array[index]});
                        message.insert(
                            message.end(), {0, 0, static_cast<uint8_t>(biases_voltages[index] ? 0x79 : 0x59), 0});
                        message.insert(message.end(), {0x00, 0x00, 0x00, static_cast<uint8_t>(index + 2)});
                    }
                    message.insert(
                        message.end(), {0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01c});
                    _interface.control_transfer("loading the biases", 0x40, 0x61, 0x0000, 0x0000, message);
                    _interface.control_transfer("loading the biases", 0x40, 0x62, 0x0000, 0x0000, message);
                    _interface.control_transfer("flushing the biases", 0x40, 0x56, 0x000a, 0x0000, {0, 0, 0, 0x40});
                    _interface.control_transfer("flushing the biases", 0x40, 0x56, 0x040a, 0x0000, {0, 0, 0, 0x40});
                }
                _previous_parameters = camera_parameters;
            }

            usb::interface _interface;
            std::thread _loop;
            std::thread _parameters_loop;
            parameters _previous_parameters;
        };

        /// make_buffered_camera creates a buffered camera from functors.
        template <typename HandleBuffer, typename HandleException>
        std::unique_ptr<buffered_camera<HandleBuffer, HandleException>> make_buffered_camera(
            HandleBuffer&& handle_buffer,
            HandleException&& handle_exception,
            const parameters& camera_parameters = default_parameters,
            const std::string& serial = {},
            const std::chrono::steady_clock::duration& timeout = std::chrono::milliseconds(15)) {
            return std::unique_ptr<buffered_camera<HandleBuffer, HandleException>>(
                new buffered_camera<HandleBuffer, HandleException>(
                    std::forward<HandleBuffer>(handle_buffer),
                    std::forward<HandleException>(handle_exception),
                    camera_parameters,
                    serial,
                    timeout));
        }

        /// decode implements a byte stream decoder for the DAVIS 346.
        template <typename HandleAtis, typename AfterBuffer>
        class decode {
            public:
            decode(HandleAtis&& handle_event, AfterBuffer&& after_buffer) :
                _handle_event(std::forward<HandleAtis>(handle_event)),
                _after_buffer(std::forward<AfterBuffer>(after_buffer)),
                _t_offset(0),
                _warps(0),
                _previous_t(0) {}
            decode(const decode&) = default;
            decode(decode&& other) = default;
            decode& operator=(const decode&) = default;
            decode& operator=(decode&& other) = default;
            virtual ~decode() {}

            /// operator() decodes a buffer of bytes.
            virtual void operator()(const std::vector<uint8_t>& buffer) {
                for (std::size_t index = 0; index < (buffer.size() / 4) * 4; index += 4) {
                    if (buffer[index + 3] == 0x80) {
                        const auto t_offset =
                            (static_cast<uint32_t>(buffer[index]) | (static_cast<uint32_t>(buffer[index + 1]) << 8)
                             | (static_cast<uint32_t>(buffer[index + 2]) << 16))
                            * 0x800;
                        if (t_offset < _t_offset) {
                            ++_warps;
                        }
                        _t_offset = t_offset;
                    } else {
                        sepia::atis_event event{
                            static_cast<uint64_t>(_warps) * (1ull << 32) + _t_offset
                                + ((static_cast<uint16_t>(buffer[index + 3] & 0b1111) << 7) | (buffer[index + 2] >> 1)),
                            static_cast<uint16_t>(
                                (static_cast<uint16_t>(buffer[index + 2] & 1) << 8) | buffer[index + 1]),
                            static_cast<uint16_t>(height - 1 - buffer[index]),
                            ((buffer[index + 3] >> 5) & 0b1) == 1,
                            ((buffer[index + 3] >> 4) & 0b1) == 1};
                        if (event.x < width && event.y < height && event.t >= _previous_t) {
                            _handle_event(event);
                            _previous_t = event.t;
                        }
                    }
                }
                _after_buffer();
            }

            protected:
            HandleAtis _handle_event;
            AfterBuffer _after_buffer;
            uint64_t _t_offset;
            uint32_t _warps;
            uint64_t _previous_t;
        };
    }
}
