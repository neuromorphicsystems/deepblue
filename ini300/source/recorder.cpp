#include "../../common/interface.hpp"
#include "../third_party/puffin/source/puffin.hpp"
#include "ini300_parameters_handler.hpp"
#include "parameters.hpp"
#include <filesystem>
#include <iostream>
#include <sstream>

std::string next_filename(const std::filesystem::path& directory) {
    uint64_t index = 0;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        const auto name_with_extension = entry.path().filename().string();
        if (name_with_extension.size() > 3
            && (name_with_extension.substr(name_with_extension.size() - 3) == ".es"
                || name_with_extension.substr(name_with_extension.size() - 3) == ".tf")) {
            const auto name = name_with_extension.substr(0, name_with_extension.size() - 3);
            if (std::all_of(name.begin(), name.end(), [](char character) { return std::isdigit(character); })) {
                const auto index_candidate = std::stoull(name) + 1;
                if (index_candidate > index) {
                    index = index_candidate;
                }
            }
        }
    }
    std::stringstream stream;
    stream << std::setw(4) << std::setfill('0') << index;
    return (directory / stream.str()).string();
}

int main(int argc, char* argv[]) {
    interface external_interface;
    try {
        if (argc != 2 && argc != 3) {
            throw std::runtime_error("syntax: recorder /path/to/directory [/path/to/parameters.json]");
        }
        std::filesystem::path directory(argv[1]);
        if (!std::filesystem::is_directory(directory)) {
            throw std::runtime_error(std::string("'") + directory.string() + "' does not exist or is not a directory");
        }
        sepia_sensors::ini300_parameters_handler parameters_handler;
        sepia_sensors::parameters parameters(parameters_handler);
        
        if (argc == 3) {
            sepia::filename_to_ifstream(argv[2])
        
        
        }
        
        std::unique_ptr<puffin::server> server;
        server = puffin::make_server(
            8080, // listening port
            [](std::size_t, const std::string&) {
                return puffin::message{};
            },
            [&](std::size_t id, const puffin::message& message) {
                try {
                    const auto data = nlohmann::json(std::string(message.bytes.begin(), message.bytes.end()));
                    if (data.find("subchannel") == data.end()) {
                        throw std::runtime_error("missing key 'subchannel'");
                    }
                    if (data.find("value") == data.end()) {
                        throw std::runtime_error("missing key 'value'");
                    }
                    parameters.handle(data["subchannel"], data["value"]);
                } catch (const std::runtime_error& exception) {
                    server->send(id, puffin::string_to_message(exception.what()));
                }
            },
            [](std::size_t) {});
        

        const auto parameters =
            argc == 3 ? parse_parameters(sepia::filename_to_ifstream(argv[2])) : sepia::ccamatis::default_parameters;
        external_interface.on();
        std::unique_ptr<sepia::write<sepia::type::atis>> write;
        std::unique_ptr<sepia::capture_exception> capture_exception;
        std::unique_ptr<sepia::ccamatis::base_camera> camera;
        for (;;) {
            auto wrote = false;
            std::string filename;
            uint8_t count = 0;
            uint8_t blink_count = 0;
            try {
                while (!external_interface.enabled()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                filename = next_filename(directory);
                std::cout << "new recording: " << filename << std::endl;
                write.reset(new sepia::write<sepia::type::atis>(
                    sepia::filename_to_ofstream(filename), sepia::ccamatis::width, sepia::ccamatis::height));
                capture_exception.reset(new sepia::capture_exception());
                camera = sepia::ccamatis::make_buffered_camera(
                    sepia::ccamatis::decode(
                        [&](sepia::atis_event event) {
                            wrote = true;
                            write->operator()(event);
                        },
                        [&]() {
                            if (count == 0) {
                                write->flush();
                                if (blink_count == 4) {
                                    blink_count = 0;
                                    external_interface.blink();
                                } else {
                                    ++blink_count;
                                }
                                if (!external_interface.enabled()) {
                                    throw std::runtime_error("switch off");
                                }
                            }
                            ++count;
                        }),
                    std::ref(*capture_exception),
                    parameters);
                capture_exception->wait();
                capture_exception->rethrow_unless<>();
            } catch (const std::runtime_error& exception) {
                std::cout << "recording stopped: " << exception.what() << std::endl;
            }
            camera.reset();
            capture_exception.reset();
            write.reset();
            if (wrote) {
                external_interface.on();
            } else {
                std::error_code code;
                std::filesystem::remove(filename, code);
            }
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    } catch (const std::runtime_error& exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }
    return 0;
}
