#ifndef ARCANE_APP_PACKETS_UPDATE_HPP
#define ARCANE_APP_PACKETS_UPDATE_HPP

#include <string>

#include <nlohmann/json.hpp>

namespace arcane::app::packets {
struct Update
{
    std::string username;
    std::string binary;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Update, username, binary);
};
} // namespace arcane::app::packets

#endif // ARCANE_APP_PACKETS_UPDATE_HPP
