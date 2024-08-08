#ifndef _RELAY_H_
#define _RELAY_H_

#include "Logger.h"
#include "Arduino.h"

class GpioRelay
{
public:
    GpioRelay(uint8_t pin) : pin_ (pin) {
        // initialize relay digital pin as an output.
        pinMode(pin_, OUTPUT);
        open();
    }

    bool is_open()
    {
        return !is_closed();
    }

    void open()
    {
        digitalWrite(pin_, LOW);
        is_closed_ = false;
        Logger.println("Setting relay to LOW");
    }

    bool is_closed()
    {
        return is_closed_;
    }

    void close()
    {
        digitalWrite(pin_, HIGH);
        is_closed_ = true;
        Logger.println("Setting relay to HIGH");
    }

    private:
        uint8_t pin_;
        bool is_closed_;
};

extern GpioRelay Relay;

#endif /* _RELAY_H_ */