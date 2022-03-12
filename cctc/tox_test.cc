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

namespace cctc {
namespace {

void *make_test(std::string_view name, std::function<void()> body) {
    etest::test(name, std::move(body));
    return nullptr;
}

void bootstrap(auto &toxes) {
    std::vector<Connection> connection_statuses{toxes.size(), Connection::None};

    auto dht_key = toxes.front().self_get_dht_id();
    auto dht_port = toxes.front().self_get_udp_port().value();

    for (int i = 1; i < toxes.size(); ++i) {
        toxes[i].bootstrap("localhost"s, dht_port, dht_key);
    }

    while (std::any_of(
            cbegin(connection_statuses),
            cend(connection_statuses),
            [](auto status) { return status == Connection::None; })) {
        for (int i = 0; i < toxes.size(); ++i) {
            auto events = toxes[i].events_iterate();
            for (auto const &event : events) {
                if (auto const *connection_event = std::get_if<SelfConnectionStatusEvent>(&event)) {
                    connection_statuses[i] = connection_event->connection;
                    break;
                }
            }
        }

        std::this_thread::sleep_for(toxes.front().iteration_interval());
    }
}

auto tox_id = make_test("tox id"sv, [] {
    Tox tox;

    auto id = tox.self_get_address();

    auto bytes = id.bytes();
    std::array<std::uint8_t, ToxID::kByteSize> new_tox_id_bytes;
    std::copy(bytes.begin(), bytes.end(), new_tox_id_bytes.begin());
    auto new_id = ToxID{std::move(new_tox_id_bytes)};

    etest::expect_eq(new_id, id);
});

auto public_key = make_test("public key", [] {
    Tox tox;

    auto id = tox.self_get_address();
    auto pk = PublicKey{id};

    auto bytes = pk.bytes();
    std::array<std::uint8_t, PublicKey::kByteSize> new_bytes;
    std::copy(bytes.begin(), bytes.end(), new_bytes.begin());
    auto new_pk = PublicKey{std::move(new_bytes)};

    etest::expect_eq(pk, new_pk);
});

auto save_and_load = make_test("saving/loading"sv, [] {
    Tox tox;
    auto id = tox.self_get_address();
    auto savedata = tox.get_savedata();

    Tox new_tox{Savedata{Savedata::Type::ToxSave, std::move(savedata)}};
    auto new_id = new_tox.self_get_address();
    etest::expect_eq(new_id, id);
});

auto friend_add_norequest = make_test("friend_send_message"sv, [] {
    std::array<Tox, 2> toxes;
    bootstrap(toxes);

    std::array<Connection, 2> connection_statuses{};
    std::array<std::uint32_t, 2> friend_numbers;
    connection_statuses[0] = Connection::None;
    friend_numbers[0] = toxes[0].friend_add_norequest(PublicKey{toxes[1].self_get_address()}).value();
    connection_statuses[1] = Connection::None;
    friend_numbers[1] = toxes[1].friend_add_norequest(PublicKey{toxes[0].self_get_address()}).value();

    while (std::any_of(
            cbegin(connection_statuses),
            cend(connection_statuses),
            [](auto status) { return status == Connection::None; })) {
        for (int i = 0; i < toxes.size(); ++i) {
            auto events = toxes[i].events_iterate();
            for (auto const &event : events) {
                if (auto const *connection_event = std::get_if<FriendConnectionStatusEvent>(&event)) {
                    etest::require(i < friend_numbers.size());
                    etest::expect_eq(connection_event->friend_number, friend_numbers[i]);
                    etest::expect_eq(connection_event->connection, Connection::Udp);
                    connection_statuses[i] = connection_event->connection;
                    break;
                }
            }
        }

        std::this_thread::sleep_for(toxes.front().iteration_interval());
    }

    toxes[0].friend_send_message(friend_numbers[0], MessageType::Normal, "hello?"sv);

    bool received_message{false};
    while (!received_message) {
        for (int i = 0; i < toxes.size(); ++i) {
            auto events = toxes[i].events_iterate();
            for (auto const &event : events) {
                if (auto const *msg_event = std::get_if<FriendMessageEvent>(&event)) {
                    etest::require(i < friend_numbers.size());
                    etest::expect_eq(msg_event->friend_number, friend_numbers[i]);
                    etest::expect_eq(msg_event->type, MessageType::Normal);
                    etest::expect_eq(msg_event->message, "hello?"sv);
                    received_message = true;
                    break;
                }
            }
        }

        std::this_thread::sleep_for(toxes.front().iteration_interval());
    }
});

} // namespace
} // namespace cctc

int main() {
    std::cout << cctc::toxcore_version() << '\n';
    return etest::run_all_tests();
}
