#include "Mqtt.h"

#include <QNEthernet.h>
#include <PubSubClient.h>

using namespace qindesign::network;

namespace Mqtt
{
    const IPAddress broker(192, 168, 86, 52);
    int port = 1883;

    EthernetClient net;
    PubSubClient client(net);

    long lastReconnectAttempt = 0;

    boolean reconnect()
    {
        Serial.print("Attempting to connect to the MQTT broker: ");
        Serial.println(broker);
        if (client.connect("teensy"))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" trying again in 5 seconds");
        }
        return client.connected();
    }

    void setup()
    {
        client.setServer(broker, port);
        lastReconnectAttempt = 0;
    }

    void loop()
    {
        if (!client.connected())
        {
            long now = millis();
            if (now - lastReconnectAttempt > 5000)
            {
                lastReconnectAttempt = now;
                // Attempt to reconnect
                if (reconnect())
                {
                    lastReconnectAttempt = 0;
                }
            }
        }
        else
        {
            // Client connected
            client.loop();
        }
    }

    boolean maybePublish(const char *topic, const char *payload)
    {
        if (!client.connected())
        {
            return false;
        }
        return client.publish(topic, payload);
    }

    boolean maybePublish(const char *topic, const float payload)
    {
        String str(payload);
        return maybePublish(topic, str);
    }

    boolean maybePublish(const char *topic, const double payload)
    {
        String str(payload);
        return maybePublish(topic, str);
    }

    boolean maybePublish(const char *topic, const uint8_t payload)
    {
        String str(payload);
        return maybePublish(topic, str);
    }

    boolean maybePublish(const char *topic, const uint32_t payload)
    {
        String str(payload);
        return maybePublish(topic, str);
    }

    boolean maybePublish(const char *topic, const int32_t payload)
    {
        String str(payload);
        return maybePublish(topic, str);
    }

    boolean maybePublish(const char *topic, const String& payload)
    {
        return maybePublish(topic, payload.c_str());
    }
}