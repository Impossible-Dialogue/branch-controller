#include "Imu.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <Logger.h>
#include <Mqtt.h>

namespace Imu
{
    Adafruit_BNO055 bno = Adafruit_BNO055(55);

    void setup()
    {
        /* Initialise the sensor */
        if (!bno.begin())
        {
            /* There was a problem detecting the BNO055 ... check your connections */
            Logger.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
            return;
        }

        delay(1000);

        bno.setExtCrystalUse(true);
    }

    void loop()
    {
        /* Get a Euler angle sample for orientation */
        imu::Vector<3> orientation;
        bool success = bno.getVector(Adafruit_BNO055::VECTOR_EULER, &orientation);
        if (success) {
            Mqtt::maybePublish("orientation/timestamp", millis());
            Mqtt::maybePublish("orientation/x", orientation.x());
            Mqtt::maybePublish("orientation/y", orientation.y());
            Mqtt::maybePublish("orientation/z", orientation.z());
        }
        
        uint8_t sys, gyro, accel, mag = 0;
        bno.getCalibration(&sys, &gyro, &accel, &mag);
        Mqtt::maybePublish("calibration/sys", sys);
        Mqtt::maybePublish("calibration/gyro", gyro);
        Mqtt::maybePublish("calibration/accel", accel);
        Mqtt::maybePublish("calibration/mag", mag);
    }

}