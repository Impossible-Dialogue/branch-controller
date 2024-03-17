// BranchController.cpp
//
// Main entry point - provides Arduino's setup() and loop() functions which
// tie together all the main functionality of the different modules.

#include <Arduino.h>
#include <BranchController.h>
#include <Heartbeat.h>
#include <Util.h>
#include <Display.h>
#include <TcpServer.h>
#include <LED.h>
#include <Persist.h>

void setup() {

    dbgprintf("Begin\n");

    Heartbeat::setup();
    Util::setup();
    Persist::setup();
    Display::setup();
    TcpServer::setup();
    LED::setup();

    dbgprintf("BranchController Setup Complete\n");

}



void loop() {

    static unsigned int lastIrCode = 0;

    Heartbeat::loop();
    TcpServer::loop();
    LED::loop();

    EVERY_N_SECONDS(30)
    {
        dbgprintf("Free memory: %u\n", Util::FreeMem() );
    }
}