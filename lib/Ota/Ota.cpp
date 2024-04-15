#include "Ota.h"

#if !(defined(CORE_TEENSY) && defined(__IMXRT1062__) && defined(ARDUINO_TEENSY41))
#error Only Teensy 4.1 supported
#endif

#include <AsyncWebServer_Teensy41.h>
#include <QNEthernet.h>
#include <Util.h>
#include <Logger.h>
#include <TeensyOtaUpdater.h>

using namespace qindesign::network;

namespace Ota
{

  TeensyOtaUpdater *tOtaUpdater;
  AsyncWebServer *webServer;
  bool updateAvailable;

  void handleNotFound(AsyncWebServerRequest *Request)
  {
    String message = "File Not Found\n\n";
    uint8_t i;

    message += "URI: ";
    message += Request->url();
    message += "\nMethod: ";
    message += (Request->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += Request->args();
    message += "\n";

    for (i = 0; i < Request->args(); i++)
    {
      message += " " + Request->argName(i) + ": " + Request->arg(i) + "\n";
    }

    Request->send(404, "text/plain", message);
  }

  void TOA_Callack()
  {
    Serial.println("An update is available");
    updateAvailable = true;
  }

  ////////////////////////// setup() ////////////////////////////////
  void setup()
  {
    webServer = new AsyncWebServer(8000);
    webServer->onNotFound(handleNotFound);
    tOtaUpdater = new TeensyOtaUpdater(webServer, "/");

    // Register a callback to be notified of when an update is available. If
    // a callback is not registered, the update will be applied automatically
    // when one is available
    updateAvailable = false;
    tOtaUpdater->registerCallback(TOA_Callack);

    webServer->begin();
    Logger.println("OTA ready");
  }

  ////////////////////////// loop() ////////////////////////////////
  void loop()
  {
    if (updateAvailable)
    {
      // Notify other layers (to display a status that about to reboot or smth)
      Serial.println("Applying udpate");

      // This function does not return
      tOtaUpdater->applyUpdate();
    }
  }

}