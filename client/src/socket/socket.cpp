#include "socket.h"

#include <VMProtectSDK.h>
#include <encryption/encryption.h>

modification::client::socket::socket(const std::string& ip, const std::string& port,
                                     const std::string& encryption_key)
    : encryption_key_{encryption_key} {
  VMProtectBeginUltra("modification::client::socket::socket()");

  WSADATA winsock_implementation_data{};

  if (const auto result = WSAStartup(MAKEWORD(2u, 2u), &winsock_implementation_data); result != 0) {
    throw exception{exception::failed::wsa_startup, result};
  }

  const addrinfo hints{0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP};

  if (const auto result = getaddrinfo(ip.c_str(), port.c_str(), &hints, &address_info_);
      result != 0) {
    throw exception{exception::failed::getaddrinfo, result};
  }

  VMProtectEnd();
}

modification::client::socket::~socket() {
  VMProtectBeginUltra("modification::client::socket::~socket()");

  if (address_info_) {
    freeaddrinfo(address_info_);
  }

  if (connect_socket_) {
    closesocket(connect_socket_);
  }

  WSACleanup();
  VMProtectEnd();
}

std::string modification::client::socket::send(const std::string& message) {
  VMProtectBeginUltra("modification::client::socket::send()");

  const std::string encrypted_message{
      encryption::cbc{encryption_key_, encryption_key_}.encrypt(message)};

  for (auto socket = address_info_; socket != nullptr; socket = socket->ai_next) {
    if (connect_socket_ = ::socket(socket->ai_family, socket->ai_socktype, socket->ai_protocol);
        INVALID_SOCKET == connect_socket_) {
      throw exception{exception::failed::socket_creation};
    }

    if (const auto result = connect(connect_socket_, socket->ai_addr, socket->ai_addrlen);
        result == SOCKET_ERROR) {
      closesocket(connect_socket_);
      connect_socket_ = INVALID_SOCKET;
      continue;
    }

    break;
  }

  if (INVALID_SOCKET == connect_socket_) {
    throw exception{exception::failed::connection};
  }

  if (const auto result =
          ::send(connect_socket_, encrypted_message.c_str(), encrypted_message.size() + 1u, 0);
      result == SOCKET_ERROR) {
    throw exception{exception::failed::sending};
  }

  if (const auto result = ::shutdown(connect_socket_, SD_SEND); result == SOCKET_ERROR) {
    throw exception{exception::failed::shutdowning};
  }

  static char buffer[16 * 1024 * 1024]{};
  recv(connect_socket_, buffer, sizeof(buffer), 0);
  closesocket(connect_socket_);

  const std::string answer{encryption::cbc{encryption_key_, encryption_key_}.decrypt(buffer)};

  VMProtectEnd();

  return answer;
}
