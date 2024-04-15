#ifndef _MQTT_H_
#define _MQTT_H_

#include <Arduino.h>

//
// Implements an interface for MQTT
//

namespace Mqtt {

    void setup();
    void loop();
    boolean maybePublish(const char* topic, const char* payload);
    boolean maybePublish(const char *topic, const float payload);
    boolean maybePublish(const char *topic, const double payload);
    boolean maybePublish(const char *topic, const uint8_t payload);
    boolean maybePublish(const char *topic, const uint32_t payload);
    boolean maybePublish(const char *topic, const int32_t payload);
    boolean maybePublish(const char *topic, const String& payload);
    
}

#endif /* _MQTT_H_ */