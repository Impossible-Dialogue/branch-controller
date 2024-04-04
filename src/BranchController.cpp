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
#include <Ota.h>

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
    Heartbeat::loop();
    TcpServer::loop();
    LED::loop();
    Ota::loop();
}