#ifndef OTAM_HTTP_H
#define OTAM_HTTP_H

#include <HTTPClient.h>

struct OtamHttpResponse {
    int httpCode;
    String payload;
};

class OtamHttp {
   public:
    static String apiKey;
    static OtamHttpResponse get(String url);
    static OtamHttpResponse post(String url, String payload);
};

#endif  // OTAM_HTTP_H
