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

[[nodiscard]] std::vector<ToxEvent> into(Tox_Events const *events) {
    std::vector<ToxEvent> converted;

    for (std::uint32_t i = 0; i < tox_events_get_self_connection_status_size(events); ++i) {
        auto const *event = tox_events_get_self_connection_status(events, i);
        auto status = into(tox_event_self_connection_status_get_connection_status(event));
        converted.emplace_back(SelfConnectionStatusEvent{status});
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

    [[nodiscard]] std::uint32_t iteration_interval() const {
        return tox_iteration_interval(tox_.get());
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

private:
    std::unique_ptr<CTox, CToxDeleter> tox_{tox_new(nullptr, nullptr)};
};

Tox::Tox(Savedata const &savedata) : impl_{std::make_unique<Impl>(savedata)} {}
Tox::~Tox() = default;

ToxErrBootstrap Tox::bootstrap(std::string const &host, std::uint16_t port, PublicKey const &pk) {
    return impl_->bootstrap(host, port, pk);
}

std::uint32_t Tox::iteration_interval() const {
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

} // namespace cctc
