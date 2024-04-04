#ifndef _OTA_H_
#define _OTA_H_

#include <Arduino.h>

//
// Implements a web server used for updating the firmware
//

class AsyncWebServerRequest;

namespace Ota {

    void setup();
    void loop();

    void handleNotFound(AsyncWebServerRequest *request);
    void handleRequest(AsyncWebServerRequest *request);
    void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
}

#endif /* _OTA_H_ */