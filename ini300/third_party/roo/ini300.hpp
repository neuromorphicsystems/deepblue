#pragma once

#include "camera.hpp"
#include "sepia.hpp"
#include "usb.hpp"

#define sepia_ini300_bias(index, name)                                                                                 \
    if (initialise || camera_parameters.biases.name != _previous_parameters.biases.name) {                             \
        send_parameter(                                                                                                \
            "sending a bias",                                                                                          \
            5,                                                                                                         \
            biases_types[index].address,                                                                               \
            biases_types[index].shift > 0 ?                                                                            \
                (((camera_parameters.biases.name & 0x7ffu) << 4) | biases_types[index].shift) :                        \
                camera_parameters.biases.name);                                                                        \
    }

namespace sepia {
    namespace ini300 {
        /// get_serial reads the serial of an interface.
        std::string get_serial(sepia::usb::interface& interface) {
            return interface.get_string_descriptor_ascii(3);
        }

        /// available_serials returns a list of connected devices serials.
        inline std::vector<std::string> available_serials() {
            return usb::available_serials(5418, 33818, get_serial);
        }

        /// name is the camera model.
        constexpr char name[] = "INI-300";

        /// width is the number of pixel columns.
        constexpr uint16_t width = 346;

        /// height is the number of pixel rows.
        constexpr uint16_t height = 260;

        /// polarity lists the possible states of a polarity filter.
        enum class polarity : uint8_t { none, off, on };

        /// voltage_and_current_to_bias converts a voltage and a current to a bias value.
        /// voltage must be in the range [0, 64[.
        /// current must be in the range [0, 8[.
        constexpr uint16_t voltage_and_current_to_bias(uint8_t voltage, uint8_t current) {
            return static_cast<uint16_t>(
                static_cast<uint16_t>(voltage & 0x3f) | (static_cast<uint16_t>(current & 0x07) << 6));
        }

        /// bias_to_voltage_and_current converts an existing bias to oltage and current values.
        constexpr std::pair<uint8_t, uint8_t> bias_to_voltage_and_current(uint16_t bias) {
            return {static_cast<uint8_t>(bias & 0x3f), static_cast<uint8_t>((bias >> 6) & 0x07)};
        }

        /// parameters lists the camera parameters.
        struct parameters {
            struct {
                uint16_t aps_overflow_level;    // values must be generated with
                                                // voltage_and_current_to_bias
                uint16_t aps_cascode;           // values must be generated with
                                                // voltage_and_current_to_bias
                uint16_t adc_reference_high;    // values must be generated with
                                                // voltage_and_current_to_bias
                uint16_t adc_reference_low;     // values must be generated with
                                                // voltage_and_current_to_bias
                uint16_t adc_test_voltage;      // values must be generated with
                                                // voltage_and_current_to_bias
                uint16_t local_buffer;          // in the range [0, 2048[
                uint16_t pad_follower;          // in the range [0, 2048[
                uint16_t differential;          // in the range [0, 2048[
                uint16_t on_threshold;          // in the range [0, 2048[
                uint16_t off_threshold;         // in the range [0, 2048[
                uint16_t passive_load_inverter; // in the range [0, 2048[
                uint16_t photoreceptor;         // in the range [0, 2048[
                uint16_t follower;              // in the range [0, 2048[
                uint16_t refractory_period;     // in the range [0, 2048[
                uint16_t readout_buffer;        // in the range [0, 2048[
                uint16_t aps_follower;          // in the range [0, 2048[
                uint16_t adc_comparator;        // in the range [0, 2048[
                uint16_t column_selection_low;  // in the range [0, 2048[
                uint16_t dac_buffer;            // in the range [0, 2048[
                uint16_t column_timeout;        // in the range [0, 2048[
                uint16_t arbiter_pulldown;      // in the range [0, 2048[
                uint16_t arbiter_pullup_x;      // in the range [0, 2048[
                uint16_t arbiter_pullup_y;      // in the range [0, 2048[
                uint16_t if_refractory_period;  // in the range [0, 2048[
                uint16_t if_threshold;          // in the range [0, 2048[
                uint16_t bias_buffer;           // in the range [0, 2048[
            } biases;
            struct {
                struct {
                    uint16_t x;
                    uint16_t y;
                    uint16_t width;  // at least 1
                    uint16_t height; // at least 1
                } roi;               // set to {0, 0, width, height} to enable all pixels
                struct {
                    uint16_t x;
                    uint16_t y;
                } filtered_pixels[8];                // set a pixel to {width, height} to disable it
                uint16_t filter_background_activity; // in the range [0, 4096[, in 250 µs units
                                                     // set to 0 to disable the filter
                uint16_t filter_refractory_period;   // in the range [0, 4096[, in 250 µs units
                                                     // set to 0 to disable the filter
                uint16_t filter_nth_event;           // in the range [0, 256]
                                                     // set to 256 to disable the filter
                polarity filter_polarity;
                bool enable_external_aer;
                bool set_all_polarities_to_off;
                bool drop_on_stall;
                bool enable;
            } dvs;
            struct {
                struct {
                    uint16_t x;
                    uint16_t y;
                    uint16_t width;      // at least 1
                    uint16_t height;     // at least 1
                } roi;                   // set to {0, 0, width, height} to enable all pixels
                uint32_t exposure;       // in ADC clock cycles (1 / 62.4 MHz units, or ~16.03 ns)
                uint32_t frame_duration; // in ADC clock cycles (1 / 62.4 MHz units, or ~16.03 ns)
                bool drop_on_stall;
                bool enable;
            } aps;
            struct {
                uint8_t rate_divider;                  // imu rate = gysroscope_low_pass_filter::rate / (1 +
                                                       // rate_divider) Hz
                uint8_t accelerometer_low_pass_filter; // value: {bandwidth, delay, rate}
                                                       // 0: {218.1 Hz,  1.88 ms, 1 kHz}
                                                       // 1: {218.1 Hz,  1.88 ms, 1 kHz}
                                                       // 2: {   99 Hz,  2.88 ms, 1 kHz}
                                                       // 3: { 44.8 Hz,  4.88 ms, 1 kHz}
                                                       // 4: { 21.2 Hz,  8.87 ms, 1 kHz}
                                                       // 5: { 10.2 Hz, 16.83 ms, 1 kHz}
                                                       // 6: { 5.05 Hz, 32.48 ms, 1 kHz}
                                                       // 7: {  420 Hz,  1.38 ms, 1 kHz}
                uint8_t accelerometer_range;           // 0: ±2g
                                                       // 1: ±4g
                                                       // 2: ±8g
                                                       // 3: ±16g
                bool enable_accelerometer;
                uint8_t gysroscope_low_pass_filter; // value: {bandwidth, delay, rate}
                                                    // 0: { 250 Hz,  0.97 ms, 8 kHz}
                                                    // 1: { 184 Hz,   2.9 ms, 1 kHz}
                                                    // 2: {  92 Hz,   3.9 ms, 1 kHz}
                                                    // 3: {  41 Hz,   5.9 ms, 1 kHz}
                                                    // 4: {  20 Hz,   9.9 ms, 1 kHz}
                                                    // 5: {  10 Hz, 17.85 ms, 1 kHz}
                                                    // 6: {   5 Hz, 33.48 ms, 1 kHz}
                                                    // 7: {3600 Hz,  0.17 ms, 8 kHz}
                uint8_t gyroscope_range;            // 0: ±250 °/s
                                                    // 1: ±500 °/s
                                                    // 2: ±1000 °/s
                                                    // 3: ±2000 °/s
                bool enable_gyroscope;
                bool enable_temperature;
            } imu;
            struct {
                uint32_t pulse_length; // in logic clock cycles (1 / 104 MHz units, or ~9.62 ns)
                polarity pulse_polarity;
                bool detect_rising_edges;
                bool detect_falling_edges;
                bool drop_on_stall;
                bool enable;
            } trigger;
            uint32_t usb_early_packet_delay; // in USB clock cycles (1 / 83.2 MHz units,
                                             // or ~12.02 ns)
            bool reset_timestamp;
        };

        struct frame_event {
            uint64_t exposure_begin_t;
            uint64_t exposure_end_t;
            uint64_t t;
            uint16_t x;
            uint16_t y;
            uint16_t width;
            uint16_t height;
            std::vector<uint16_t> lumas;
        };

        SEPIA_PACK(struct imu_event {
            uint64_t t;
            float ax; // m.s⁻²
            float ay; // m.s⁻²
            float az; // m.s⁻²
            float gx; // rad.s⁻¹
            float gy; // rad.s⁻¹
            float gz; // rad.s⁻¹
            float temperature; // ºC
        });

        SEPIA_PACK(struct trigger_event{

        });

        /// default_parameters provides camera parameters tuned for standard use.
        constexpr parameters default_parameters{
            {voltage_and_current_to_bias(27, 6),
             voltage_and_current_to_bias(21, 6),
             voltage_and_current_to_bias(32, 7),
             voltage_and_current_to_bias(1, 7),
             voltage_and_current_to_bias(21, 7),
             1444,
             2007,
             1063,
             1535,
             1025,
             1680,
             570, // photoreceptor
             272,
             1049, // refractory_period
             1556, // readout_buffer
             1755,
             1300,
             1, // column_selection_low
             1596,
             1329,
             1627,
             1104, // arbiter_pullup_x
             1944, // arbiter_pullup_y
             1535,
             1535,
             1534},
            {{0, 0, width, height},
             {{width, height},
              {width, height},
              {width, height},
              {width, height},
              {width, height},
              {width, height},
              {width, height},
              {width, height}},
             0,
             0,
             256,
             polarity::none,
             false,
             false,
             true,
             true},
            {{0, 0, width, height}, 624000, 6240000, true, true},
            {0, 1, 1, true, 1, 1, true, true},
            {1040, polarity::on, false, false, true, true},
            83200,
            false};

        /// base_camera is a common base type for ini300 cameras.
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

        /// buffered_camera is a buffer observable connected to a ini300 camera.
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
            /// bias_type bundles a bias address and a prefix.
            SEPIA_PACK(struct bias_type {
                uint8_t address;
                uint8_t shift;
            });

            /// biases_types lists the bias types.
            static constexpr std::array<bias_type, 26> biases_types{
                {{0, 0},       {1, 0},       {2, 0},       {3, 0},       {4, 0},       {8, 0b1111},  {9, 0b1011},
                 {10, 0b1111}, {11, 0b1111}, {12, 0b1111}, {13, 0b1111}, {14, 0b1101}, {15, 0b1101}, {16, 0b1101},
                 {17, 0b1101}, {18, 0b1111}, {19, 0b1101}, {20, 0b1111}, {21, 0b1101}, {22, 0b1111}, {23, 0b1111},
                 {24, 0b1101}, {25, 0b1101}, {26, 0b1111}, {27, 0b1111}, {34, 0b1111}}};

            /// initialise is called by constructors.
            virtual void initialise(const std::string& serial, const std::chrono::steady_clock::duration& timeout) {
                _interface = usb::open(name, 5418, 33818, get_serial, serial);
                if ((_interface.descriptor().bcdDevice & 0xff) != 0x06) {
                    throw usb::error("unsupported camera firmware version");
                }
                _interface.checked_control_transfer(
                    "checking the logic version", 0xc0, 0xbf, 6, 0, {0x00, 0x00, 0x00, 0x12});
                _interface.checked_control_transfer(
                    "checking the logic patch version", 0xc0, 0xbf, 6, 7, {0x00, 0x00, 0x00, 0x01});
                _interface.checked_control_transfer(
                    "checking the IMU chip type", 0xc0, 0xbf, 3, 0, {0x00, 0x00, 0x00, 0x02});
                send_parameters(_parameters, true);
                send_parameter("configuring the FPGA global shutter", 5, 142, true);
                send_parameter("configuring the sensor global shutter", 2, 7, true);
                send_parameter("configuring ssp", 5, 35, 0x8410);
                send_parameter("configuring ssp", 5, 36, 0x8410);
                send_parameter("configuring digital MUX 0", 5, 128, 0);
                send_parameter("configuring digital MUX 1", 5, 129, 0);
                send_parameter("configuring digital MUX 2", 5, 130, 0);
                send_parameter("configuring digital MUX 3", 5, 131, 0);
                send_parameter("configuring analog MUX 0", 5, 132, 0);
                send_parameter("configuring analog MUX 1", 5, 133, 0);
                send_parameter("configuring analog MUX 2", 5, 134, 0);
                send_parameter("configuring bias MUX", 5, 135, 0);
                send_parameter("configuring reset neuron", 5, 136, 1);
                send_parameter("configuring calibrate neuron", 5, 137, 0);
                send_parameter("configuring reset test pixel", 5, 138, 1);
                send_parameter("configuring AER n-arrow", 5, 140, 0);
                send_parameter("configuring analog out", 5, 141, 0);
                send_parameter("configuring select gray counter", 5, 143, 1);
                send_parameter("configuring test ADC", 5, 144, 0);
                send_parameter("configuring MUX enable", 0, 0, false);
                send_parameter("configuring MUX timestamp enable", 0, 1, false);
                send_parameter("configuring USB enable", 9, 0, false);
                send_parameter("configuring MUX chip enable", 0, 3, false);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                _interface.halt(0x82);
                send_parameter("configuring MUX chip enable", 0, 3, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                send_parameter("configuring USB enable", 9, 0, true);
                send_parameter("configuring MUX timestamp enable", 0, 1, true);
                send_parameter("configuring MUX enable", 0, 0, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                send_parameter("resetting the timestamp generator", 0, 2, true);
                send_parameter("resetting the timestamp generator", 0, 2, false);
                send_parameters(_parameters, false);
                const auto bulk_timeout =
                    static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
                _loop = std::thread([this, bulk_timeout]() {
                    try {
                        std::vector<uint8_t> buffer;
                        while (this->_running.load(std::memory_order_relaxed)) {
                            buffer.resize(1 << 13);
                            _interface.bulk_transfer_accept_timeout("reading events", 0x82, buffer, bulk_timeout);
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
                sepia_ini300_bias(0, aps_overflow_level);
                sepia_ini300_bias(1, aps_cascode);
                sepia_ini300_bias(2, adc_reference_high);
                sepia_ini300_bias(3, adc_reference_low);
                sepia_ini300_bias(4, adc_test_voltage);
                sepia_ini300_bias(5, local_buffer);
                sepia_ini300_bias(6, pad_follower);
                sepia_ini300_bias(7, differential);
                sepia_ini300_bias(8, on_threshold);
                sepia_ini300_bias(9, off_threshold);
                sepia_ini300_bias(10, passive_load_inverter);
                sepia_ini300_bias(11, photoreceptor);
                sepia_ini300_bias(12, follower);
                sepia_ini300_bias(13, refractory_period);
                sepia_ini300_bias(14, readout_buffer);
                sepia_ini300_bias(15, aps_follower);
                sepia_ini300_bias(16, adc_comparator);
                sepia_ini300_bias(17, column_selection_low);
                sepia_ini300_bias(18, dac_buffer);
                sepia_ini300_bias(19, column_timeout);
                sepia_ini300_bias(20, arbiter_pulldown);
                sepia_ini300_bias(21, arbiter_pullup_x);
                sepia_ini300_bias(22, arbiter_pullup_y);
                sepia_ini300_bias(23, if_refractory_period);
                sepia_ini300_bias(24, if_threshold);
                sepia_ini300_bias(25, bias_buffer);
                if (initialise || camera_parameters.dvs.roi.x != _previous_parameters.dvs.roi.x) {
                    send_parameter("configuring the DVS ROI begin x coordinate", 1, 41, camera_parameters.dvs.roi.x);
                }
                if (initialise || camera_parameters.dvs.roi.y != _previous_parameters.dvs.roi.y
                    || camera_parameters.dvs.roi.height != _previous_parameters.dvs.roi.height) {
                    const auto y = camera_parameters.dvs.roi.y + camera_parameters.dvs.roi.height;
                    send_parameter("configuring the DVS ROI begin y coordinate", 1, 42, y > height ? y : height - y);
                }
                if (initialise || camera_parameters.dvs.roi.x != _previous_parameters.dvs.roi.x
                    || camera_parameters.dvs.roi.width != _previous_parameters.dvs.roi.width) {
                    send_parameter(
                        "configuring the DVS ROI end x coordinate",
                        1,
                        43,
                        camera_parameters.dvs.roi.width - 1 + camera_parameters.dvs.roi.x);
                }
                if (initialise || camera_parameters.dvs.roi.y != _previous_parameters.dvs.roi.y) {
                    send_parameter(
                        "configuring the DVS ROI end y coordinate",
                        1,
                        44,
                        camera_parameters.dvs.roi.y < height ? height - 1 - camera_parameters.dvs.roi.y :
                                                               camera_parameters.dvs.roi.y);
                }
                for (uint8_t index = 0; index < 8; ++index) {
                    if (initialise
                        || camera_parameters.dvs.filtered_pixels[index].y
                               != _previous_parameters.dvs.filtered_pixels[index].y) {
                        send_parameter(
                            "configuring a filtered pixel's x coordinate",
                            1,
                            11 + index * 2,
                            camera_parameters.dvs.filtered_pixels[index].y < height ?
                                height - 1 - camera_parameters.dvs.filtered_pixels[index].y :
                                camera_parameters.dvs.filtered_pixels[index].y);
                    }
                    if (initialise
                        || camera_parameters.dvs.filtered_pixels[index].x
                               != _previous_parameters.dvs.filtered_pixels[index].x) {
                        send_parameter(
                            "configuring a filtered pixel's y coordinate",
                            1,
                            12 + index * 2,
                            camera_parameters.dvs.filtered_pixels[index].x);
                    }
                }
                if (initialise
                    || camera_parameters.dvs.filter_background_activity
                           != _previous_parameters.dvs.filter_background_activity) {
                    send_parameter(
                        "configuring filter background activity",
                        1,
                        31,
                        camera_parameters.dvs.filter_background_activity > 0);
                    send_parameter(
                        "configuring filter background activity time",
                        1,
                        32,
                        camera_parameters.dvs.filter_background_activity & 0x7ffu);
                }
                if (initialise
                    || camera_parameters.dvs.filter_refractory_period
                           != _previous_parameters.dvs.filter_refractory_period) {
                    send_parameter(
                        "configuring filter refractory period",
                        1,
                        33,
                        camera_parameters.dvs.filter_refractory_period > 0);
                    send_parameter(
                        "configuring filter refractory period time",
                        1,
                        34,
                        camera_parameters.dvs.filter_refractory_period & 0x7ffu);
                }

                if (initialise || camera_parameters.dvs.filter_nth_event != _previous_parameters.dvs.filter_nth_event) {
                    send_parameter("configuring filter nth event", 1, 51, camera_parameters.dvs.filter_nth_event < 256);
                    send_parameter(
                        "configuring filter nth event time", 1, 52, camera_parameters.dvs.filter_nth_event & 0xffu);
                }
                if (initialise || camera_parameters.dvs.filter_polarity != _previous_parameters.dvs.filter_polarity) {
                    send_parameter(
                        "configuring filter polarities",
                        1,
                        62,
                        camera_parameters.dvs.filter_polarity != polarity::none);
                    send_parameter(
                        "configuring filter polarities type",
                        1,
                        63,
                        camera_parameters.dvs.filter_polarity == polarity::on);
                }
                if (initialise || camera_parameters.dvs.drop_on_stall != _previous_parameters.dvs.drop_on_stall) {
                    send_parameter("configuring drop DVS events on stall", 0, 5, camera_parameters.dvs.drop_on_stall);
                    send_parameter("configuring pause AER on stall", 1, 4, !camera_parameters.dvs.drop_on_stall);
                }
                if (initialise
                    || camera_parameters.dvs.enable_external_aer != _previous_parameters.dvs.enable_external_aer) {
                    send_parameter("configuring the external AER", 1, 5, camera_parameters.dvs.enable_external_aer);
                    send_parameter(
                        "configuring DVS run", 1, 3, !camera_parameters.dvs.enable_external_aer && !initialise);
                }

                if (initialise
                    || camera_parameters.dvs.set_all_polarities_to_off
                           != _previous_parameters.dvs.set_all_polarities_to_off) {
                    send_parameter(
                        "configuring all polarities to off", 1, 61, camera_parameters.dvs.set_all_polarities_to_off);
                }
                if (initialise) {
                    send_parameter("configuring enable DVS", 1, 3, false);
                } else if (camera_parameters.dvs.enable != _previous_parameters.dvs.enable) {
                    send_parameter("configuring enable DVS", 1, 3, camera_parameters.dvs.enable);
                }
                if (initialise || camera_parameters.aps.roi.x != _previous_parameters.aps.roi.x) {
                    send_parameter("configuring the APS ROI begin x coordinate", 2, 8, camera_parameters.aps.roi.x);
                }
                if (initialise || camera_parameters.aps.roi.y != _previous_parameters.aps.roi.y
                    || camera_parameters.aps.roi.height != _previous_parameters.aps.roi.height) {
                    const auto y = camera_parameters.aps.roi.y + camera_parameters.aps.roi.height;
                    send_parameter("configuring the APS ROI begin y coordinate", 2, 9, y > height ? y : height - y);
                }
                if (initialise || camera_parameters.aps.roi.x != _previous_parameters.aps.roi.x
                    || camera_parameters.aps.roi.width != _previous_parameters.aps.roi.width) {
                    send_parameter(
                        "configuring the APS ROI end x coordinate",
                        2,
                        10,
                        camera_parameters.aps.roi.width - 1 + camera_parameters.aps.roi.x);
                }
                if (initialise || camera_parameters.aps.roi.y != _previous_parameters.aps.roi.y) {
                    send_parameter(
                        "configuring the APS ROI end y coordinate",
                        2,
                        11,
                        camera_parameters.aps.roi.y < height ? height - 1 - camera_parameters.aps.roi.y :
                                                               camera_parameters.aps.roi.y);
                }
                if (initialise || camera_parameters.aps.exposure != _previous_parameters.aps.exposure) {
                    send_parameter("configuring the exposure", 2, 12, camera_parameters.aps.exposure);
                }
                if (initialise || camera_parameters.aps.frame_duration != _previous_parameters.aps.frame_duration) {
                    send_parameter("configuring the frame duration", 2, 13, camera_parameters.aps.frame_duration);
                }
                if (initialise || camera_parameters.aps.drop_on_stall != _previous_parameters.aps.drop_on_stall) {
                    send_parameter("configuring drop APS frames on stall", 2, 5, camera_parameters.aps.drop_on_stall);
                }
                if (initialise) {
                    send_parameter("configuring enable APS", 2, 4, false);
                } else if (camera_parameters.aps.enable != _previous_parameters.aps.enable) {
                    send_parameter("configuring enable APS", 2, 4, camera_parameters.aps.enable);
                }
                if (initialise || camera_parameters.imu.rate_divider != _previous_parameters.imu.rate_divider) {
                    send_parameter("configuring the IMU rate divider", 3, 5, camera_parameters.imu.rate_divider);
                }
                if (initialise
                    || camera_parameters.imu.accelerometer_low_pass_filter
                           != _previous_parameters.imu.accelerometer_low_pass_filter) {
                    send_parameter(
                        "configuring the accelerometer low-pass filter",
                        3,
                        6,
                        camera_parameters.imu.accelerometer_low_pass_filter);
                }
                if (initialise
                    || camera_parameters.imu.accelerometer_range != _previous_parameters.imu.accelerometer_range) {
                    send_parameter(
                        "configuring the accelerometer range", 3, 7, camera_parameters.imu.accelerometer_range);
                }
                if (initialise) {
                    send_parameter("configuring enable accelerometer", 3, 2, false);
                } else if (
                    camera_parameters.imu.enable_accelerometer != _previous_parameters.imu.enable_accelerometer) {
                    send_parameter(
                        "configuring enable accelerometer", 3, 2, camera_parameters.imu.enable_accelerometer);
                }
                if (initialise
                    || camera_parameters.imu.gysroscope_low_pass_filter
                           != _previous_parameters.imu.gysroscope_low_pass_filter) {
                    send_parameter(
                        "configuring the gyroscope low-pass filter",
                        3,
                        9,
                        camera_parameters.imu.gysroscope_low_pass_filter);
                }
                if (initialise || camera_parameters.imu.gyroscope_range != _previous_parameters.imu.gyroscope_range) {
                    send_parameter("configuring the gyroscope range", 3, 10, camera_parameters.imu.gyroscope_range);
                }
                if (initialise) {
                    send_parameter("configuring enable gyroscope", 3, 3, false);
                } else if (camera_parameters.imu.enable_gyroscope != _previous_parameters.imu.enable_gyroscope) {
                    send_parameter("configuring enable gyroscope", 3, 3, camera_parameters.imu.enable_gyroscope);
                }
                if (initialise) {
                    send_parameter("configuring enable temperature", 3, 4, false);
                } else if (camera_parameters.imu.enable_temperature != _previous_parameters.imu.enable_temperature) {
                    send_parameter("configuring enable temperature", 3, 4, camera_parameters.imu.enable_temperature);
                }
                if (initialise || camera_parameters.trigger.pulse_length != _previous_parameters.trigger.pulse_length) {
                    send_parameter("configuring the pulse length", 4, 5, camera_parameters.trigger.pulse_length);
                }
                if (initialise
                    || camera_parameters.trigger.pulse_polarity != _previous_parameters.trigger.pulse_polarity) {
                    send_parameter(
                        "configuring the pulse polarity",
                        4,
                        3,
                        camera_parameters.trigger.pulse_polarity != polarity::none);
                    send_parameter(
                        "configuring the pulse polarity type",
                        4,
                        4,
                        camera_parameters.trigger.pulse_polarity == polarity::on);
                }
                if (initialise
                    || camera_parameters.trigger.detect_rising_edges
                           != _previous_parameters.trigger.detect_rising_edges) {
                    send_parameter(
                        "configuring detect rising edges", 4, 1, camera_parameters.trigger.detect_rising_edges);
                }
                if (initialise
                    || camera_parameters.trigger.detect_falling_edges
                           != _previous_parameters.trigger.detect_falling_edges) {
                    send_parameter(
                        "configuring detect falling edges", 4, 2, camera_parameters.trigger.detect_falling_edges);
                }
                if (initialise
                    || camera_parameters.trigger.drop_on_stall != _previous_parameters.trigger.drop_on_stall) {
                    send_parameter(
                        "configuring drop external events on stall", 0, 4, camera_parameters.trigger.drop_on_stall);
                }
                if (initialise) {
                    send_parameter("configuring enable trigger", 4, 0, false);
                } else if (camera_parameters.trigger.enable != _previous_parameters.trigger.enable) {
                    send_parameter("configuring enable trigger", 4, 0, camera_parameters.trigger.enable);
                }
                if (initialise
                    || camera_parameters.usb_early_packet_delay != _previous_parameters.usb_early_packet_delay) {
                    send_parameter(
                        "configuring the early USB packet delay", 9, 1, camera_parameters.usb_early_packet_delay);
                }
                if (!initialise && camera_parameters.reset_timestamp) {
                    send_parameter("resetting the timestamp generator", 0, 2, true);
                    send_parameter("resetting the timestamp generator", 0, 2, false);
                }
                _previous_parameters = camera_parameters;
                if (initialise) {
                    _previous_parameters.dvs.enable = false;
                    _previous_parameters.aps.enable = false;
                    _previous_parameters.imu.enable_accelerometer = false;
                    _previous_parameters.imu.enable_gyroscope = false;
                    _previous_parameters.imu.enable_temperature = false;
                    _previous_parameters.trigger.enable = false;
                }
                _previous_parameters.reset_timestamp = false;
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
        template <
            typename HandleDvs,
            typename HandleFrame,
            typename HandleImu,
            typename HandleTrigger,
            typename AfterBuffer>
        class decode {
            public:
            decode(
                HandleDvs&& handle_event,
                HandleFrame&& handle_frame,
                HandleImu&& handle_imu,
                HandleTrigger&& handle_trigger,
                AfterBuffer&& after_buffer) :
                _handle_dvs(std::forward<HandleDvs>(handle_event)),
                _handle_frame(std::forward<HandleFrame>(handle_frame)),
                _handle_imu(std::forward<HandleImu>(handle_imu)),
                _handle_trigger(std::forward<HandleTrigger>(handle_trigger)),
                _after_buffer(std::forward<AfterBuffer>(after_buffer)),
                _offset_t(0),
                _event({0, 0, height, false}),
                _frame_event({0, 0, 0, 0, 0, width, height, std::vector<uint16_t>(width * height, 0)}),
                _frame_reset_x(0),
                _frame_reset_y(0),
                _frame_signal_x(0),
                _frame_signal_y(0),
                _frame_x_begin(0),
                _frame_x_end(0),
                _frame_y_begin(0),
                _frame_y_end(0),
                _frame_roi_index(0),
                _frame_signal_readout(false),
                _accelerometer_scale(2.0f / 16384.0f),
                _gyroscope_scale(2.0f / 131.072f),
                _temperature_enabled(true),
                _gyroscope_enabled(true),
                _accelerometer_enabled(true),
                _imu_state(15),
                _imu_cache(0) {}
            decode(const decode&) = default;
            decode(decode&& other) = default;
            decode& operator=(const decode&) = default;
            decode& operator=(decode&& other) = default;
            virtual ~decode() {}

            /// operator() decodes a buffer of bytes.
            virtual void operator()(const std::vector<uint8_t>& buffer) {
                for (std::size_t index = 0; index < (buffer.size() / 2) * 2; index += 2) {
                    if ((buffer[index + 1] >> 7) == 1) {
                        _event.t = _offset_t
                                   + (static_cast<uint64_t>(buffer[index])
                                      | static_cast<uint64_t>((buffer[index + 1] & 0x7f) << 8));
                    } else {
                        const auto type = static_cast<uint8_t>((buffer[index + 1] & 0b01110000) >> 4);
                        const auto data = static_cast<uint16_t>(
                            static_cast<uint16_t>(buffer[index])
                            | static_cast<uint16_t>((buffer[index + 1] & 0xf) << 8));
                        switch (type) {
                            case 0: {
                                switch (data) {
                                    case 1: {
                                        _offset_t = 0;
                                        break;
                                    }
                                    case 5:
                                        if (_accelerometer_enabled) {
                                            _imu_state = 0;
                                        } else if (_temperature_enabled) {
                                            _imu_state = 6;
                                        } else if (_gyroscope_enabled) {
                                            _imu_state = 8;
                                        } else {
                                            _imu_state = 15;
                                        }
                                        _imu_cache = 0;
                                        break;
                                    case 7:
                                        if (_imu_state == 14) {
                                            _imu_event.t = _event.t;
                                            _handle_imu(_imu_event);
                                            ++_imu_state;
                                        }
                                        break;
                                    case 8:
                                    case 9:
                                        _frame_event.x = _frame_x_begin;
                                        _frame_event.y = height - 1 - _frame_y_end;
                                        _frame_event.width = _frame_x_end - _frame_x_begin + 1;
                                        _frame_event.height = _frame_y_end - _frame_y_begin + 1;
                                        _frame_event.lumas.resize(_frame_event.width * _frame_event.height);
                                        std::fill(_frame_event.lumas.begin(), _frame_event.lumas.end(), 0);
                                        _frame_reset_x = _frame_x_begin;
                                        _frame_reset_y = _frame_y_begin;
                                        _frame_signal_x = _frame_x_begin;
                                        _frame_signal_y = _frame_y_begin;
                                        _frame_signal_readout = false;
                                        break;
                                    case 10:
                                        _frame_event.t = _event.t;
                                        if (_frame_event.exposure_end_t >= _frame_event.exposure_begin_t) {
                                            _handle_frame(_frame_event);
                                        }
                                        break;
                                    case 11:
                                        _frame_reset_x = _frame_x_begin;
                                        _frame_signal_readout = false;
                                        break;
                                    case 12:
                                        _frame_signal_x = _frame_x_begin;
                                        _frame_signal_readout = true;
                                        break;
                                    case 13:
                                        if (_frame_signal_readout) {
                                            if (_frame_signal_y <= _frame_y_end) {
                                                ++_frame_signal_y;
                                            }
                                        } else if (_frame_reset_y <= _frame_y_end) {
                                            ++_frame_reset_y;
                                        }
                                        break;
                                    case 14:
                                        _frame_event.exposure_begin_t = _event.t;
                                        break;
                                    case 15:
                                        _frame_event.exposure_end_t = _event.t;
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            }
                            case 1: {
                                _event.x = data;
                                break;
                            }
                            case 2:
                            case 3: {
                                if (_event.x < width && data < height) {
                                    _event.y = height - 1 - data;
                                    _event.on = type == 3;
                                    _handle_dvs(_event);
                                }
                                break;
                            }
                            case 4: {
                                if (_frame_signal_readout) {
                                    if (_frame_signal_x <= _frame_x_end && _frame_signal_y <= _frame_y_end) {
                                        auto& luma = _frame_event.lumas
                                                         [_frame_x_end - _frame_signal_x
                                                          + (_frame_y_end - _frame_signal_y) * _frame_event.width];
                                        if (luma < 384 || data == 0) {
                                            luma = 1023;
                                        } else if (data > luma) {
                                            luma = 0;
                                        } else {
                                            luma -= data;
                                            if (luma > 1023) {
                                                luma = 1023;
                                            }
                                        }
                                        ++_frame_signal_x;
                                    }
                                } else if (_frame_reset_x <= _frame_x_end && _frame_reset_y <= _frame_y_end) {
                                    _frame_event.lumas
                                        [_frame_x_end - _frame_reset_x
                                         + (_frame_y_end - _frame_reset_y) * _frame_event.width] = data;
                                    ++_frame_reset_x;
                                }
                                break;
                            }
                            case 5: {
                                switch ((data & 0xf00) >> 8) {
                                    case 0:
                                        switch (_imu_state) {
                                            case 0:
                                            case 2:
                                            case 4:
                                            case 6:
                                            case 8:
                                            case 10:
                                            case 12:
                                                _imu_cache =
                                                    static_cast<uint16_t>(static_cast<uint16_t>(data & 0xff) << 8);
                                                ++_imu_state;
                                                break;
                                            case 1:
                                                _imu_event.ax =
                                                    static_cast<float>(static_cast<int16_t>(_imu_cache | (data & 0xff)))
                                                    * _accelerometer_scale;
                                                ++_imu_state;
                                                break;
                                            case 3:
                                                _imu_event.ay =
                                                    static_cast<float>(static_cast<int16_t>(_imu_cache | (data & 0xff)))
                                                    * _accelerometer_scale;
                                                ++_imu_state;
                                                break;
                                            case 5:
                                                _imu_event.az =
                                                    static_cast<float>(static_cast<int16_t>(_imu_cache | (data & 0xff)))
                                                    * _accelerometer_scale;
                                                if (_temperature_enabled) {
                                                    ++_imu_state;
                                                } else if (_gyroscope_enabled) {
                                                    _imu_state = 8;
                                                } else {
                                                    _imu_state = 14;
                                                }
                                                break;
                                            case 7:
                                                _imu_event.temperature =
                                                    static_cast<float>(static_cast<int16_t>(_imu_cache | (data & 0xff)))
                                                        / 333.87f
                                                    + 21.0f;
                                                if (_gyroscope_enabled) {
                                                    ++_imu_state;
                                                } else {
                                                    _imu_state = 14;
                                                }
                                                break;
                                            case 9:
                                                _imu_event.gx =
                                                    static_cast<float>(static_cast<int16_t>(_imu_cache | (data & 0xff)))
                                                    * _gyroscope_scale;
                                                ++_imu_state;
                                                break;
                                            case 11:
                                                _imu_event.gy =
                                                    static_cast<float>(static_cast<int16_t>(_imu_cache | (data & 0xff)))
                                                    * _gyroscope_scale;
                                                ++_imu_state;
                                                break;
                                            case 13:
                                                _imu_event.gz =
                                                    static_cast<float>(static_cast<int16_t>(_imu_cache | (data & 0xff)))
                                                    * _gyroscope_scale;
                                                ++_imu_state;
                                                break;
                                            default:
                                                break;
                                        }
                                        break;
                                    case 1:
                                        switch (_frame_roi_index) {
                                            case 0:
                                                _frame_x_begin = static_cast<uint16_t>((data & 0xff) << 8);
                                                break;
                                            case 1:
                                                _frame_y_begin = static_cast<uint16_t>((data & 0xff) << 8);
                                                break;
                                            case 2:
                                                _frame_x_end = static_cast<uint16_t>((data & 0xff) << 8);
                                                break;
                                            case 3:
                                                _frame_y_end = static_cast<uint16_t>((data & 0xff) << 8);
                                                break;
                                        }
                                        break;
                                    case 2:
                                        switch (_frame_roi_index) {
                                            case 0:
                                                _frame_x_begin |= (data & 0xff);
                                                break;
                                            case 1:
                                                _frame_y_begin |= (data & 0xff);
                                                break;
                                            case 2:
                                                _frame_x_end |= (data & 0xff);
                                                break;
                                            case 3:
                                                _frame_y_end |= (data & 0xff);
                                                break;
                                        }
                                        _frame_roi_index = (_frame_roi_index + 1) % 4;
                                        break;
                                    case 3:
                                        _accelerometer_scale = static_cast<float>(1 << ((data >> 2) & 0b11)) / 16384.0f * 9.80665f;
                                        _gyroscope_scale = static_cast<float>(1 << (data & 0b11)) / (131.072f * 180.0f) * static_cast<float>(M_PI);
                                        _temperature_enabled = ((data >> 5) & 0b1 == 1);
                                        _gyroscope_enabled = ((data >> 6) & 0b1 == 1);
                                        _accelerometer_enabled = ((data >> 7) & 0b1 == 1);
                                        if (_accelerometer_enabled) {
                                            _imu_state = 0;
                                        } else if (_temperature_enabled) {
                                            _imu_state = 6;
                                        } else if (_gyroscope_enabled) {
                                            _imu_state = 8;
                                        } else {
                                            _imu_state = 15;
                                        }
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            }
                            case 6: {
                                break;
                            }
                            case 7: {
                                _offset_t += (static_cast<uint64_t>(buffer[index])
                                              | static_cast<uint64_t>((buffer[index + 1] & 0xf) << 8))
                                             * 0x8000;
                                _event.t = _offset_t;
                                break;
                            }
                        }
                    }
                }
                _after_buffer();
            }

            protected:
            HandleDvs _handle_dvs;
            HandleFrame _handle_frame;
            HandleImu _handle_imu;
            HandleTrigger _handle_trigger;
            AfterBuffer _after_buffer;
            uint64_t _offset_t;
            sepia::dvs_event _event;
            frame_event _frame_event;
            imu_event _imu_event;
            uint16_t _frame_reset_x;
            uint16_t _frame_reset_y;
            uint16_t _frame_signal_x;
            uint16_t _frame_signal_y;
            uint16_t _frame_x_begin;
            uint16_t _frame_x_end;
            uint16_t _frame_y_begin;
            uint16_t _frame_y_end;
            uint8_t _frame_roi_index;
            bool _frame_signal_readout;
            float _accelerometer_scale;
            float _gyroscope_scale;
            bool _temperature_enabled;
            bool _gyroscope_enabled;
            bool _accelerometer_enabled;
            uint8_t _imu_state;
            uint16_t _imu_cache;
        };
    }
}
