#include "cctc/tox.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <utility>

using namespace std::literals;

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

bool test_public_key() {
    cctc::Tox tox;

    auto id = tox.self_get_address();
    auto pk = cctc::PublicKey{id};

    auto bytes = pk.bytes();
    std::array<std::uint8_t, cctc::PublicKey::kByteSize> new_bytes;
    std::copy(bytes.begin(), bytes.end(), new_bytes.begin());
    auto new_pk = cctc::PublicKey{std::move(new_bytes)};

    return pk == new_pk;
}

bool test_saving_and_loading() {
    cctc::Tox tox;
    auto id = tox.self_get_address();
    auto savedata = tox.get_savedata();

    cctc::Tox new_tox{cctc::Savedata{cctc::Savedata::Type::ToxSave, std::move(savedata)}};
    auto new_id = new_tox.self_get_address();
    return new_id == id;
}

bool connecting_to_the_network() {
    cctc::Tox tox;
    tox.bootstrap("tox.abilinski.com"s, 33445, cctc::PublicKey(std::to_array<std::uint8_t>({
        0x10, 0xC0, 0x0E, 0xB2, 0x50, 0xC3, 0x23, 0x3E,
        0x34, 0x3E, 0x2A, 0xEB, 0xA0, 0x71, 0x15, 0xA5,
        0xC2, 0x89, 0x20, 0xE9, 0xC8, 0xD2, 0x94, 0x92,
        0xF6, 0xD0, 0x0B, 0x29, 0x04, 0x9E, 0xDC, 0x7E,
    })));
    tox.bootstrap("tox.initramfs.io"s, 33445, cctc::PublicKey(std::to_array<std::uint8_t>({
        0x02, 0x80, 0x7C, 0xF4, 0xF8, 0xBB, 0x8F, 0xB3,
        0x90, 0xCC, 0x37, 0x94, 0xBD, 0xF1, 0xE8, 0x44,
        0x9E, 0x9A, 0x83, 0x92, 0xC5, 0xD3, 0xF2, 0x20,
        0x00, 0x19, 0xDA, 0x9F, 0x1E, 0x81, 0x2E, 0x46,
    })));
    tox.bootstrap("tox.plastiras.org"s, 33445, cctc::PublicKey(std::to_array<std::uint8_t>({
        0x8E, 0x8B, 0x63, 0x29, 0x9B, 0x3D, 0x52, 0x0F,
        0xB3, 0x77, 0xFE, 0x51, 0x00, 0xE6, 0x5E, 0x33,
        0x22, 0xF7, 0xAE, 0x5B, 0x20, 0xA0, 0xAC, 0xED,
        0x29, 0x81, 0x76, 0x9F, 0xC5, 0xB4, 0x37, 0x25,
    })));

    bool connected = false;
    while (!connected) {
        auto events = tox.events_iterate();
        for (auto const &event : events) {
            if (auto const *connection_event = std::get_if<cctc::SelfConnectionStatusEvent>(&event)) {
                connected = connection_event->connection != cctc::Connection::None;
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(tox.iteration_interval()));
    }

    return true;
}

} // namespace

int main() {
    std::cout << cctc::toxcore_version() << '\n';
    if (!test_tox_id()) {
        std::cout << "ToxID is broken.\n";
        return 1;
    }

    if (!test_public_key()) {
        std::cout << "PublicKey is broken.\n";
        return 1;
    }

    if (!test_saving_and_loading()) {
        std::cout << "Saving and loading is broken.\n";
        return 1;
    }

    if (!connecting_to_the_network()) {
        std::cout << "Connecting to the network is broken.\n";
        return 1;
    }
}
