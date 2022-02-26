#include "cctc/tox.h"

#include "etest/etest.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

using namespace std::literals;

namespace {

void *make_test(std::string_view name, std::function<void()> body) {
    etest::test(name, std::move(body));
    return nullptr;
}

auto tox_id = make_test("tox id"sv, [] {
    cctc::Tox tox;

    auto id = tox.self_get_address();

    auto bytes = id.bytes();
    std::array<std::uint8_t, cctc::ToxID::kByteSize> new_tox_id_bytes;
    std::copy(bytes.begin(), bytes.end(), new_tox_id_bytes.begin());
    auto new_id = cctc::ToxID{std::move(new_tox_id_bytes)};

    return new_id == id;
});

auto public_key = make_test("public key", [] {
    cctc::Tox tox;

    auto id = tox.self_get_address();
    auto pk = cctc::PublicKey{id};

    auto bytes = pk.bytes();
    std::array<std::uint8_t, cctc::PublicKey::kByteSize> new_bytes;
    std::copy(bytes.begin(), bytes.end(), new_bytes.begin());
    auto new_pk = cctc::PublicKey{std::move(new_bytes)};

    return pk == new_pk;
});

auto save_and_load = make_test("saving/loading"sv, [] {
    cctc::Tox tox;
    auto id = tox.self_get_address();
    auto savedata = tox.get_savedata();

    cctc::Tox new_tox{cctc::Savedata{cctc::Savedata::Type::ToxSave, std::move(savedata)}};
    auto new_id = new_tox.self_get_address();
    return new_id == id;
});

auto connect = make_test("bootstrapping/connecting"sv, [] {
    std::array<cctc::Tox, 3> toxes{};
    std::array<cctc::Connection, toxes.size()> connection_statuses{};

    auto dht_key = toxes.front().self_get_dht_id();
    auto dht_port = toxes.front().self_get_udp_port().value();

    for (int i = 1; i < toxes.size(); ++i) {
        toxes[i].bootstrap("localhost"s, dht_port, dht_key);
    }

    while (std::any_of(
            cbegin(connection_statuses),
            cend(connection_statuses),
            [](auto status) { return status == cctc::Connection::None; })) {
        for (int i = 0; i < toxes.size(); ++i) {
            auto events = toxes[i].events_iterate();
            for (auto const &event : events) {
                if (auto const *connection_event = std::get_if<cctc::SelfConnectionStatusEvent>(&event)) {
                    connection_statuses[i] = connection_event->connection;
                    break;
                }
            }
        }

        std::this_thread::sleep_for(toxes.front().iteration_interval());
    }

    return true;
});

} // namespace

int main() {
    std::cout << cctc::toxcore_version() << '\n';
    return etest::run_all_tests();
}
