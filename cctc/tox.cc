#include "cctc/tox.h"

#include <tox/tox.h>

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
        return tox;
    }()} {}

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

private:
    std::unique_ptr<CTox, CToxDeleter> tox_{tox_new(nullptr, nullptr)};
};

Tox::Tox(Savedata const &savedata) : impl_{std::make_unique<Impl>(savedata)} {}
Tox::~Tox() = default;

ToxID Tox::self_get_address() const {
    return impl_->self_get_address();
}

std::vector<std::uint8_t> Tox::get_savedata() const {
    return impl_->get_savedata();
}

} // namespace cctc
