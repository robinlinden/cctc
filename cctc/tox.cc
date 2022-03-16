#include "cctc/tox.h"

#include <tox/tox.h>
#include <tox/tox_events.h>

#include <array>
#include <cstdint>
#include <exception>
#include <memory>
#include <sstream>

using CTox = Tox;

namespace cctc {
namespace {

static_assert(ToxID::kByteSize == TOX_ADDRESS_SIZE);

struct CToxDeleter {
    void operator()(CTox *tox) {
        tox_kill(tox);
    }
};

[[nodiscard]] constexpr Tox_Savedata_Type into(Savedata::Type type) {
    switch (type) {
        case Savedata::Type::None: return TOX_SAVEDATA_TYPE_NONE;
        case Savedata::Type::ToxSave: return TOX_SAVEDATA_TYPE_TOX_SAVE;
        case Savedata::Type::SecretKey: return TOX_SAVEDATA_TYPE_SECRET_KEY;
    }
    std::terminate();
}

[[nodiscard]] constexpr ToxErrBootstrap into(Tox_Err_Bootstrap err) {
    switch (err) {
        case TOX_ERR_BOOTSTRAP_OK: return ToxErrBootstrap::Ok;
        case TOX_ERR_BOOTSTRAP_NULL: return ToxErrBootstrap::Null;
        case TOX_ERR_BOOTSTRAP_BAD_HOST: return ToxErrBootstrap::BadHost;
        case TOX_ERR_BOOTSTRAP_BAD_PORT: return ToxErrBootstrap::BadPort;
    }
    std::terminate();
}

[[nodiscard]] constexpr Connection into(Tox_Connection connection) {
    switch (connection) {
        case TOX_CONNECTION_NONE: return Connection::None;
        case TOX_CONNECTION_TCP: return Connection::Tcp;
        case TOX_CONNECTION_UDP: return Connection::Udp;
    }
    std::terminate();
}

[[nodiscard]] constexpr MessageType into(Tox_Message_Type type) {
    switch (type) {
        case TOX_MESSAGE_TYPE_NORMAL: return MessageType::Normal;
        case TOX_MESSAGE_TYPE_ACTION: return MessageType::Action;
    }
    std::terminate();
}

[[nodiscard]] constexpr Tox_Message_Type into(MessageType type) {
    switch (type) {
        case MessageType::Normal: return TOX_MESSAGE_TYPE_NORMAL;
        case MessageType::Action: return TOX_MESSAGE_TYPE_ACTION;
    }
    std::terminate();
}

[[nodiscard]] std::vector<ToxEvent> into(Tox_Events const *events) {
    std::vector<ToxEvent> converted;

    for (std::uint32_t i = 0; i < tox_events_get_self_connection_status_size(events); ++i) {
        auto const *event = tox_events_get_self_connection_status(events, i);
        auto status = into(tox_event_self_connection_status_get_connection_status(event));
        converted.emplace_back(SelfConnectionStatusEvent{status});
    }

    for (std::uint32_t i = 0; i < tox_events_get_friend_connection_status_size(events); ++i) {
        auto const *event = tox_events_get_friend_connection_status(events, i);
        auto friend_no = tox_event_friend_connection_status_get_friend_number(event);
        auto status = into(tox_event_friend_connection_status_get_connection_status(event));
        converted.emplace_back(FriendConnectionStatusEvent{friend_no, status});
    }

    for (std::uint32_t i = 0; i < tox_events_get_friend_message_size(events); ++i) {
        auto const *event = tox_events_get_friend_message(events, i);
        auto friend_no = tox_event_friend_message_get_friend_number(event);
        auto type = into(tox_event_friend_message_get_type(event));
        auto message = std::string{
                reinterpret_cast<char const *>(tox_event_friend_message_get_message(event)),
                tox_event_friend_message_get_message_length(event)};
        converted.emplace_back(FriendMessageEvent{friend_no, type, std::move(message)});
    }

    return converted;
}

} // namespace

std::string toxcore_version() {
    std::stringstream ss;
    ss << tox_version_major() << "." << tox_version_minor() << "." << tox_version_patch();
    return ss.str();
}

class Tox::Impl {
public:
    Impl(Savedata const &savedata) : tox_{[&savedata] {
        auto *tox_options{tox_options_new(nullptr)};
        tox_options_set_savedata_type(tox_options, into(savedata.type));
        tox_options_set_savedata_data(tox_options, savedata.data.data(), savedata.data.size());
        auto *tox{tox_new(tox_options, nullptr)};
        tox_options_free(tox_options);
        tox_events_init(tox);
        return tox;
    }()} {}

    ToxErrBootstrap bootstrap(std::string const &host, std::uint16_t port, PublicKey const &pk) {
        Tox_Err_Bootstrap err;
        tox_bootstrap(tox_.get(), host.c_str(), port, pk.bytes().data(), &err);
        return into(err);
    }

    [[nodiscard]] std::chrono::milliseconds iteration_interval() const {
        return std::chrono::milliseconds{tox_iteration_interval(tox_.get())};
    }

    [[nodiscard]] std::vector<ToxEvent> events_iterate() {
        // TODO(robinlinden): Error handling.
        auto *events = tox_events_iterate(tox_.get(), false, nullptr);
        auto converted = into(events);
        tox_events_free(events);
        return converted;
    }

    [[nodiscard]] ToxID self_get_address() const {
        std::array<std::uint8_t, ToxID::kByteSize> bytes;
        tox_self_get_address(tox_.get(), bytes.data());
        return ToxID{std::move(bytes)};
    }

    [[nodiscard]] std::vector<std::uint8_t> get_savedata() const {
        std::vector<std::uint8_t> savedata;
        savedata.resize(tox_get_savedata_size(tox_.get()));
        tox_get_savedata(tox_.get(), savedata.data());
        return savedata;
    }

    [[nodiscard]] PublicKey self_get_dht_id() const {
        std::array<std::uint8_t, PublicKey::kByteSize> bytes;
        tox_self_get_dht_id(tox_.get(), bytes.data());
        return PublicKey{std::move(bytes)};
    }

    [[nodiscard]] std::optional<std::uint16_t> self_get_udp_port() const {
        std::uint16_t maybe_port = tox_self_get_udp_port(tox_.get(), nullptr);
        if (maybe_port == 0) {
            return std::nullopt;
        }

        return std::make_optional<std::uint16_t>(maybe_port);
    }

    bool self_set_name(std::string_view name) {
        return tox_self_set_name(
                tox_.get(),
                reinterpret_cast<std::uint8_t const *>(name.data()),
                name.size(),
                nullptr);
    }

    [[nodiscard]] std::string self_get_name() const {
        auto name = std::string(tox_self_get_name_size(tox_.get()), '\0');
        tox_self_get_name(tox_.get(), reinterpret_cast<std::uint8_t *>(name.data()));
        return name;
    }

    std::optional<std::uint32_t> friend_add_norequest(PublicKey const &pk) {
        Tox_Err_Friend_Add err{};
        std::uint32_t friend_no = tox_friend_add_norequest(tox_.get(), pk.bytes().data(), &err);
        if (err != TOX_ERR_FRIEND_ADD_OK) {
            return std::nullopt;
        }

        return friend_no;
    }

    std::uint32_t friend_send_message(std::uint32_t friend_number, MessageType type, std::string_view message) {
        return tox_friend_send_message(
                tox_.get(),
                friend_number,
                into(type),
                reinterpret_cast<std::uint8_t const *>(message.data()),
                message.size(),
                nullptr);
    }

private:
    std::unique_ptr<CTox, CToxDeleter> tox_{tox_new(nullptr, nullptr)};
};

Tox::Tox(Savedata const &savedata) : impl_{std::make_unique<Impl>(savedata)} {}
Tox::~Tox() = default;

ToxErrBootstrap Tox::bootstrap(std::string const &host, std::uint16_t port, PublicKey const &pk) {
    return impl_->bootstrap(host, port, pk);
}

std::chrono::milliseconds Tox::iteration_interval() const {
    return impl_->iteration_interval();
}

std::vector<ToxEvent> Tox::events_iterate() {
    return impl_->events_iterate();
}

ToxID Tox::self_get_address() const {
    return impl_->self_get_address();
}

std::vector<std::uint8_t> Tox::get_savedata() const {
    return impl_->get_savedata();
}

PublicKey Tox::self_get_dht_id() const {
    return impl_->self_get_dht_id();
}

std::optional<std::uint16_t> Tox::self_get_udp_port() const {
    return impl_->self_get_udp_port();
}

bool Tox::self_set_name(std::string_view name) {
    return impl_->self_set_name(name);
}

std::string Tox::self_get_name() const {
    return impl_->self_get_name();
}

std::optional<std::uint32_t> Tox::friend_add_norequest(PublicKey const &pk) {
    return impl_->friend_add_norequest(pk);
}

std::uint32_t Tox::friend_send_message(std::uint32_t friend_number, MessageType type, std::string_view message) {
    return impl_->friend_send_message(friend_number, type, message);
}

} // namespace cctc
