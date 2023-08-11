#ifndef ARCANE_APP_PACKETS_LOAD_HPP
#define ARCANE_APP_PACKETS_LOAD_HPP

#include <string>

#include <nlohmann/json.hpp>

namespace arcane::app::packets {
struct Load
{
    std::string username;
    std::string dll;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Load, username, dll);
};
} // namespace arcane::app::packets

#endif // ARCANE_APP_PACKETS_LOAD_HPP
