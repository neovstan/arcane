#ifndef ARCANE_LOADER_SRC_SOCKET_SOCKET_H
#define ARCANE_LOADER_SRC_SOCKET_SOCKET_H

#include <winsock2.h>

#include <exception>
#include <iostream>
#include <string>

// winsock2 should be included first
#include <windows.h>
#include <ws2tcpip.h>

#include <ktsignal/ktsignal.hpp>

namespace modification {
namespace loader {
class socket {
 public:
  class exception : public std::exception {
   public:
    enum class failed {
      wsa_startup,
      getaddrinfo,
      socket_creation,
      connection,
      sending,
      shutdowning,
    };

    exception(const failed reason, const int code = ::WSAGetLastError()) noexcept
        : reason_{reason}, code_{code} {
    }

    failed reason() const noexcept {
      return reason_;
    }
    int code() const noexcept {
      return code_;
    }

   private:
    failed reason_{};
    int code_{};
  };

  /**
   * @brief Constructor of the Socket class. Contains an invariant.
   */
  socket(const std::string& ip, const std::string& port, const std::string& encryption_key);

  ~socket();

  std::string send(const std::string& message);

 private:
  addrinfo* address_info_{};
  ::SOCKET connect_socket_{};
  std::string encryption_key_{};
};
}  // namespace loader
}  // namespace modification

#endif  // ARCANE_LOADER_SRC_SOCKET_SOCKET_H
