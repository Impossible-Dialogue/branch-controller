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
    Mqtt::RateLimitedMqttPublisher<uint32_t> timestamp_publisher("orientation/timestamp");
    Mqtt::RateLimitedMqttPublisher<float> orientation_x_publisher("orientation/x");
    Mqtt::RateLimitedMqttPublisher<float> orientation_y_publisher("orientation/x");
    Mqtt::RateLimitedMqttPublisher<float> orientation_z_publisher("orientation/x");
    Mqtt::RateLimitedMqttPublisher<uint8_t> calibration_sys_publisher("calibration/sys");
    Mqtt::RateLimitedMqttPublisher<uint8_t> calibration_gyro_publisher("calibration/gyro");
    Mqtt::RateLimitedMqttPublisher<uint8_t> calibration_accel_publisher("calibration/accel");
    Mqtt::RateLimitedMqttPublisher<uint8_t> calibration_mag_publisher("calibration/mag");

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
        if (success)
        {
            timestamp_publisher.maybePublish(millis());
            orientation_x_publisher.maybePublish(orientation.x());
            orientation_z_publisher.maybePublish(orientation.y());
            orientation_z_publisher.maybePublish(orientation.z());
        }

        uint8_t sys, gyro, accel, mag = 0;
        bno.getCalibration(&sys, &gyro, &accel, &mag);
        calibration_sys_publisher.maybePublish(sys);
        calibration_gyro_publisher.maybePublish(gyro);
        calibration_accel_publisher.maybePublish(accel);
        calibration_mag_publisher.maybePublish(mag);
    }

}