#ifndef _MQTT_H_
#define _MQTT_H_

#include <Arduino.h>
#include <PubSubClient.h>

//
// Implements an interface for MQTT
//

namespace Mqtt
{
    extern PubSubClient MqttClient;

    template <typename T>
    class RateLimitedMqttPublisher
    {
    public:
        RateLimitedMqttPublisher(const char *topic, float max_publish_rate_per_sec = 10.0, PubSubClient *client = &MqttClient) : topic_(topic), max_publish_rate_per_sec_(max_publish_rate_per_sec), client_(client), last_publish_time_(0)
        {
        }

        boolean maybePublish(T payload)
        {
            if (max_publish_rate_per_sec_ <= 0)
                return false;

            const uint32_t current_time = millis();
            const float duration_since_last_publish_sec = (last_publish_time_ - current_time) / 1000.0;
            if (duration_since_last_publish_sec < 1.0 / max_publish_rate_per_sec_)
                return false;

            if (!client_->connected())
            {
                return false;
            }
            last_publish_time_ = current_time;
            return client_->publish(topic_, String(payload).c_str());
        }

    private:
        const char *topic_;
        float max_publish_rate_per_sec_;
        PubSubClient *client_;
        uint32_t last_publish_time_;
    };

    void setup();
    void loop();

}

#endif /* _MQTT_H_ */