#include "data_representation.h"

#include <fstream>  // std::ifstream
#include <iomanip>  // std::setfill
#include <sstream>  // std::ostringstream

// SHA256::Init, SHA256::Update, SHA256::Final
#include <cryptopp/sha.h>

std::string data_representation::byte_array_as_hex_string(const std::vector<unsigned char>& in) {
  std::ostringstream stream{};

  stream << std::hex << std::setfill('0');

  for (const auto i : in) {
    stream << std::setw(2) << static_cast<unsigned int>(i);
  }

  return stream.str();
}

std::vector<unsigned char> data_representation::hex_string_as_byte_array(std::string_view in) {
  std::vector<unsigned char> out{};

  std::istringstream input_stream{in.data()};
  std::string str{};

  while ((input_stream >> std::uppercase >> std::setw(2) >> str)) {
    std::uint32_t byte{};
    std::istringstream stream{str};
    stream >> std::setbase(16) >> byte;
    out.push_back(byte);
  }

  return out;
}

std::string data_representation::file_as_hex_string(std::string_view path) {
  std::ifstream file{path.data(), std::ios::binary};
  if (file.fail()) {
    return {};
  }

  std::stringstream stream{};
  stream << std::hex << std::uppercase << std::setfill('0');

  auto it = std::istreambuf_iterator<char>(file);
  auto eof = std::istreambuf_iterator<char>();

  while (!it.equal(eof)) {
    const int c{*it++ & 0xFF};
    stream << std::setw(2) << c;
  }

  file.close();
  return stream.str();
}

std::string data_representation::sha256_string(const char* in) {
  const auto hash = [in]() {
    using namespace CryptoPP;

    SHA256 hash;
    std::string digest;

    hash.Update(reinterpret_cast<const byte*>(in), std::strlen(in));
    digest.resize(hash.DigestSize());
    hash.Final(reinterpret_cast<byte*>(&digest[0]));

    return std::vector<unsigned char>(digest.begin(), digest.end());
  }();

  return byte_array_as_hex_string(hash);
}

std::string data_representation::sha256_file(std::string_view path) {
  std::ifstream file{path.data(), std::ios::in | std::ios::binary};
  if (file.fail()) {
    return {};
  }

  const auto hash = [&file]() {
    using namespace CryptoPP;

    SHA256 hash;
    char buffer[1 << 12]{};

    while (file.good()) {
      file.read(buffer, sizeof(buffer));
      hash.Update(reinterpret_cast<const byte*>(buffer), file.gcount());
    }

    std::string digest;
    digest.resize(hash.DigestSize());
    hash.Final(reinterpret_cast<byte*>(&digest[0]));

    return std::vector<unsigned char>(digest.begin(), digest.end());
  }();

  return byte_array_as_hex_string(hash);
}

std::string data_representation::sha256_byte_array(const std::vector<unsigned char>& in) {
  const auto hash = [in]() {
    using namespace CryptoPP;

    SHA256 hash;
    std::string digest;

    hash.Update(reinterpret_cast<const byte*>(in.data()), in.size());
    digest.resize(hash.DigestSize());
    hash.Final(reinterpret_cast<byte*>(&digest[0]));

    return std::vector<unsigned char>(digest.begin(), digest.end());
  }();

  return byte_array_as_hex_string(hash);
}
