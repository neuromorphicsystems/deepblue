#pragma once

#include <limits>

namespace tarsier {
    /// update_exposure implements an auto-exposure algorithm.
    /// The algorithm also generates a tone-mapped frame.
    template <typename Time, typename HandleExposureAndFrameDuration, typename HandleTonemappedFrame>
    class update_exposure {
        public:
        update_exposure(
            float exposure_inertia,
            float tonemapping_inertia,
            float maximum_shutter,
            uint32_t maximum_pixel_value,
            Time minimum_exposure,
            Time maximum_exposure,
            Time initial_exposure,
            Time minimum_frame_duration,
            Time maximum_frame_duration,
            Time initial_frame_duration,
            HandleExposureAndFrameDuration&& handle_exposure_and_frame_duration,
            HandleTonemappedFrame&& handle_tonemapped_frame) :
            _exposure_inertia(exposure_inertia),
            _tonemapping_inertia(tonemapping_inertia),
            _maximum_shutter(maximum_shutter),
            _maximum_pixel_value(maximum_pixel_value),
            _minimum_exposure(minimum_exposure),
            _maximum_exposure(maximum_exposure),
            _minimum_frame_duration(minimum_frame_duration),
            _maximum_frame_duration(maximum_frame_duration),
            _exposure(initial_exposure),
            _frame_duration(initial_frame_duration),
            _handle_exposure_and_frame_duration(
                std::forward<HandleExposureAndFrameDuration>(handle_exposure_and_frame_duration)),
            _handle_tonemapped_frame(std::forward<HandleTonemappedFrame>(handle_tonemapped_frame)),
            _slope(1.0f / maximum_pixel_value),
            _intercept(0.0f) {
            if (_minimum_exposure > _maximum_exposure) {
                throw std::logic_error("the maximum exposure must be larger than the minimum exposure");
            }
            if (_minimum_frame_duration > _maximum_frame_duration) {
                throw std::logic_error("the maximum frame duration must be larger than "
                                       "the minimum frame duration");
            }
            if (_exposure_inertia < 0 || _exposure_inertia > 1) {
                throw std::logic_error("the exposure inertia must be in the range ]0, 1]");
            }
            if (_tonemapping_inertia < 0 || _tonemapping_inertia > 1) {
                throw std::logic_error("the tonemapping inertia must be in the range ]0, 1]");
            }
            if (_maximum_shutter <= 0 || maximum_shutter > 1) {
                throw std::logic_error("the maximum shutter must be in the range ]0, 1]");
            }
        }
        update_exposure(const update_exposure&) = default;
        update_exposure(update_exposure&& other) = default;
        update_exposure& operator=(const update_exposure&) = default;
        update_exposure& operator=(update_exposure&& other) = default;
        virtual ~update_exposure() {}

        /// operator() handles a sensor frame.
        template <typename Pixel>
        void operator()(const std::vector<Pixel>& frame) {
            std::vector<uint32_t> histogram(_maximum_pixel_value, 0);
            for (const auto pixel : frame) {
                ++histogram[pixel];
            }
            Pixel minimum = _maximum_pixel_value + 1;
            Pixel maximum = _maximum_pixel_value + 1;
            {
                uint32_t sum = 0;
                for (uint32_t index = 0; index < _maximum_pixel_value; ++index) {
                    sum += histogram[index];
                    if (minimum == _maximum_pixel_value + 1 && sum > frame.size() / 20) {
                        minimum = index;
                    }
                    if (maximum == _maximum_pixel_value + 1 && sum > frame.size() / 20 * 19) {
                        maximum = index;
                    }
                }
            }
            auto exposure =
                _exposure
                * (1
                   + (1 - _exposure_inertia) * (static_cast<float>(_maximum_pixel_value / 2) - (maximum + minimum) / 2)
                         / _maximum_pixel_value);
            if (exposure < _minimum_exposure) {
                exposure = static_cast<float>(_minimum_exposure);
            } else if (exposure > _maximum_exposure) {
                exposure = static_cast<float>(_maximum_exposure);
            }
            auto frame_duration = exposure / _maximum_shutter;
            if (frame_duration < _minimum_frame_duration) {
                frame_duration = static_cast<float>(_minimum_frame_duration);
            } else if (frame_duration > _maximum_frame_duration) {
                exposure = _maximum_frame_duration * _maximum_shutter;
                frame_duration = static_cast<float>(_maximum_frame_duration);
            }
            _exposure = static_cast<Time>(exposure);
            _frame_duration = static_cast<Time>(frame_duration);
            _handle_exposure_and_frame_duration(_exposure, _frame_duration);
            std::vector<float> tonemapped_frame(frame.size());
            if (minimum < maximum) {
                _slope = _slope * _tonemapping_inertia + 1.0f / (maximum - minimum) * (1 - _tonemapping_inertia);
                _intercept = _intercept * _tonemapping_inertia + (-_slope * minimum) * (1 - _tonemapping_inertia);
                ;
            }
            std::transform(frame.begin(), frame.end(), tonemapped_frame.begin(), [this, minimum, maximum](Pixel pixel) {
                if (pixel < minimum) {
                    return 0.0f;
                }
                if (pixel > maximum) {
                    return 1.0f;
                }
                return _slope * pixel + _intercept;
            });
            _handle_tonemapped_frame(tonemapped_frame);
        }

        protected:
        const float _exposure_inertia;
        const float _tonemapping_inertia;
        const float _maximum_shutter;
        const uint32_t _maximum_pixel_value;
        const Time _minimum_exposure;
        const Time _maximum_exposure;
        const Time _minimum_frame_duration;
        const Time _maximum_frame_duration;
        Time _exposure;
        Time _frame_duration;
        HandleExposureAndFrameDuration _handle_exposure_and_frame_duration;
        HandleTonemappedFrame _handle_tonemapped_frame;
        float _slope;
        float _intercept;
    };
}
