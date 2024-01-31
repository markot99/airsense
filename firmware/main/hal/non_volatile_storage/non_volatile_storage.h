#pragma once

#include <string>

#include "main/logger/logger.h"
#include "nvs.h"
#include "nvs_flash.h"

//! @brief Non-volatile storage
class NonVolatileStorage {
 public:
  //! @brief Constructor
  NonVolatileStorage();

  //! @brief Destructor
  ~NonVolatileStorage();

  //! @brief Initialize the non-volatile storage.
  void init();

  //! @brief check if the non-volatile storage is initialized.
  //! @return True if initialized, false otherwise.
  bool isInitialized();

  //! @brief Erase the complete non-volatile storage.
  bool erase();

  //! @brief Get an int8 value from the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to store the result in.
  //! @return True if successful, false otherwise.
  bool getValue(const std::string& namespace_name, const std::string& key,
                int8_t* value);

  //! @brief Get an uint8 value from the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to store the result in.
  //! @return True if successful, false otherwise.
  bool getValue(const std::string& namespace_name, const std::string& key,
                uint8_t* value);

  //! @brief Get an int16 value from the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to store the result in.
  //! @return True if successful, false otherwise.
  bool getValue(const std::string& namespace_name, const std::string& key,
                int16_t* value);

  //! @brief Get an uint16 value from the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to store the result in.
  //! @return True if successful, false otherwise.
  bool getValue(const std::string& namespace_name, const std::string& key,
                uint16_t* value);

  //! @brief Get an int32 value from the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to store the result in.
  //! @return True if successful, false otherwise.
  bool getValue(const std::string& namespace_name, const std::string& key,
                int32_t* value);

  //! @brief Get an uint32 value from the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to store the result in.
  //! @return True if successful, false otherwise.
  bool getValue(const std::string& namespace_name, const std::string& key,
                uint32_t* value);

  //! @brief Get an int64 value from the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to store the result in.
  //! @return True if successful, false otherwise.
  bool getValue(const std::string& namespace_name, const std::string& key,
                int64_t* value);

  //! @brief Get an uint64 value from the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to store the result in.
  //! @return True if successful, false otherwise.
  bool getValue(const std::string& namespace_name, const std::string& key,
                uint64_t* value);

  //! @brief Get a string value from the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The string to store the result in.
  //! @return True if successful, false otherwise.
  bool getValue(const std::string& namespace_name, const std::string& key,
                std::string* value);

  //! @brief Set an int8 value in the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to set.
  //! @return True if successful, false otherwise.
  bool setValue(const std::string& namespace_name, const std::string& key,
                int8_t value);

  //! @brief Set an uint8 value in the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to set.
  //! @return True if successful, false otherwise.
  bool setValue(const std::string& namespace_name, const std::string& key,
                uint8_t value);

  //! @brief Set an int16 value in the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to set.
  //! @return True if successful, false otherwise.
  bool setValue(const std::string& namespace_name, const std::string& key,
                int16_t value);

  //! @brief Set an uint16 value in the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to set.
  //! @return True if successful, false otherwise.
  bool setValue(const std::string& namespace_name, const std::string& key,
                uint16_t value);

  //! @brief Set an int32 value in the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to set.
  //! @return True if successful, false otherwise.
  bool setValue(const std::string& namespace_name, const std::string& key,
                int32_t value);

  //! @brief Set an uint32 value in the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to set.
  //! @return True if successful, false otherwise.
  bool setValue(const std::string& namespace_name, const std::string& key,
                uint32_t value);

  //! @brief Set an int64 value in the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to set.
  //! @return True if successful, false otherwise.
  bool setValue(const std::string& namespace_name, const std::string& key,
                int64_t value);

  //! @brief Set an uint64 value in the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The value to set.
  //! @return True if successful, false otherwise.
  bool setValue(const std::string& namespace_name, const std::string& key,
                uint64_t value);

  //! @brief Set a string value in the non-volatile storage.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @param value The string to set.
  //! @return True if successful, false otherwise.
  bool setValue(const std::string& namespace_name, const std::string& key,
                const std::string& value);

 private:
  //! @brief Open the persisted storage
  //! @param namespace_name The namespace name.
  //! @param handle The handle to store the result in.
  //! @return True if successful, false otherwise.
  bool openStorage(const std::string& namespace_name, nvs_handle_t* handle);

  //! @brief Close the persisted storage
  //! @param handle The handle to close.
  void closeStorage(nvs_handle_t handle);

  //! @brief Persist a value
  //! @param handle The handle to use.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  bool persistValue(nvs_handle_t handle, const std::string& namespace_name,
                    const std::string& key);

  //! @brief Evaluate an error
  //! @param err The error to evaluate.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  bool evaluateReadError(esp_err_t err, const std::string& namespace_name,
                         const std::string& key);

  //! @brief Evaluate an error
  //! @param err The error to evaluate.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @return True if no error, false otherwise.
  bool evaluateWriteError(esp_err_t err, const std::string& namespace_name,
                          const std::string& key);

  //! @brief Evaluate an error
  //! @param err The error to evaluate.
  //! @param namespace_name The namespace name.
  //! @param key The key.
  //! @return True if no error, false otherwise.
  bool evaluateCommitError(esp_err_t err, const std::string& namespace_name,
                           const std::string& key);

  //! @brief Check if a pointer is null
  //! @param value The pointer to check.
  //! @return True if null, false otherwise.
  template <typename T>
  bool isNullPointer(T* value) {
    if (value == nullptr) {
      Logger::error("Null pointer passed!");
      return true;
    }
    return false;
  }

  //! @brief Initialization flag, true if initialized, false otherwise.
  bool m_initialized;
};
