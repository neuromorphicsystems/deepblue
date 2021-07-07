#include "../third_party/roo/ccamatis.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            throw std::runtime_error("syntax: distribution threshold");
        }
        const auto threshold = std::stoull(argv[1]);
        uint64_t previous_t = 0;
        uint64_t total = 0;
        uint64_t below_threshold = 0;
        uint8_t count = 0;
        auto capture_exception = sepia::make_unique<sepia::capture_exception>();
        auto camera = sepia::ccamatis::make_buffered_camera(
            sepia::ccamatis::decode(
                [&](sepia::atis_event event) {
                    ++total;
                    if (event.t - previous_t < threshold) {
                        ++below_threshold;
                    }
                    previous_t = event.t;
                },
                [&]() {
                    if (count == 0) {
                        std::cout << below_threshold << " / " << total << " ("
                                  << (static_cast<double>(below_threshold) / total * 100) << " %)" << std::endl;
                    }
                    ++count;
                }),
            std::ref(*capture_exception));
        capture_exception->wait();
        capture_exception->rethrow_unless<>();
    } catch (const std::runtime_error& exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }
    return 0;
}
