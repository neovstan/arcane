#include "utils.h"

#include <sstream>

#include <infoware/infoware.hpp>
#include <data_representation/data_representation.h>

std::string utils::hwid() {
  std::stringstream stream;

  stream << iware::system::OS_info().full_name.c_str();
  stream << iware::cpu::model_name().c_str();
  stream << iware::system::memory().physical_total;

  for (const auto& gpu : iware::gpu::device_properties()) {
    stream << gpu.name.c_str();
    stream << gpu.memory_size;
  }

  return data_representation::sha256_string(stream.str().c_str());
}
