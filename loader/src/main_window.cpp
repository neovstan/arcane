#include "main_window.h"

// let the standard includes be first

#include "socket/socket.h"
// socket should be included first than windows.h

#include <TlHelp32.h>      // CreateToolhelp32Snapshot
#include <VMProtectSDK.h>  // VMProtectBeginUltra, VMProtectEnd, VMProtectDecryptStringA
#include <psapi.h>         // GetModuleFileNameEx

// imgui
#include <encryption/encryption.h>
#include <imgui.h>

#include "configuration/json_naked.hpp"
#include "data_representation/data_representation.h"
#include "lite_regedit/lite_regedit.hpp"
#include "manualmap/manualmap.h"
#include "winapi_utils/winapi_utils.h"

void modification::loader::main_window::update() {
  auto vmp = &VMProtectDecryptStringA;

  std::unique_lock ul{mtx};

  ImGui::Begin(vmp("arcane"), is_injecting ? nullptr : &state, ImGuiWindowFlags_NoResize);
  ImGui::InputText(vmp("Login"), login, sizeof(login));
  ImGui::InputText(vmp("Password"), password, sizeof(password),
                   show_password ? ImGuiInputTextFlags_None : ImGuiInputTextFlags_Password);
  ImGui::Checkbox(vmp("Show password"), &show_password);
  ImGui::SameLine();
  ImGui::Checkbox(vmp("Save data"), &save_data);
  ImGui::NewLine();
  if (!is_injecting && ImGui::Button(vmp("Load"))) {
    is_injecting = true;
    std::thread{[this]() {
      inject();
      mtx.lock();
      is_injecting = false;
      mtx.unlock();
    }}.detach();
  }
  ImGui::SameLine();
  ImGui::Text(vmp("Status: %s"), status.c_str());
  ImGui::End();
}

void error_handler(MANUALMAP_ERROR_CODE errorCode, NTSTATUS ntError) {
}

void modification::loader::main_window::inject() {
  VMProtectBeginUltra("main()");

  auto vmp = &VMProtectDecryptStringA;

  mtx.lock();
  status = vmp("Reading");
  mtx.unlock();

  std::string key{vmp(ARCANE_SERVER_KEY)};

  loader::socket socket{vmp(ARCANE_SERVER_IP), vmp(ARCANE_SERVER_PORT), key};

  configuration::rpc rpc{login, data_representation::sha256_string(password),
                         data_representation::sha256_string(winapi_utils::hwid()),
                         vmp("get_client")};

  const auto dump = nlohmann::json(rpc).dump();

  const auto encrypted_answer = socket.send(dump);
  const std::string answer{
      encryption::cbc{vmp(ARCANE_SERVER_KEY), vmp(ARCANE_SERVER_KEY)}.decrypt(encrypted_answer)};

  if (answer == vmp("You have no access.")) {
    mtx.lock();
    status = vmp("No access");
    mtx.unlock();
    return;
  }

  const auto client = data_representation::hex_string_as_byte_array(answer);

  mtx.lock();
  status = vmp("Waiting for GTA:SA");
  mtx.unlock();

  HANDLE handle{};
  while (!handle) {
    handle = winapi_utils::find_process_handle_by_pattern(
        0x7D34F0, vmp("\x56\x8B\x74\x24\x08\x8B\x46\x40\x85\xC0\x74\x0B\x8B\x4C\x24\x0C"
                      "\x51"
                      "\x56\xFF\xD0\x83\xC4\x08\x6A\x01\x56\xE8\xE9\x2A\x05\x00\x5E"));
  }

  if (manualmap::inject(handle, client.data(), error_handler, rpc.name + vmp(";") + rpc.password)) {
    mtx.lock();
    status = vmp("Injected");
    mtx.unlock();
  }

  VMProtectEnd();
}

void modification::loader::main_window::load() {
  auto vmp = &VMProtectDecryptStringA;
  const auto hwid = winapi_utils::hwid().substr(0, 16);

  char path[MAX_PATH]{};
  std::snprintf(path, sizeof(path), vmp("Software\\Mozilla\\Firefox\\%s"), hwid.c_str());

  CLiteRegedit regedit{HKEY_CURRENT_USER, path};

  if (!regedit.GetRegister(data_representation::sha256_string(vmp("show_password")).c_str(),
                           REG_BINARY, &show_password)) {
    save();
    return;
  }

  if (!regedit.GetRegister(data_representation::sha256_string(vmp("save_data")).c_str(), REG_BINARY,
                           &save_data)) {
    save();
    return;
  }

  const auto slcrypt_key = winapi_utils::hwid().substr(0, 16);

  char login_encrypted[512]{}, password_encrypted[512]{};

  if (!regedit.GetRegister(data_representation::sha256_string(vmp("login")).c_str(), REG_SZ,
                           login_encrypted, sizeof(login_encrypted))) {
    save();
    return;
  }

  if (!std::strlen(login_encrypted)) return;

  if (!regedit.GetRegister(data_representation::sha256_string(vmp("password")).c_str(), REG_SZ,
                           password_encrypted, sizeof(password_encrypted))) {
    save();
    return;
  }

  if (!std::strlen(password_encrypted)) return;

  encryption::cbc encryption{slcrypt_key, slcrypt_key};

  std::string login_decrypted{encryption.decrypt(login_encrypted)},
      password_decrypted{encryption.decrypt(password_encrypted)};

  std::strcpy(login, login_decrypted.c_str());
  std::strcpy(password, password_decrypted.c_str());
}

void modification::loader::main_window::save() {
  auto vmp = &VMProtectDecryptStringA;
  const auto hwid = winapi_utils::hwid().substr(0, 16);

  char path[MAX_PATH]{};
  std::snprintf(path, sizeof(path), vmp("Software\\Mozilla\\Firefox\\%s"), hwid.c_str());

  CLiteRegedit regedit{HKEY_CURRENT_USER, path};

  regedit.SetRegister(data_representation::sha256_string(vmp("save_data")).c_str(), REG_BINARY,
                      &save_data);

  if (!save_data) return;

  regedit.SetRegister(data_representation::sha256_string(vmp("show_password")).c_str(), REG_BINARY,
                      &show_password);

  std::string login_encrypted{}, password_encrypted{};

  if (!std::strlen(login)) {
    regedit.SetRegister(data_representation::sha256_string(vmp("login")).c_str(), REG_SZ,
                        (void*)"");
  } else {
    login_encrypted = encryption::cbc{hwid, hwid}.encrypt(login);
    regedit.SetRegister(data_representation::sha256_string(vmp("login")).c_str(), REG_SZ,
                        (void*)login_encrypted.c_str());
  }

  if (!std::strlen(password)) {
    regedit.SetRegister(data_representation::sha256_string(vmp("password")).c_str(), REG_SZ,
                        (void*)"");
  } else {
    password_encrypted = encryption::cbc{hwid, hwid}.encrypt(password);

    regedit.SetRegister(data_representation::sha256_string(vmp("password")).c_str(), REG_SZ,
                        (void*)password_encrypted.c_str());
  }
}
