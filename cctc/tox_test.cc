#include "cctc/tox.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <utility>

namespace {

bool test_tox_id() {
    cctc::Tox tox;

    auto id = tox.self_get_address();

    auto bytes = id.bytes();
    std::array<std::uint8_t, cctc::ToxID::kByteSize> new_tox_id_bytes;
    std::copy(bytes.begin(), bytes.end(), new_tox_id_bytes.begin());
    auto new_id = cctc::ToxID{std::move(new_tox_id_bytes)};

    return new_id == id;
}

} // namespace

int main() {
    std::cout << cctc::toxcore_version() << '\n';
    if (!test_tox_id()) {
        std::cout << "ToxID is broken.\n";
        return 1;
    }
}
