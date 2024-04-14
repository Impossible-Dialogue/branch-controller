#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <Arduino.h>
#include <Print.h>
#include <PubSubClient.h>

enum MqttLoggerMode
{
    MqttAndSerialFallback = 0,
    SerialOnly = 1,
    MqttOnly = 2,
    MqttAndSerial = 3,
};

class MqttLogger : public Print
{
private:
    const char *topic;
    uint8_t *buffer;
    uint8_t *bufferEnd;
    uint16_t bufferCnt = 0, bufferSize = 0;
    PubSubClient *client;
    MqttLoggerMode mode;
    void sendBuffer();

public:
    MqttLogger(const char *topic, MqttLoggerMode mode = MqttLoggerMode::MqttAndSerialFallback);
    MqttLogger(PubSubClient &client, const char *topic, MqttLoggerMode mode = MqttLoggerMode::MqttAndSerialFallback);
    ~MqttLogger();

    void setClient(PubSubClient &client);
    void setTopic(const char *topic);
    void setMode(MqttLoggerMode mode);
    void setRetained(boolean retained);

    virtual size_t write(uint8_t);
    using Print::write;

    uint16_t getBufferSize();
    boolean setBufferSize(uint16_t size);
};

extern MqttLogger Logger;

#endif