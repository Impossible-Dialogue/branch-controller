#ifndef _IMU_H_
#define _IMU_H_

#include <Arduino.h>

//
// Implements an interface for Adafruit's BNO055 IMU
//

namespace Imu
{

    void setup();
    void loop();

    extern uint32_t timestamp;
    extern float orientation_x;
    extern float orientation_y;
    extern float orientation_z;
    extern float head_orientation;
    extern uint8_t calibration_sys;
    extern uint8_t calibration_gyro;
    extern uint8_t calibration_accel;
    extern uint8_t calibration_mag;
}

#endif /* _IMU_H_ */