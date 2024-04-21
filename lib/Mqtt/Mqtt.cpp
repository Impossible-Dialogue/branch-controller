#include "Mqtt.h"

#include <Logger.h>
#include <QNEthernet.h>
#include <PubSubClient.h>

using namespace qindesign::network;

namespace Mqtt
{
    const IPAddress broker(192, 168, 86, 52);
    int port = 1883;

    EthernetClient net;
    PubSubClient MqttClient(net);

    long lastReconnectAttempt = 0;

    boolean reconnect()
    {
        Logger.print("Attempting to connect to the MQTT broker: ");
        Logger.println(broker);
        if (MqttClient.connect("teensy"))
        {
            Logger.println("MPTT connected");
        }
        else
        {
            Logger.print("failed, rc=");
            Logger.print(MqttClient.state());
            Logger.println(" trying again in 5 seconds");
        }
        return MqttClient.connected();
    }

    void setup()
    {
        Logger.setClient(MqttClient);
        MqttClient.setServer(broker, port);
        lastReconnectAttempt = 0;
        Logger.println("MQTT ready");
    }

    void loop()
    {
        if (!MqttClient.connected())
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
            MqttClient.loop();
        }
    }
}