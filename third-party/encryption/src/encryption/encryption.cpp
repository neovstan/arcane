#include "encryption.h"

std::string encryption::cbc::encrypt(const std::string& plain) {
  std::string cipher{};

  CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption cbc_encryption{
      key_, key_.size(), iv_};

  CryptoPP::StreamTransformationFilter stf_encryption{
      cbc_encryption,
      new CryptoPP::Base64Encoder{new CryptoPP::StringSink{cipher}},
      CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING};

  stf_encryption.Put(reinterpret_cast<const unsigned char*>(plain.c_str()),
                     plain.length() + 1);

  stf_encryption.MessageEnd();

  return cipher;
}

std::string encryption::cbc::decrypt(const std::string& cipher) {
  std::string aes_encrypt_data{};
  CryptoPP::Base64Decoder decoder{};
  decoder.Attach(new CryptoPP::StringSink{aes_encrypt_data});
  decoder.Put(reinterpret_cast<const unsigned char*>(cipher.c_str()),
              cipher.length());
  decoder.MessageEnd();

  std::string recovered{};

  CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption cbc_decryption{
      key_, key_.size(), iv_};
  CryptoPP::StreamTransformationFilter stf_decryption{
      cbc_decryption, new CryptoPP::StringSink{recovered},
      CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING};

  stf_decryption.Put(
      reinterpret_cast<const unsigned char*>(aes_encrypt_data.c_str()),
      aes_encrypt_data.length());
  stf_decryption.MessageEnd();

  return recovered;
}
