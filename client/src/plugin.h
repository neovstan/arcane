#ifndef ARCANE_CLIENT_SRC_PLUGIN_H
#define ARCANE_CLIENT_SRC_PLUGIN_H

#include <windows.h>

#include <stdexcept>

namespace modification::client {
class plugin {
 public:
  class initialization;

  class information {
    friend class initialization;

   public:
    [[nodiscard]] HMODULE module_handle() const {
      return data_.module_handle;
    }

    [[nodiscard]] SIZE_T size_of_image() const {
      return data_.size_of_image;
    }
    [[nodiscard]] const char* data() const {
      return data_.data;
    }

   private:
    struct initialization_data {
      HMODULE module_handle{};
      SIZE_T size_of_image{};
      const char* data{};
    } data_;
  };

  static information& properties() {
    static information instance{};
    return instance;
  }
};

template <class T>
class singleton {
 public:
  singleton() {
    if (!plugin::properties().module_handle()) {
      throw instancing_in_global_object_exception{};
    }
  }

  singleton(const singleton&) = delete;
  singleton(singleton&&) = delete;

  class instancing_in_global_object_exception final : public std::exception {
   public:
    [[nodiscard]] const char* what() const noexcept override {
      return "attempt to call modification::client::singleton::instance() in "
             "global "
             "object constructor";
    }
  };

  static T& instance() {
    static T instance{};
    return instance;
  }
};
}  // namespace modification::client

#endif  // ARCANE_CLIENT_SRC_PLUGIN_H
