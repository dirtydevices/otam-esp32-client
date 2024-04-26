#ifndef OTAM_UTILS_H
#define OTAM_UTILS_H

#include <Arduino.h>
#include <cJSON.h>

class OtamUtils {
   public:
    static cJSON* parseJSON(String json);
    static String getJSONValue(cJSON* json, String key);
};

#endif  // OTAM_UTILS_H