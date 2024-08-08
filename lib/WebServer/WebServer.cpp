#include <Arduino.h>
#include <WebServer.h>
#include <Persist.h>
#include <LED.h>
#include <Logger.h>
#include <Imu.h>
#include <Relay.h>

#include <QNEthernet.h>
using namespace qindesign::network;

#include <AsyncWebServer_Teensy41.hpp>

#define BUFFER_SIZE 1000

namespace WebServer
{
    AsyncWebServer server(80);

    void notFound(AsyncWebServerRequest *request)
    {
        request->send(404, "text/plain", "Not found");
    }

    void handleForm(AsyncWebServerRequest *request)
    {
        for (uint8_t i = 0; i < request->args(); i++)
        {
            if (request->argName(i) == "c")
                Persist::data.center_orientation = request->arg(i).toFloat();
            if (request->argName(i) == "s")
                Persist::data.static_ip = (request->arg(i).toInt() == 1);
            if (request->argName(i) == "i0")
                Persist::data.ip_addr[0] = request->arg(i).toInt();
            if (request->argName(i) == "i1")
                Persist::data.ip_addr[1] = request->arg(i).toInt();
            if (request->argName(i) == "i2")
                Persist::data.ip_addr[2] = request->arg(i).toInt();
            if (request->argName(i) == "i3")
                Persist::data.ip_addr[3] = request->arg(i).toInt();
        }
        Persist::write();
        LED::load_persistant_data();
    }

    void handleRoot(AsyncWebServerRequest *request)
    {

        char temp[BUFFER_SIZE];

        snprintf(temp, BUFFER_SIZE - 1,
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "<!DOCTYPE HTML>"
                 "<html>"
                 "<h1>Branch Controller</h1>"
                 "<p>built " __DATE__ " " __TIME__ "</p>"
                 "<hr>"
                 "<form method='post' action=/>"
                 "Center orientation: "
                 "<input name=c value='%f'> degrees"
                 "<br>"
                 "<input type=checkbox id=s name=s value=1 %s><label for=s>Static IP Address: </label> "
                 "<input name=i0 size=3 value=%d>."
                 "<input name=i1 size=3 value=%d>."
                 "<input name=i2 size=3 value=%d>."
                 "<input name=i3 size=3 value=%d>"
                 "<br / >"
                 "<input type=submit>"
                 "<br>"
                 "Test colors: "
                 "<a href=/r>Red</a> "
                 "<a href=/g>Green</a> "
                 "<a href=/b>Blue</a> "
                 "<a href=/w>White</a> "
                 "</form>"
                 "</html>",
                 Persist::data.center_orientation,
                 Persist::data.static_ip ? "checked" : "",
                 Persist::data.ip_addr[0],
                 Persist::data.ip_addr[1],
                 Persist::data.ip_addr[2],
                 Persist::data.ip_addr[3]);

        request->send(200, "text/html", temp);
    }

    void setup()
    {
        server.begin();
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { handleRoot(request); 
                        request->redirect("/"); });
        server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
                  {
                      handleForm(request);
                      // request->redirect("/");
                  });
        server.on("/r", HTTP_GET, [](AsyncWebServerRequest *request)
                  { LED::setSolidColor(RED); 
                        request->redirect("/"); });
        server.on("/g", HTTP_GET, [](AsyncWebServerRequest *request)
                  { LED::setSolidColor(GREEN); 
                        request->redirect("/"); });
        server.on("/b", HTTP_GET, [](AsyncWebServerRequest *request)
                  { LED::setSolidColor(BLUE); 
                        request->redirect("/"); });
        server.on("/w", HTTP_GET, [](AsyncWebServerRequest *request)
                  { LED::setSolidColor(WHITE); 
                        request->redirect("/"); });

        server.on("/head_orientation", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "text/plain", String(Imu::head_orientation)); });

        server.on("/relay_open", HTTP_POST, [](AsyncWebServerRequest *request)
                  { Relay.open(); });

        server.on("/relay_is_open", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "text/plain", String(Relay.is_open())); });

        server.on("/relay_close", HTTP_POST, [](AsyncWebServerRequest *request)
                  { Relay.close(); });

        server.on("/relay_is_closed", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "text/plain", String(Relay.is_closed())); });

        server.onNotFound(notFound);
        server.begin();
        Logger.println("Webserver ready");
    }

    void loop()
    {
    }
}
