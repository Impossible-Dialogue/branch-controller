#include "WebSocket.h"
#include <BranchController.h>
#include <Logger.h>
#include <Imu.h>

#if (defined(CORE_TEENSY) && defined(__IMXRT1062__) && defined(ARDUINO_TEENSY41))
// For Teensy 4.1
#define BOARD_TYPE "TEENSY 4.1"
// Use true for NativeEthernet Library, false if using other Ethernet libraries
#define USE_NATIVE_ETHERNET false
#define USE_QN_ETHERNET true
// #define USE_NATIVE_ETHERNET     true
// #define USE_QN_ETHERNET         false
#else
#error Only Teensy 4.1 supported
#endif

#ifndef BOARD_NAME
#define BOARD_NAME BOARD_TYPE
#endif

#define WEBSOCKETS_USE_ETHERNET true

#include <WebSockets2_Generic.h>

using namespace websockets2_generic;

namespace WebSocket
{

    // Define how many clients we accept simultaneously.
    const byte maxClients = 16;

    WebsocketsClient clients[maxClients];
    WebsocketsServer server;

    void setup()
    {
        // Start websockets server.
        server.listen(WEBSOCKET_PORT);

        if (server.available())
        {
            Logger.print("Server available at ws://");
            Logger.print(Ethernet.localIP());
            Logger.printf(":%d", WEBSOCKET_PORT);
            Logger.println();
        }
        else
        {
            Logger.println("Server not available!");
        }
    }

    void handleMessage(WebsocketsClient &client, WebsocketsMessage message)
    {
        auto data = message.data();
        if (data == "imu/orientation_x")
        {
            client.send(String(Imu::orientation_x).c_str());
        }
        else if (data == "imu/orientation_y")
        {
            client.send(String(Imu::orientation_y).c_str());
        }
        else if (data == "imu/orientation_z")
        {
            client.send(String(Imu::orientation_z).c_str());
        }
        else if (data == "imu/calibration_sys")
        {
            client.send(String(Imu::calibration_sys).c_str());
        }
        else if (data == "imu/calibration_gyro")
        {
            client.send(String(Imu::calibration_gyro).c_str());
        }
        else if (data == "imu/calibration_accel")
        {
            client.send(String(Imu::calibration_accel).c_str());
        }
        else if (data == "imu/calibration_mag")
        {
            client.send(String(Imu::calibration_mag).c_str());
        }
        else if (data == "imu/timestamp")
        {
            client.send(String(Imu::timestamp).c_str());
        }
    }

    void handleEvent(WebsocketsClient &client, WebsocketsEvent event, String data)
    {
        if (event == WebsocketsEvent::ConnectionClosed)
        {
            Serial.println("Connection closed");
        }
    }

    int8_t getFreeClientIndex()
    {
        // If a client in our list is not available, it's connection is closed and we
        // can use it for a new client.
        for (byte i = 0; i < maxClients; i++)
        {
            if (!clients[i].available())
                return i;
        }

        return -1;
    }

    void listenForClients()
    {
        if (server.poll())
        {
            int8_t freeIndex = getFreeClientIndex();

            if (freeIndex >= 0)
            {
                WebsocketsClient newClient = server.accept();
                if (newClient.available())
                {
                    Logger.printf("Accepted new websockets client at index %d\n", freeIndex);
                    newClient.onMessage(handleMessage);
                    newClient.onEvent(handleEvent);
                    clients[freeIndex] = newClient;
                }
            }
            else
            {
                Logger.printf("Exceeded the number of clients that are able to connect (%d)./n", maxClients);
            }
        }
    }

    void pollClients()
    {
        for (byte i = 0; i < maxClients; i++)
        {
            clients[i].poll();
        }
    }

    void loop()
    {
        listenForClients();
        pollClients();
    }

}