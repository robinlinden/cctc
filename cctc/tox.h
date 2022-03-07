#ifndef CCTC_TOX_H_
#define CCTC_TOX_H_

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <memory>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

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

class PublicKey {
public:
    constexpr static auto kByteSize = 32;

    constexpr explicit PublicKey(std::array<std::uint8_t, kByteSize> bytes) :
            bytes_{std::move(bytes)} {}

    explicit PublicKey(ToxID const &id) {
        auto pk_bytes = id.bytes().subspan(0, kByteSize);
        std::memcpy(bytes_.data(), pk_bytes.data(), kByteSize);
    }

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

    [[nodiscard]] bool operator==(PublicKey const &) const = default;

private:
    std::array<std::uint8_t, kByteSize> bytes_;
};

struct Savedata {
    enum class Type {
        None,
        ToxSave,
        SecretKey,
    };

    Type type{Type::None};
    std::vector<std::uint8_t> data{};
};

enum class ToxErrBootstrap {
    Ok,
    Null,
    BadHost,
    BadPort,
};

enum class Connection {
    None,
    Tcp,
    Udp,
};

struct SelfConnectionStatusEvent {
    Connection connection{Connection::None};
};

struct FriendConnectionStatusEvent {
    std::uint32_t friend_number{};
    Connection connection{Connection::None};
};

using ToxEvent = std::variant<
        SelfConnectionStatusEvent,
        FriendConnectionStatusEvent
    >;

class Tox {
public:
    Tox(Savedata const & = {});
    ~Tox();

    ToxErrBootstrap bootstrap(std::string const &host, std::uint16_t port, PublicKey const &);

    [[nodiscard]] std::chrono::milliseconds iteration_interval() const;
    [[nodiscard]] std::vector<ToxEvent> events_iterate();

    [[nodiscard]] ToxID self_get_address() const;
    [[nodiscard]] std::vector<std::uint8_t> get_savedata() const;

    [[nodiscard]] PublicKey self_get_dht_id() const;
    [[nodiscard]] std::optional<std::uint16_t> self_get_udp_port() const;

    // TODO(robinlinden): Error handling.
    std::optional<std::uint32_t> friend_add_norequest(PublicKey const &);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace cctc

#endif
