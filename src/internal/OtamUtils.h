#include <Arduino.h>
#include <cJSON.h>

class OtamUtils {
   public:
    static cJSON* parseJSON(String json) {
        cJSON* parsed = cJSON_Parse(json.c_str());

        if (parsed == NULL) {
            const char* error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                Serial.println("Error before: " + String(error_ptr));
            }
            return NULL;
        }

        return parsed;
    }

    static String getJSONValue(cJSON* json, String key) {
        cJSON* item = cJSON_GetObjectItemCaseSensitive(json, key.c_str());

        if (cJSON_IsString(item) && (item->valuestring != NULL)) {
            return String(item->valuestring);
        } else if (cJSON_IsNumber(item)) {
            return String(item->valuedouble);
        }

        return "";
    }
};
