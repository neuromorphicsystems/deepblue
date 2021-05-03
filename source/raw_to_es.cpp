#include "../third_party/roo/sepia.hpp"
#include <iostream>

constexpr uint16_t width = 304;
constexpr uint16_t height = 240;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "syntax: raw_to_es /path/to/input.raw /path/to/output.es" << std::endl;
        return 1;
    }
    auto input = sepia::filename_to_ifstream(argv[1]);
    for (;;) {
        auto next = input->peek();
        if (input->eof()) {
            std::cerr << "the RAW file does not contain events" << std::endl;
            return 1;
        }
        if (next != '%') {
            break;
        }
        std::vector<uint8_t> buffer(1 << 16);
        input->getline(reinterpret_cast<char*>(buffer.data()), buffer.size(), '\n');
    }
    std::vector<uint8_t> buffer(1 << 16);
    uint64_t t_offset = 0;
    uint64_t warps = 0;
    uint64_t previous_t = 0;
    sepia::write<sepia::type::atis> write(sepia::filename_to_ofstream(argv[2]), width, height);
    while (!input->eof()) {
        input->read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        if (input->eof()) {
            buffer.resize(input->gcount());
        }
        for (std::size_t index = 0; index < (buffer.size() / 4) * 4; index += 4) {
            if (buffer[index + 3] == 0x80) {
                const auto new_t_offset =
                    (static_cast<uint32_t>(buffer[index]) | (static_cast<uint32_t>(buffer[index + 1]) << 8)
                     | (static_cast<uint32_t>(buffer[index + 2]) << 16))
                    * 0x800;
                if (new_t_offset < t_offset) {
                    ++warps;
                }
                t_offset = new_t_offset;
            } else {
                sepia::atis_event event{
                    static_cast<uint64_t>(warps) * (1ull << 32) + t_offset
                        + ((static_cast<uint16_t>(buffer[index + 3] & 0b1111) << 7) | (buffer[index + 2] >> 1)),
                    static_cast<uint16_t>((static_cast<uint16_t>(buffer[index + 2] & 1) << 8) | buffer[index + 1]),
                    static_cast<uint16_t>(height - 1 - buffer[index]),
                    ((buffer[index + 3] >> 5) & 0b1) == 1,
                    ((buffer[index + 3] >> 4) & 0b1) == 1};
                if (event.x < width && event.y < height && event.t >= previous_t) {
                    write(event);
                    previous_t = event.t;
                }
            }
        }
    }
    return 0;
}
