#pragma once

#include "../third_party/json.hpp"
#include "../third_party/roo/ccamatis.hpp"

sepia::ccamatis::parameters parse_parameters(std::unique_ptr<std::istream> json_stream) {
    nlohmann::json json;
    (*json_stream) >> json;
    if (!json.is_object()) {
        throw std::runtime_error("parameters' root element must be an object");
    }
    auto parameters = sepia::ccamatis::default_parameters;
    for (auto& [key, value] : json.items()) {
        if (!value.is_number_unsigned()) {
            throw std::runtime_error(
                std::string("'") + key + "' must be associated with an integer in the range [0, 255]");
        }
        uint8_t bias;
        {
            uint64_t bias_as_unsigned = value;
            if (bias_as_unsigned > 255) {
                throw std::runtime_error(
                    std::string("'") + key + "' must be associated with an integer in the range [0, 255]");
            }
            bias = static_cast<uint8_t>(bias_as_unsigned);
        }
        if (key == "cd_reset_switch_bulk_potential") {
            parameters.biases.cd_reset_switch_bulk_potential = bias;
        } else if (key == "cd_photoreceptor_feedback") {
            parameters.biases.cd_photoreceptor_feedback = bias;
        } else if (key == "cd_refractory_period") {
            parameters.biases.cd_refractory_period = bias;
        } else if (key == "cd_follower") {
            parameters.biases.cd_follower = bias;
        } else if (key == "cd_event_source_amplifier") {
            parameters.biases.cd_event_source_amplifier = bias;
        } else if (key == "cd_on_event_threshold") {
            parameters.biases.cd_on_event_threshold = bias;
        } else if (key == "cd_off_event_threshold") {
            parameters.biases.cd_off_event_threshold = bias;
        } else if (key == "cd_off_event_inverter") {
            parameters.biases.cd_off_event_inverter = bias;
        } else if (key == "cd_cascode_photoreceptor_feedback") {
            parameters.biases.cd_cascode_photoreceptor_feedback = bias;
        } else if (key == "em_comparator_tail") {
            parameters.biases.em_comparator_tail = bias;
        } else if (key == "em_comparator_hysteresis") {
            parameters.biases.em_comparator_hysteresis = bias;
        } else if (key == "em_comparator_output_stage") {
            parameters.biases.em_comparator_output_stage = bias;
        } else if (key == "em_upper_threshold") {
            parameters.biases.em_upper_threshold = bias;
        } else if (key == "em_lower_threshold") {
            parameters.biases.em_lower_threshold = bias;
        } else if (key == "pullup_em_x_request") {
            parameters.biases.pullup_em_x_request = bias;
        } else if (key == "pullup_em_y_request") {
            parameters.biases.pullup_em_y_request = bias;
        } else if (key == "pullup_cd_x_request") {
            parameters.biases.pullup_cd_x_request = bias;
        } else if (key == "pullup_cd_y_request") {
            parameters.biases.pullup_cd_y_request = bias;
        } else if (key == "pullup_x_acknowledge") {
            parameters.biases.pullup_x_acknowledge = bias;
        } else if (key == "pullup_x_encoder") {
            parameters.biases.pullup_x_encoder = bias;
        } else if (key == "pullup_y_encoder") {
            parameters.biases.pullup_y_encoder = bias;
        } else if (key == "control_em_timeout") {
            parameters.biases.control_em_timeout = bias;
        } else if (key == "control_sequential_em_timeout") {
            parameters.biases.control_sequential_em_timeout = bias;
        } else if (key == "control_x_acknowledge_timeout") {
            parameters.biases.control_x_acknowledge_timeout = bias;
        } else if (key == "control_latch_cell_scan_pulldown") {
            parameters.biases.control_latch_cell_scan_pulldown = bias;
        } else if (key == "control_x_request_pulldown") {
            parameters.biases.control_x_request_pulldown = bias;
        } else {
            throw std::runtime_error(std::string("unknown parameters key '") + key + "'");
        }
    }
    return parameters;
}
