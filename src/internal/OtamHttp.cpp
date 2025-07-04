#include "internal/OtamHttp.h"

String OtamHttp::apiKey;

OtamHttpResponse OtamHttp::get(String url) {
    HTTPClient http;

    http.begin(url);
    http.addHeader("x-api-key", apiKey);

    int httpCode = http.GET();
    String response = http.getString();

    http.end();

    return {httpCode, response};
}

OtamHttpResponse OtamHttp::post(String url, String payload) {
    HTTPClient http;

    http.begin(url);
    http.addHeader("x-api-key", apiKey);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(payload);
    String response = http.getString();

    http.end();

    return {httpCode, payload: response};
}