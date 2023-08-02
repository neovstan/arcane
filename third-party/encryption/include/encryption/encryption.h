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
  cbc(const std::string& key, const std::string& iv)
      : key_{reinterpret_cast<const CryptoPP::byte*>(key.c_str()), key.size()},
        iv_{reinterpret_cast<const CryptoPP::byte*>(iv.c_str()), iv.size()} {}

  std::string encrypt(const std::string& plain);
  std::string decrypt(const std::string& cipher);

 private:
  CryptoPP::SecByteBlock key_{}, iv_{};
};
}  // namespace encryption

#endif  // ENCRYPTION_H