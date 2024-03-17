#pragma once
#include <Arduino.h>
#include <QNEthernet.h>

//
// implements a web server used for 
// advanced configuration options
//


namespace WebServer {

    void setup();
    void loop();
    void read_available();
    void process_get(char* szGet);
    void output_html();

}
