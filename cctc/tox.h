#ifndef CCTC_TOX_H_
#define CCTC_TOX_H_

#include <algorithm>
#include <array>
#include <cstdint>
#include <iomanip>
#include <memory>
#include <span>
#include <sstream>
#include <string>
#include <utility>

namespace cctc {

std::string toxcore_version();

class ToxID {
public:
    constexpr static auto kByteSize = 38;

    constexpr explicit ToxID(std::array<std::uint8_t, kByteSize> bytes) :
            bytes_{std::move(bytes)} {}

    [[nodiscard]] constexpr std::span<std::uint8_t const, kByteSize> bytes() const {
        return {bytes_};
    }

    [[nodiscard]] std::string to_string() const {
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0');
        for (int byte : bytes_) {
            ss << std::setw(2) << byte;
        }
        return ss.str();
    }

    [[nodiscard]] bool operator==(ToxID const &) const = default;

private:
    std::array<std::uint8_t, kByteSize> bytes_;
};

class Tox {
public:
    Tox();
    ~Tox();

    [[nodiscard]] ToxID self_get_address() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace cctc

#endif
