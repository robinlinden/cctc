#include "cctc/tox.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <utility>

int main() {
    std::cout << cctc::toxcore_version() << '\n';
    cctc::Tox tox;

    auto id = tox.self_get_address();
    std::cout << id.to_string();

    auto bytes = id.bytes();
    std::array<std::uint8_t, cctc::ToxID::kByteSize> new_tox_id_bytes;
    std::copy(bytes.begin(), bytes.end(), new_tox_id_bytes.begin());
    auto new_id = cctc::ToxID{std::move(new_tox_id_bytes)};

    if (new_id != id) {
        std::cout << "Copying ToxID using bytes() didn't work\n";
        return 1;
    }
}
