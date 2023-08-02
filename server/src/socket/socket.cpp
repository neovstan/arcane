#include "socket.h"

#include <encryption/encryption.h>

modification::server::socket::socket(const std::string& port, const std::string& encryption_key)
    : encryption_key_{encryption_key} {
  ::WSADATA winsock_implementation_data{};

  if (const auto result = ::WSAStartup(MAKEWORD(2u, 2u), &winsock_implementation_data);
      result != 0) {
    throw exception{exception::failed::wsa_startup, "WSAStartup failed", result};
  }

  const addrinfo hints{AI_PASSIVE, AF_INET, SOCK_STREAM, IPPROTO_TCP};

  if (const auto result = ::getaddrinfo(nullptr, port.c_str(), &hints, &address_info_);
      result != 0) {
    throw exception{exception::failed::getaddrinfo, "getaddrinfo failed", result};
  }

  listen_socket_ =
      ::socket(address_info_->ai_family, address_info_->ai_socktype, address_info_->ai_protocol);

  if (INVALID_SOCKET == listen_socket_) {
    throw exception{exception::failed::socket_creation, "socket creation failed"};
  }

  if (const auto result = ::bind(listen_socket_, address_info_->ai_addr, address_info_->ai_addrlen);
      result == SOCKET_ERROR) {
    throw exception{exception::failed::binding, "binding failed"};
  }

  if (address_info_) {
    ::freeaddrinfo(address_info_);
  }

  if (const auto result = ::listen(listen_socket_, SOMAXCONN); result == SOCKET_ERROR) {
    throw exception{exception::failed::listening, "listen failed"};
  }
}

modification::server::socket::~socket() {
  if (listen_socket_) {
    ::shutdown(listen_socket_, SD_BOTH);
    ::closesocket(listen_socket_);
  }

  ::WSACleanup();
}

void modification::server::socket::process() {
  ::SOCKET client_socket{};
  if (client_socket = ::accept(listen_socket_, nullptr, nullptr); INVALID_SOCKET == client_socket) {
    throw exception{exception::failed::accepting, "accept failed"};
  }

  char buffer[2048]{};
  if (::recv(client_socket, buffer, sizeof(buffer), 0) > 0) {
    std::string message{};
    try {
      message = encryption::cbc{encryption_key_, encryption_key_}.decrypt(buffer);
    } catch (...) {
    }

    if (!message.empty()) {
      std::string answer{};
      on_receive_.emit<const std::string&, std::string&>(message, answer);

      if (!answer.empty()) {
        const std::string encrypted_answer{
            encryption::cbc{encryption_key_, encryption_key_}.encrypt(answer)};

        if (const auto result =
                ::send(client_socket, encrypted_answer.c_str(), encrypted_answer.size() + 1u, 0);
            result == SOCKET_ERROR) {
          throw exception{exception::failed::sending, "send failed"};
        }
      }
    }
  }

  if (const auto result = ::shutdown(client_socket, SD_SEND); result == SOCKET_ERROR) {
    ::closesocket(client_socket);
    throw exception{exception::failed::shutdowning, "shutdown failed"};
  }

  ::closesocket(client_socket);
}
