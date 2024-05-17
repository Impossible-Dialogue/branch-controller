#include "Imu.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <Logger.h>

namespace Imu
{
    Adafruit_BNO055 bno = Adafruit_BNO055(55);
    uint32_t timestamp = 0;
    float orientation_x = 0.0;
    float orientation_y = 0.0;
    float orientation_z = 0.0;
    uint8_t calibration_sys = 0;
    uint8_t calibration_gyro = 0;
    uint8_t calibration_accel = 0;
    uint8_t calibration_mag = 0;

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

        timestamp = 0;
        orientation_x = 0.0;
        orientation_y = 0.0;
        orientation_z = 0.0;
        calibration_sys = 0;
        calibration_gyro = 0;
        calibration_accel = 0;
        calibration_mag = 0;
    }

    void loop()
    {
        /* Get a Euler angle sample for orientation */
        imu::Vector<3> orientation;
        bool success = bno.getVector(Adafruit_BNO055::VECTOR_EULER, &orientation);
        if (success)
        {
            timestamp = millis();
            orientation_x = orientation.x();
            orientation_y = orientation.y();
            orientation_z = orientation.z();
        }
        bno.getCalibration(&calibration_sys, &calibration_gyro, &calibration_accel, &calibration_mag);
    }

}