#ifndef ARCANE_CLIENT_SRC_SOCKET_H
#define ARCANE_CLIENT_SRC_SOCKET_H

#include <winsock2.h>

#include <exception>
#include <iostream>
#include <string>

// winsock2 should be included first
#include <windows.h>
#include <ws2tcpip.h>

namespace modification {
namespace client {
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

    explicit exception(const failed reason, const int code = ::WSAGetLastError()) noexcept
        : reason_{reason}, code_{code} {
    }

    failed reason() const noexcept {
      return reason_;
    }
    int code() const noexcept {
      return code_;
    }

   private:
    failed reason_;
    int code_;
  };

  socket(std::string_view ip, std::string_view port, std::string_view encryption_key);
  ~socket();

  std::string send(std::string_view message);

 private:
  addrinfo* address_info_;
  SOCKET connect_socket_;
  std::string encryption_key_;
};
}  // namespace client
}  // namespace modification

#endif  // ARCANE_CLIENT_SRC_SOCKET_H
