#include "Imu.h"


#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

namespace Imu
{
    Adafruit_BNO055 bno = Adafruit_BNO055(55);

    void setup() {
        /* Initialise the sensor */
        if(!bno.begin())
        {
            /* There was a problem detecting the BNO055 ... check your connections */
            Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
            while(1);
        }
        
        delay(1000);
            
        bno.setExtCrystalUse(true);

    }

    void loop() {
         /* Get a new sensor event */ 
        sensors_event_t event; 
        bno.getEvent(&event);

        /* The WebSerial 3D Model Viewer expects data as heading, pitch, roll */
        Serial.print(F("Orientation: "));
        Serial.print(360 - (float)event.orientation.x);
        Serial.print(F(", "));
        Serial.print((float)event.orientation.y);
        Serial.print(F(", "));
        Serial.print((float)event.orientation.z);
        Serial.println(F(""));

        /* The WebSerial 3D Model Viewer also expects data as roll, pitch, heading */
        imu::Quaternion quat = bno.getQuat();
        
        Serial.print(F("Quaternion: "));
        Serial.print((float)quat.w(), 4);
        Serial.print(F(", "));
        Serial.print((float)quat.x(), 4);
        Serial.print(F(", "));
        Serial.print((float)quat.y(), 4);
        Serial.print(F(", "));
        Serial.print((float)quat.z(), 4);
        Serial.println(F(""));

        /* Also send calibration data for each sensor. */
        uint8_t sys, gyro, accel, mag = 0;
        bno.getCalibration(&sys, &gyro, &accel, &mag);
        Serial.print(F("Calibration: "));
        Serial.print(sys, DEC);
        Serial.print(F(", "));
        Serial.print(gyro, DEC);
        Serial.print(F(", "));
        Serial.print(accel, DEC);
        Serial.print(F(", "));
        Serial.print(mag, DEC);
        Serial.println(F(""));
    }

}