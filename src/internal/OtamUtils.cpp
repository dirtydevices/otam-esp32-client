#include "internal/OtamUtils.h"
#include "internal/LightJson.h"

String OtamUtils::getJSONValue(const char* json, const char* key) {
    return LightJson::getValue(json, key);
}

int OtamUtils::getJSONIntValue(const char* json, const char* key) {
    return LightJson::getIntValue(json, key);
}

bool OtamUtils::hasJSONKey(const char* json, const char* key) {
    return LightJson::hasKey(json, key);
}

String OtamUtils::createJSONObject(const char* key, const char* value) {
    return LightJson::createObject(key, value);
}

String OtamUtils::createJSONObject(const char* key, int value) {
    return LightJson::createObject(key, value);
}

String OtamUtils::mergeJSONObjects(const char* obj1, const char* obj2) {
    return LightJson::mergeObjects(obj1, obj2);
}