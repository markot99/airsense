#include "main/hal/non_volatile_storage/non_volatile_storage.h"

#include "main/logger/logger.h"

NonVolatileStorage::NonVolatileStorage() : m_initialized(false) { init(); }

NonVolatileStorage::~NonVolatileStorage() {}

void NonVolatileStorage::init() {
  Logger::info("Initializing non volatile storage.");
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  m_initialized = true;
  Logger::info("Finished initializing non volatile storage.");
}

bool NonVolatileStorage::isInitialized() { return m_initialized; }

bool NonVolatileStorage::erase() {
  esp_err_t err = nvs_flash_erase();
  if (err != ESP_OK) {
    Logger::error("Error erasing NVS partition!" +
                  std::string(esp_err_to_name(err)));
    return false;
  }
  return true;
}

bool NonVolatileStorage::openStorage(const std::string& namespace_name,
                                     nvs_handle_t* handle) {
  esp_err_t err = nvs_open(namespace_name.c_str(), NVS_READWRITE, handle);
  if (err != ESP_OK) {
    Logger::error("Error opening NVS handle!" +
                  std::string(esp_err_to_name(err)));
    return false;
  }
  return true;
}

void NonVolatileStorage::closeStorage(nvs_handle_t handle) {
  nvs_close(handle);
}

bool NonVolatileStorage::evaluateReadError(esp_err_t err,
                                           const std::string& namespace_name,
                                           const std::string& key) {
  switch (err) {
    case ESP_OK:
      return true;
    case ESP_ERR_NVS_NOT_FOUND:
      Logger::warn("The key '" + key + "' in the namespace '" + namespace_name +
                   "' is not initialized yet! Setting to 0!\n");
      return true;
    default:
      Logger::error("Error reading key '" + key + "' in the namespace '" +
                    namespace_name + "'! " + std::string(esp_err_to_name(err)) +
                    "\n");
      return false;
  }
}

bool NonVolatileStorage::evaluateWriteError(esp_err_t err,
                                            const std::string& namespace_name,
                                            const std::string& key) {
  if (err != ESP_OK) {
    Logger::error("Error setting key '" + key + "' in the namespace '" +
                  namespace_name + "'! " + std::string(esp_err_to_name(err)) +
                  "\n");
    return false;
  }
  return true;
}

bool NonVolatileStorage::persistValue(nvs_handle_t handle,
                                      const std::string& namespace_name,
                                      const std::string& key) {
  esp_err_t err = nvs_commit(handle);
  if (err != ESP_OK) {
    Logger::error("Error committing key '" + key + "' in the namespace '" +
                  namespace_name + "'! " + std::string(esp_err_to_name(err)) +
                  "\n");
    return false;
  }
  return true;
}

bool NonVolatileStorage::getValue(const std::string& namespace_name,
                                  const std::string& key, int8_t* value) {
  if (isNullPointer(value)) {
    return false;
  }

  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_get_i8(handle, key.c_str(), value);

  closeStorage(handle);

  return evaluateReadError(err, namespace_name, key);
}

bool NonVolatileStorage::getValue(const std::string& namespace_name,
                                  const std::string& key, uint8_t* value) {
  if (isNullPointer(value)) {
    return false;
  }

  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_get_u8(handle, key.c_str(), value);

  closeStorage(handle);

  return evaluateReadError(err, namespace_name, key);
}

bool NonVolatileStorage::getValue(const std::string& namespace_name,
                                  const std::string& key, int16_t* value) {
  if (isNullPointer(value)) {
    return false;
  }

  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_get_i16(handle, key.c_str(), value);

  closeStorage(handle);

  return evaluateReadError(err, namespace_name, key);
}

bool NonVolatileStorage::getValue(const std::string& namespace_name,
                                  const std::string& key, uint16_t* value) {
  if (isNullPointer(value)) {
    return false;
  }

  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_get_u16(handle, key.c_str(), value);

  closeStorage(handle);

  return evaluateReadError(err, namespace_name, key);
}

bool NonVolatileStorage::getValue(const std::string& namespace_name,
                                  const std::string& key, int32_t* value) {
  if (isNullPointer(value)) {
    return false;
  }

  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_get_i32(handle, key.c_str(), value);

  closeStorage(handle);

  return evaluateReadError(err, namespace_name, key);
}

bool NonVolatileStorage::getValue(const std::string& namespace_name,
                                  const std::string& key, uint32_t* value) {
  if (isNullPointer(value)) {
    return false;
  }

  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_get_u32(handle, key.c_str(), value);

  closeStorage(handle);

  return evaluateReadError(err, namespace_name, key);
}

bool NonVolatileStorage::getValue(const std::string& namespace_name,
                                  const std::string& key, int64_t* value) {
  if (isNullPointer(value)) {
    return false;
  }

  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_get_i64(handle, key.c_str(), value);

  closeStorage(handle);

  return evaluateReadError(err, namespace_name, key);
}

bool NonVolatileStorage::getValue(const std::string& namespace_name,
                                  const std::string& key, uint64_t* value) {
  if (isNullPointer(value)) {
    return false;
  }

  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_get_u64(handle, key.c_str(), value);

  closeStorage(handle);

  return evaluateReadError(err, namespace_name, key);
}

bool NonVolatileStorage::getValue(const std::string& namespace_name,
                                  const std::string& key, std::string* value) {
  if (isNullPointer(value)) {
    return false;
  }

  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }
  size_t required_size;
  esp_err_t err = nvs_get_str(handle, key.c_str(), NULL, &required_size);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
    closeStorage(handle);
    return false;
  }

  char* buffer = static_cast<char*>(malloc(required_size));
  err = nvs_get_str(handle, key.c_str(), buffer, &required_size);

  bool read_success = true;
  if (err == ESP_OK) {
    *value = std::string(buffer);
  } else {
    Logger::error("Failed to read string from NVS! Error: " +
                  std::string(esp_err_to_name(err)) + "\n");
    read_success = false;
  }
  free(buffer);

  closeStorage(handle);

  return read_success;
}

bool NonVolatileStorage::setValue(const std::string& namespace_name,
                                  const std::string& key, int8_t value) {
  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_set_i8(handle, key.c_str(), value);
  if (!evaluateWriteError(err, namespace_name, key)) {
    closeStorage(handle);
    return false;
  }

  bool persist_status = persistValue(handle, namespace_name, key);
  closeStorage(handle);
  return persist_status;
}

bool NonVolatileStorage::setValue(const std::string& namespace_name,
                                  const std::string& key, uint8_t value) {
  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_set_u8(handle, key.c_str(), value);
  if (!evaluateWriteError(err, namespace_name, key)) {
    closeStorage(handle);
    return false;
  }

  bool persist_status = persistValue(handle, namespace_name, key);
  closeStorage(handle);
  return persist_status;
}

bool NonVolatileStorage::setValue(const std::string& namespace_name,
                                  const std::string& key, int16_t value) {
  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_set_i16(handle, key.c_str(), value);
  if (!evaluateWriteError(err, namespace_name, key)) {
    closeStorage(handle);
    return false;
  }

  bool persist_status = persistValue(handle, namespace_name, key);
  closeStorage(handle);
  return persist_status;
}

bool NonVolatileStorage::setValue(const std::string& namespace_name,
                                  const std::string& key, uint16_t value) {
  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_set_u16(handle, key.c_str(), value);
  if (!evaluateWriteError(err, namespace_name, key)) {
    closeStorage(handle);
    return false;
  }

  bool persist_status = persistValue(handle, namespace_name, key);
  closeStorage(handle);
  return persist_status;
}

bool NonVolatileStorage::setValue(const std::string& namespace_name,
                                  const std::string& key, int32_t value) {
  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_set_i32(handle, key.c_str(), value);
  if (!evaluateWriteError(err, namespace_name, key)) {
    closeStorage(handle);
    return false;
  }

  bool persist_status = persistValue(handle, namespace_name, key);
  closeStorage(handle);
  return persist_status;
}

bool NonVolatileStorage::setValue(const std::string& namespace_name,
                                  const std::string& key, uint32_t value) {
  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_set_u32(handle, key.c_str(), value);
  if (!evaluateWriteError(err, namespace_name, key)) {
    closeStorage(handle);
    return false;
  }

  bool persist_status = persistValue(handle, namespace_name, key);
  closeStorage(handle);
  return persist_status;
}

bool NonVolatileStorage::setValue(const std::string& namespace_name,
                                  const std::string& key, int64_t value) {
  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_set_i64(handle, key.c_str(), value);
  if (!evaluateWriteError(err, namespace_name, key)) {
    closeStorage(handle);
    return false;
  }

  bool persist_status = persistValue(handle, namespace_name, key);
  closeStorage(handle);
  return persist_status;
}

bool NonVolatileStorage::setValue(const std::string& namespace_name,
                                  const std::string& key, uint64_t value) {
  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_set_u64(handle, key.c_str(), value);
  if (!evaluateWriteError(err, namespace_name, key)) {
    closeStorage(handle);
    return false;
  }

  bool persist_status = persistValue(handle, namespace_name, key);
  closeStorage(handle);
  return persist_status;
}

bool NonVolatileStorage::setValue(const std::string& namespace_name,
                                  const std::string& key,
                                  const std::string& value) {
  nvs_handle_t handle;
  if (!openStorage(namespace_name, &handle)) {
    return false;
  }

  esp_err_t err = nvs_set_str(handle, key.c_str(), value.c_str());
  if (!evaluateWriteError(err, namespace_name, key)) {
    closeStorage(handle);
    return false;
  }

  bool persist_status = persistValue(handle, namespace_name, key);
  closeStorage(handle);
  return persist_status;
}
