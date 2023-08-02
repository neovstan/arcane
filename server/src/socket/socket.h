#ifndef ARCANE_SERVER_SRC_SOCKET_SOCKET_H
#define ARCANE_SERVER_SRC_SOCKET_SOCKET_H

#include <winsock2.h>

// winsock2.h should be included first
#include <windows.h>
#include <ws2tcpip.h>

#include <exception>
#include <iostream>
#include <ktsignal/ktsignal.hpp>
#include <string>

namespace modification {
namespace server {
class socket {
 public:
  class exception : public std::exception {
   public:
    enum class failed {
      wsa_startup,
      getaddrinfo,
      socket_creation,
      binding,
      listening,
      accepting,
      sending,
      shutdowning,
    };

    exception(const failed reason, const std::string& text = "",
              const int code = ::WSAGetLastError()) noexcept
        : reason_{reason}, text_{text}, code_{code} {}

    failed reason() const noexcept { return reason_; }
    int code() const noexcept { return code_; }
    const char* what() const noexcept override { return text_.c_str(); }

   private:
    failed reason_{};
    int code_{};
    std::string text_{};
  };

  /**
   * @brief Constructor of the Socket class. Contains an invariant.
   */
  socket(const std::string& port, const std::string& encryption_key);
  ~socket();

  void process();

  ktsignal::ktsignal<void(const std::string&, std::string&)>& on_receive() {
    return on_receive_;
  }

 private:
  addrinfo* address_info_{};
  ::SOCKET listen_socket_{};
  ktsignal::ktsignal<void(const std::string&, std::string&)> on_receive_{};
  std::string encryption_key_{};
};
}  // namespace server
}  // namespace modification

#endif  // ARCANE_SERVER_SRC_SOCKET_SOCKET_H
