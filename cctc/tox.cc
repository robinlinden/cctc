#include "cctc/tox.h"

#include <tox/tox.h>

#include <memory>
#include <sstream>

using CTox = Tox;

namespace cctc {
namespace {

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
private:
    std::unique_ptr<CTox, CToxDeleter> tox_{tox_new(nullptr, nullptr)};
};

Tox::Tox() : impl_{std::make_unique<Impl>()} {}
Tox::~Tox() = default;

} // namespace cctc
