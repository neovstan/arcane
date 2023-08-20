#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <cryptopp/base64.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rijndael.h>

namespace encryption {
class cbc {
 public:
  cbc(std::string_view key, std::string_view iv)
      : key_{reinterpret_cast<const CryptoPP::byte*>(key.data()), key.size()},
        iv_{reinterpret_cast<const CryptoPP::byte*>(iv.data()), iv.size()} {
  }

  std::string encrypt(std::string_view plain);
  std::string decrypt(std::string_view cipher);

 private:
  CryptoPP::SecByteBlock key_{}, iv_{};
};
}  // namespace encryption

#endif  // ENCRYPTION_H