#include "data_representation.h"

#include <fstream>  // std::ifstream
#include <iomanip>  // std::setfill
#include <sstream>  // std::ostringstream

// SHA256_Init, SHA256_Update, SHA256_Final
#include <openssl/sha.h>

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

std::string data_representation::sha256_string(std::string_view in) {
  const auto hash = [in]() {
    unsigned char hash[SHA256_DIGEST_LENGTH]{};

    SHA256_CTX context{};
    SHA256_Init(&context);
    SHA256_Update(&context, in.data(), in.length());
    SHA256_Final(hash, &context);

    return std::vector<unsigned char>(hash, hash + sizeof(hash));
  }();

  return byte_array_as_hex_string(hash);
}

std::string data_representation::sha256_file(std::string_view path) {
  std::ifstream file{path.data(), std::ios::in | std::ios::binary};
  if (file.fail()) {
    return {};
  }

  const auto hash = [&file]() {
    SHA256_CTX context{};
    SHA256_Init(&context);

    char buffer[1 << 12]{};
    unsigned char hash[SHA256_DIGEST_LENGTH]{};

    while (file.good()) {
      file.read(buffer, sizeof(buffer));
      SHA256_Update(&context, buffer, file.gcount());
    }

    SHA256_Final(hash, &context);
    return std::vector<unsigned char>(hash, hash + sizeof(hash));
  }();

  return byte_array_as_hex_string(hash);
}

std::string data_representation::sha256_byte_array(const std::vector<unsigned char>& in) {
  const auto hash = [in]() {
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256_CTX context{};
    SHA256_Init(&context);
    SHA256_Update(&context, in.data(), in.size());
    SHA256_Final(hash, &context);

    return std::vector<unsigned char>(hash, hash + sizeof(hash));
  }();

  return byte_array_as_hex_string(hash);
}
