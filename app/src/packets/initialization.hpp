#ifndef ARCANE_APP_PACKETS_INITIALIZATION_HPP
#define ARCANE_APP_PACKETS_INITIALIZATION_HPP

#include <string>

#include <nlohmann/json.hpp>

namespace arcane::app::packets {
struct Initialization
{
    std::string username;
    std::uint32_t actualAppVersion;
    std::uint32_t daysRemaining;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Initialization, username, actualAppVersion, daysRemaining);
};
} // namespace arcane::app::packets

#endif // ARCANE_APP_PACKETS_INITIALIZATION_HPP
