#include "json_util.h"
#include "esphome/core/log.h"
#include "esp_heap_caps.h"

namespace esphome {
namespace json {

static const char *const TAG = "json";

template <typename JsonDocType>
std::string process_json(const json_build_t &f, size_t &request_size, size_t free_mem) {
  while (true) {
    ESP_LOGV(TAG, "Allocating %zu bytes for JSON serialization", request_size);
    JsonDocType json_document(request_size);
    JsonObject root = json_document.template to<JsonObject>();
    f(root);

    if (json_document.overflowed()) {
      request_size = std::min(request_size * 2, free_mem);
      continue;
    }
    
    json_document.shrinkToFit();
    std::string output;
    serializeJson(json_document, output);
    return output;
  }
}

std::string build_json(const json_build_t &f) {
  size_t free_psram = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
  size_t free_internal = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
  
  bool use_psram = (free_psram > 512);
  size_t request_size = std::min(use_psram ? free_psram : free_internal, (size_t)512);
  
  return use_psram ? process_json<PSRAMJsonDocument>(f, request_size, free_psram)
                   : process_json<DynamicJsonDocument>(f, request_size, free_internal);
}

template <typename JsonDocType>
bool process_json_parse(const std::string &data, const json_parse_t &f, size_t &request_size, size_t free_mem) {
  while (true) {
    ESP_LOGV(TAG, "Allocating %zu bytes for JSON parsing", request_size);
    JsonDocType json_document(request_size);
    DeserializationError err = deserializeJson(json_document, data);
    json_document.shrinkToFit();

    if (err == DeserializationError::Ok) {
      return f(json_document.template as<JsonObject>());
    } else if (err == DeserializationError::NoMemory) {
      if (request_size * 2 >= free_mem) {
        ESP_LOGE(TAG, "Not enough memory to allocate larger JSON buffer.");
        return false;
      }
      ESP_LOGV(TAG, "Increasing memory allocation.");
      request_size *= 2;
      continue;
    } else {
      ESP_LOGE(TAG, "JSON parse error: %s", err.c_str());
      return false;
    }
  }
}

bool parse_json(const std::string &data, const json_parse_t &f) {
  size_t free_psram = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
  size_t free_internal = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
  
  bool use_psram = (free_psram > data.size() * 1.5);
  size_t request_size = std::min(use_psram ? free_psram : free_internal, (size_t)(data.size() * 1.5));
  
  return use_psram ? process_json_parse<PSRAMJsonDocument>(data, f, request_size, free_psram)
                   : process_json_parse<DynamicJsonDocument>(data, f, request_size, free_internal);
}

}  // namespace json
}  // namespace esphome