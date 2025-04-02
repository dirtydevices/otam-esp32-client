#include "internal/LightJson.h"

String LightJson::getValue(const char* json, const char* key) {
    const char* valueStart = findValueStart(json, key);
    if (!valueStart)
        return "";
    return extractValue(valueStart);
}

int LightJson::getIntValue(const char* json, const char* key) {
    const char* valueStart = findValueStart(json, key);
    if (!valueStart)
        return 0;
    return atoi(valueStart);
}

bool LightJson::hasKey(const char* json, const char* key) {
    return findKey(json, key) != nullptr;
}

String LightJson::createObject(const char* key, const char* value) {
    return createObject(key, value, true);
}

String LightJson::createObject(const char* key, int value) {
    char valueStr[12];
    itoa(value, valueStr, 10);
    return createObject(key, valueStr, false);
}

String LightJson::createObject(const char* key, const char* value, bool isString) {
    String result = "{\"";
    result += key;
    result += "\":";
    if (isString) {
        result += "\"";
        result += value;
        result += "\"";
    } else {
        result += value;
    }
    result += "}";
    return result;
}

String LightJson::mergeObjects(const char* obj1, const char* obj2) {
    String result = obj1;
    // Remove the closing brace from obj1
    result = result.substring(0, result.length() - 1);
    // Add comma and obj2 without opening brace
    result += "," + String(obj2).substring(1);
    return result;
}

const char* LightJson::findKey(const char* json, const char* key) {
    const char* current = json;
    while (*current) {
        if (*current == '"') {
            current++;
            const char* keyStart = current;
            while (*current && *current != '"')
                current++;
            if (*current == '"') {
                String foundKey = String(keyStart).substring(0, current - keyStart);
                if (foundKey == key) {
                    return current + 1;
                }
            }
        }
        current++;
    }
    return nullptr;
}

const char* LightJson::findValueStart(const char* json, const char* key) {
    const char* keyEnd = findKey(json, key);
    if (!keyEnd)
        return nullptr;

    while (*keyEnd && *keyEnd != ':')
        keyEnd++;
    if (*keyEnd == ':')
        keyEnd++;

    while (*keyEnd && isspace(*keyEnd))
        keyEnd++;
    return keyEnd;
}

const char* LightJson::findValueEnd(const char* json) {
    const char* current = json;
    if (*current == '"') {
        current++;
        while (*current && *current != '"')
            current++;
        if (*current == '"')
            current++;
    } else if (*current == '{') {
        int depth = 1;
        current++;
        while (*current && depth > 0) {
            if (*current == '{')
                depth++;
            if (*current == '}')
                depth--;
            current++;
        }
    } else if (*current == '[') {
        int depth = 1;
        current++;
        while (*current && depth > 0) {
            if (*current == '[')
                depth++;
            if (*current == ']')
                depth--;
            current++;
        }
    } else {
        while (*current && *current != ',' && *current != '}')
            current++;
    }
    return current;
}

String LightJson::extractValue(const char* json) {
    const char* end = findValueEnd(json);
    if (!end)
        return "";
    String value = String(json).substring(0, end - json);
    // Remove quotes if present
    if (value.startsWith("\"") && value.endsWith("\"")) {
        value = value.substring(1, value.length() - 1);
    }
    return value;
}

bool LightJson::isStringValue(const char* json) {
    return *json == '"';
}