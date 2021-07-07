#pragma once

#include "../third_party/json.hpp"
#include <atomic>
#include <string>

const std::string sepia_sensors_json_as_string{
#include "sepia_sensors.json.hpp"
};

namespace sepia_sensors {
    template <typename Handler>
    class parameters {
        public:
        parameters(Handler& handler) :
            _handler(handler), _json(nlohmann::json::parse(sepia_sensors_json_as_string)[handler.sensor()]) {
            _accessing_data.clear(std::memory_order_release);
        }
        parameters(const parameters&) = delete;
        parameters(parameters&& other) = delete;
        parameters& operator=(const parameters&) = delete;
        parameters& operator=(parameters&& other) = delete;
        ~parameters() {}

        template <typename HandleSubchannelAndMessage>
        void for_each(HandleSubchannelAndMessage&& handle_subchannel_and_message) const {
            for_each_child(
                "", _json["parameters"], std::forward<HandleSubchannelAndMessage>(handle_subchannel_and_message));
        }

        void handle(const std::string& subchannel, const nlohmann::json& data) {
            std::size_t key_begin = 0;
            nlohmann::json* json = &(_json["parameters"]);
            for (std::size_t index = 0; index < subchannel.size() + 1; ++index) {
                if (index == subchannel.size() || subchannel[index] == ':') {
                    const auto key = subchannel.substr(key_begin, index);
                    auto found = false;
                    for (auto& child : *json) {
                        if (child["key"] == key) {
                            json = &child;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        throw std::runtime_error(std::string("invalid keys in subchannel '") + subchannel + "'");
                    }
                    if (index == subchannel.size()) {
                        const std::string type = (*json)["type"];
                        if (type == "collection") {
                            throw std::runtime_error(std::string("not enough keys in subchannel '") + subchannel + "'");
                        }
                        if (type == "int") {
                            const auto value = data.get<int64_t>();
                            if (value < (*json)["minimum"].get<int64_t>()
                                || value > (*json)["maximum"].get<int64_t>()) {
                                throw std::runtime_error(
                                    std::string("out of range value for subchannel '") + subchannel + "'");
                            }
                            _handler.handle_int(subchannel, value);
                            return;
                        }
                        if (type == "bool") {
                            _handler.handle_bool(subchannel, data.get<bool>());
                            return;
                        }
                        if (type == "enum") {
                            const auto value = data.get<std::string>();
                            auto found = false;
                            for (const auto& valid_value : (*json)["values"]) {
                                if (value == valid_value) {
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                throw std::runtime_error(
                                    std::string("invalid value for subchannel '") + subchannel + "'");
                            }
                            _handler.handle_enum(subchannel, value);
                            return;
                        }
                        if (type == "pixel") {
                            if (!data.is_array() || data.size() != 2) {
                                throw std::runtime_error(std::string("bad format for subchannel '") + subchannel + "'");
                            }
                            std::array<int64_t, 2> values{data[0].get<int64_t>(), data[1].get<int64_t>()};
                            for (uint8_t index = 0; index < 2; ++index) {
                                if (values[index] < (*json)["minimum"][index].get<int64_t>()
                                    || values[index] > (*json)["maximum"][index].get<int64_t>()) {
                                    throw std::runtime_error(
                                        std::string("out of range value for subchannel '") + subchannel + "'");
                                }
                            }
                            _handler.handle_pixel(subchannel, values[0], values[1]);
                            return;
                        }
                        if (type == "rectangle") {
                            if (!data.is_array() || data.size() != 2) {
                                throw std::runtime_error(std::string("bad format for subchannel '") + subchannel + "'");
                            }
                            std::array<int64_t, 4> values;
                            for (uint8_t index = 0; index < 2; ++index) {
                                if (!data[index].is_array() || data[index].size() != 2) {
                                    throw std::runtime_error(
                                        std::string("bad format for subchannel '") + subchannel + "'");
                                }
                                values[2 * index] = data[index][0].get<int64_t>();
                                values[2 * index + 1] = data[index][1].get<int64_t>();
                                for (uint8_t other_index = 0; other_index < 2; ++other_index) {
                                    if (values[2 * index + other_index] < (*json)["minimum"][other_index].get<int64_t>()
                                        || values[2 * index + other_index]
                                               > (*json)["maximum"][other_index].get<int64_t>()) {
                                        throw std::runtime_error(
                                            std::string("out of range value for subchannel '") + subchannel + "'");
                                    }
                                }
                            }
                            _handler.handle_rectangle(subchannel, values[0], values[1], values[2], values[3]);
                            return;
                        }
                        throw std::logic_error(std::string("unknow type '") + type + "'");
                    } else {
                        if ((*json)["type"] != "collection") {
                            throw std::runtime_error(std::string("too many keys in subchannel '") + subchannel + "'");
                        }
                        json = &((*json)["children"]);
                        key_begin = index + 1;
                    }
                }
            }
        }

        protected:
        template <typename HandleSubchannelAndMessage>
        static void for_each_child(
            const std::string& prefix,
            const nlohmann::json& json,
            HandleSubchannelAndMessage&& handle_subchannel_and_message) {
            for (const auto& child : json) {
                if (child["type"] == "collection") {
                    for_each_child(
                        prefix + child["key"].get<std::string>() + ":",
                        child["children"],
                        std::forward<HandleSubchannelAndMessage>(handle_subchannel_and_message));
                } else {
                    handle_subchannel_and_message(prefix + child["key"].get<std::string>(), child["default"].dump());
                }
            }
        }

        Handler& _handler;
        nlohmann::json _json;
        std::atomic_flag _accessing_data;
    };
}
