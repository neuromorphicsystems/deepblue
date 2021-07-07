#pragma once

#include "../third_party/roo/ini300.hpp"

#define ini300_parameters_handler_voltage_current_bias(name)                                                           \
    if (subchannel == "biases:" #name "_voltage") {                                                                    \
        _parameters.biases.name = sepia::ini300::voltage_and_current_to_bias(                                          \
            static_cast<uint8_t>(value), sepia::ini300::bias_to_voltage_and_current(_parameters.biases.name).second);  \
    } else if (subchannel == "biases:" #name "_current") {                                                             \
        _parameters.biases.name = sepia::ini300::voltage_and_current_to_bias(                                          \
            sepia::ini300::bias_to_voltage_and_current(_parameters.biases.name).first, static_cast<uint8_t>(value));   \
    }

#define ini300_parameters_handler_parameter(collection, name)                                                          \
    if (subchannel == #collection ":" #name) {                                                                         \
        _parameters.collection.name = value;                                                                           \
    }

#define ini300_parameters_handler_parameter_uint32(collection, name)                                                   \
    if (subchannel == #collection ":" #name) {                                                                         \
        _parameters.collection.name = static_cast<uint32_t>(value);                                                    \
    }

#define ini300_parameters_handler_pixel(index)                                                                         \
    if (subchannel == "dvs:filtered_pixel_" #index) {                                                                  \
        _parameters.dvs.filtered_pixels[index].x = x;                                                                  \
        _parameters.dvs.filtered_pixels[index].y = y;                                                                  \
    }

namespace sepia_sensors {
    class ini300_parameters_handler {
        public:
        ini300_parameters_handler() : _parameters(sepia::ini300::default_parameters) {
            _accessing_parameters.clear(std::memory_order_release);
        }
        ini300_parameters_handler(const ini300_parameters_handler&) = delete;
        ini300_parameters_handler(ini300_parameters_handler&& other) = delete;
        ini300_parameters_handler& operator=(const ini300_parameters_handler&) = delete;
        ini300_parameters_handler& operator=(ini300_parameters_handler&& other) = delete;
        ~ini300_parameters_handler() {}

        std::string sensor() const {
            return "ini300";
        }

        void handle_int(const std::string& subchannel, int64_t value) {
            while (_accessing_parameters.test_and_set(std::memory_order_acquire)) {
            }
            ini300_parameters_handler_voltage_current_bias(aps_overflow_level);
            ini300_parameters_handler_voltage_current_bias(aps_cascode);
            ini300_parameters_handler_voltage_current_bias(adc_reference_high);
            ini300_parameters_handler_voltage_current_bias(adc_reference_low);
            ini300_parameters_handler_voltage_current_bias(adc_test_voltage);
            ini300_parameters_handler_parameter(biases, local_buffer);
            ini300_parameters_handler_parameter(biases, pad_follower);
            ini300_parameters_handler_parameter(biases, differential);
            ini300_parameters_handler_parameter(biases, on_threshold);
            ini300_parameters_handler_parameter(biases, off_threshold);
            ini300_parameters_handler_parameter(biases, passive_load_inverter);
            ini300_parameters_handler_parameter(biases, photoreceptor);
            ini300_parameters_handler_parameter(biases, follower);
            ini300_parameters_handler_parameter(biases, refractory_period);
            ini300_parameters_handler_parameter(biases, readout_buffer);
            ini300_parameters_handler_parameter(biases, aps_follower);
            ini300_parameters_handler_parameter(biases, adc_comparator);
            ini300_parameters_handler_parameter(biases, column_selection_low);
            ini300_parameters_handler_parameter(biases, dac_buffer);
            ini300_parameters_handler_parameter(biases, column_timeout);
            ini300_parameters_handler_parameter(biases, arbiter_pulldown);
            ini300_parameters_handler_parameter(biases, arbiter_pullup_x);
            ini300_parameters_handler_parameter(biases, arbiter_pullup_y);
            ini300_parameters_handler_parameter(biases, if_refractory_period);
            ini300_parameters_handler_parameter(biases, if_threshold);
            ini300_parameters_handler_parameter(biases, bias_buffer);
            ini300_parameters_handler_parameter(dvs, filter_background_activity);
            ini300_parameters_handler_parameter(dvs, filter_refractory_period);
            ini300_parameters_handler_parameter(dvs, filter_nth_event);
            ini300_parameters_handler_parameter_uint32(aps, exposure);
            ini300_parameters_handler_parameter_uint32(aps, frame_duration);
            ini300_parameters_handler_parameter(imu, rate_divider);
            ini300_parameters_handler_parameter(imu, accelerometer_low_pass_filter);
            ini300_parameters_handler_parameter(imu, accelerometer_range);
            ini300_parameters_handler_parameter(imu, gysroscope_low_pass_filter);
            ini300_parameters_handler_parameter(imu, gyroscope_range);
            ini300_parameters_handler_parameter_uint32(trigger, pulse_length);
            if (subchannel == "usb_early_packet_delay") {
                _parameters.usb_early_packet_delay = static_cast<uint32_t>(value);
            }
            _accessing_parameters.clear(std::memory_order_release);
        }

        void handle_bool(const std::string& subchannel, bool value) {
            while (_accessing_parameters.test_and_set(std::memory_order_acquire)) {
            }
            ini300_parameters_handler_parameter(dvs, enable_external_aer);
            ini300_parameters_handler_parameter(dvs, set_all_polarities_to_off);
            ini300_parameters_handler_parameter(dvs, drop_on_stall);
            ini300_parameters_handler_parameter(dvs, enable);
            ini300_parameters_handler_parameter(aps, drop_on_stall);
            ini300_parameters_handler_parameter(aps, enable);
            ini300_parameters_handler_parameter(imu, enable_accelerometer);
            ini300_parameters_handler_parameter(imu, enable_gyroscope);
            ini300_parameters_handler_parameter(imu, enable_temperature);
            ini300_parameters_handler_parameter(trigger, detect_rising_edges);
            ini300_parameters_handler_parameter(trigger, detect_falling_edges);
            ini300_parameters_handler_parameter(trigger, drop_on_stall);
            ini300_parameters_handler_parameter(trigger, enable);
            _accessing_parameters.clear(std::memory_order_release);
        }

        void handle_enum(const std::string& subchannel, const std::string& value) {
            while (_accessing_parameters.test_and_set(std::memory_order_acquire)) {
            }
            if (subchannel == "dvs:filter_polarity") {
                if (value == "OFF") {
                    _parameters.dvs.filter_polarity = sepia::ini300::polarity::off;
                } else if (value == "ON") {
                    _parameters.dvs.filter_polarity = sepia::ini300::polarity::on;
                } else {
                    _parameters.dvs.filter_polarity = sepia::ini300::polarity::none;
                }
            } else if (subchannel == "trigger:pulse_polarity") {
                if (value == "OFF") {
                    _parameters.trigger.pulse_polarity = sepia::ini300::polarity::off;
                } else if (value == "ON") {
                    _parameters.trigger.pulse_polarity = sepia::ini300::polarity::on;
                } else {
                    _parameters.trigger.pulse_polarity = sepia::ini300::polarity::none;
                }
            }
            _accessing_parameters.clear(std::memory_order_release);
        }

        void handle_pixel(const std::string& subchannel, uint16_t x, uint16_t y) {
            while (_accessing_parameters.test_and_set(std::memory_order_acquire)) {
            }
            ini300_parameters_handler_pixel(0);
            ini300_parameters_handler_pixel(1);
            ini300_parameters_handler_pixel(2);
            ini300_parameters_handler_pixel(3);
            ini300_parameters_handler_pixel(4);
            ini300_parameters_handler_pixel(5);
            ini300_parameters_handler_pixel(6);
            ini300_parameters_handler_pixel(7);
            _accessing_parameters.clear(std::memory_order_release);
        }

        void handle_rectangle(const std::string& subchannel, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
            while (_accessing_parameters.test_and_set(std::memory_order_acquire)) {
            }
            if (subchannel == "dvs:roi") {
                _parameters.dvs.roi.x = x0;
                _parameters.dvs.roi.y = y0;
                _parameters.dvs.roi.width = static_cast<uint16_t>(x1 - x0 + 1);
                _parameters.dvs.roi.height = static_cast<uint16_t>(y1 - y0 + 1);
            } else if (subchannel == "aps:roi") {
                _parameters.aps.roi.x = x0;
                _parameters.aps.roi.y = y0;
                _parameters.aps.roi.width = static_cast<uint16_t>(x1 - x0 + 1);
                _parameters.aps.roi.height = static_cast<uint16_t>(y1 - y0 + 1);
            }
            _accessing_parameters.clear(std::memory_order_release);
        }

        sepia::ini300::parameters parameters() {
            while (_accessing_parameters.test_and_set(std::memory_order_acquire)) {
            }
            const auto copied_parameters = _parameters;
            _accessing_parameters.clear(std::memory_order_release);
            return copied_parameters;
        }

        protected:
        std::atomic_flag _accessing_parameters;
        sepia::ini300::parameters _parameters;
    };
}
