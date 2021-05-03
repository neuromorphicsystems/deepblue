#include "../third_party/roo/ccamatis.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            throw std::runtime_error("syntax: distribution /path/to/output.es");
        }
        auto capture_exception = sepia::make_unique<sepia::capture_exception>();
        sepia::write<sepia::type::atis> write(
            sepia::filename_to_ofstream(argv[1]), sepia::ccamatis::width, sepia::ccamatis::height);
        auto camera = sepia::ccamatis::make_buffered_camera(
            sepia::ccamatis::decode(std::ref(write), [&]() { write.flush(); }), std::ref(*capture_exception));
        capture_exception->wait();
        capture_exception->rethrow_unless<>();
    } catch (const std::runtime_error& exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }
    return 0;
}
