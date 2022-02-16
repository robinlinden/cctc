#include "cctc/tox.h"

#include <tox/tox.h>

#include <array>
#include <cstdint>
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

} // namespace

std::string toxcore_version() {
    std::stringstream ss;
    ss << tox_version_major() << "." << tox_version_minor() << "." << tox_version_patch();
    return ss.str();
}

class Tox::Impl {
public:
    [[nodiscard]] ToxID self_get_address() const {
        std::array<std::uint8_t, ToxID::kByteSize> bytes;
        tox_self_get_address(tox_.get(), bytes.data());
        return ToxID{std::move(bytes)};
    }

private:
    std::unique_ptr<CTox, CToxDeleter> tox_{tox_new(nullptr, nullptr)};
};

Tox::Tox() : impl_{std::make_unique<Impl>()} {}
Tox::~Tox() = default;

ToxID Tox::self_get_address() const {
    return impl_->self_get_address();
}

} // namespace cctc
