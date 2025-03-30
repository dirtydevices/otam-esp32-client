#ifndef OTAM_UTILS_H
#define OTAM_UTILS_H

#include <Arduino.h>

class OtamUtils {
   public:
    static String getJSONValue(const char* json, const char* key);
    static int getJSONIntValue(const char* json, const char* key);
    static bool hasJSONKey(const char* json, const char* key);
    static String createJSONObject(const char* key, const char* value);
    static String createJSONObject(const char* key, int value);
    static String mergeJSONObjects(const char* obj1, const char* obj2);
};

#endif  // OTAM_UTILS_H