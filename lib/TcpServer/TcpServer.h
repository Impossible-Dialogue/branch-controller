#pragma once

// Code for listening, as a service, on a TCP port
// and responding appropriately.

#include <Arduino.h>

namespace TcpServer {

    void setup();
    void loop();

    bool initialized();
    void networkChanged(bool hasIP, bool linkState);

}
