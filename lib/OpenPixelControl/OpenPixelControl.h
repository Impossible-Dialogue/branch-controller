#pragma once
#include <Arduino.h>
#include <QNEthernet.h>

// implements a simple version of Open Pixel Control protocol
// See "doc/OpenPixelControl.html" for the spec
//
// Note: 
//
//      OpenPixelControl is not the greatest protocol, but
//      we will try to support it well enough for L.E.D. Lab 
//      (in FadeCandy mode).
//


namespace OpenPixelControl {

    void setup();
    void loop();
    void read_available();

}