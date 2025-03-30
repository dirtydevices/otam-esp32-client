#ifndef LIGHT_JSON_H
#define LIGHT_JSON_H

#include <Arduino.h>

class LightJson {
   public:
    static String getValue(const char* json, const char* key);
    static int getIntValue(const char* json, const char* key);
    static bool hasKey(const char* json, const char* key);
    static String createObject(const char* key, const char* value);
    static String createObject(const char* key, int value);
    static String createObject(const char* key, const char* value, bool isString);
    static String mergeObjects(const char* obj1, const char* obj2);

   private:
    static const char* findKey(const char* json, const char* key);
    static const char* findValueStart(const char* json, const char* key);
    static const char* findValueEnd(const char* json);
    static String extractValue(const char* json);
    static bool isStringValue(const char* json);
};

#endif  // LIGHT_JSON_H