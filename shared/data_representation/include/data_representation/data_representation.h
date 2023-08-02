#ifndef DATA_REPRESENTATION_H
#define DATA_REPRESENTATION_H

#include <string>
#include <vector>

namespace data_representation {
std::string byte_array_as_hex_string(const std::vector<unsigned char>& in);
std::vector<unsigned char> hex_string_as_byte_array(std::string_view in);
std::string file_as_hex_string(std::string_view path);
std::string sha256_string(std::string_view in);
std::string sha256_file(std::string_view path);
std::string sha256_byte_array(const std::vector<unsigned char>& in);
}  // namespace data_representation

#endif  // DATA_REPRESENTATION_H
