#include <TcpServer.h>
#include <Util.h>
#include <SPI.h>
#include <BranchController.h>
#include <QNEthernet.h>
#include <Persist.h>
#include <MacAddress.h>
#include <OpenPixelControl.h>
#include <WebServer.h>
#include <Ota.h>
#include <Mqtt.h>
#include <Logger.h>
#include <WebSocket.h>

// Include Teensy41_AsyncTCP.h to link implementation of AsyncTCP
#include "Teensy41_AsyncTCP.h"

using namespace qindesign::network;

namespace TcpServer
{

    enum Status
    {
        uninitialized,
        noHardware,
        noCable,
        noDHCP,
        ready
    };
    Status status = uninitialized;

    const char *rgchRetry = "Press AUTO to connect";

    void setup()
    {

        uint8_t mac[6];
        Ethernet.macAddress(mac); // This is informative; it retrieves, not sets
        Logger.printf("MAC = %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        // Listen for link changes
        Ethernet.onLinkState([](bool state)
                             {
            if (state) {
            Logger.printf("[Ethernet] Link ON, %d Mbps, %s duplex\r\n",
                    Ethernet.linkSpeed(),
                    Ethernet.linkIsFullDuplex() ? "Full" : "Half");
            } else {
            Logger.println("[Ethernet] Link OFF\r\n");
            }
            networkChanged(Ethernet.localIP() != IPAddress((uint32_t)0), state); });

        // Listen for address changes
        Ethernet.onAddressChanged([]()
                                  {
            IPAddress ip = Ethernet.localIP();
            bool hasIP = (ip != IPAddress((uint32_t)0));
            if (hasIP) {
            IPAddress subnet = Ethernet.subnetMask();
            IPAddress gw = Ethernet.gatewayIP();
            IPAddress dns = Ethernet.dnsServerIP();

            Logger.printf("[Ethernet] Address changed:\r\n"
                    "    Local IP = %u.%u.%u.%u\r\n"
                    "    Subnet   = %u.%u.%u.%u\r\n"
                    "    Gateway  = %u.%u.%u.%u\r\n"
                    "    DNS      = %u.%u.%u.%u\r\n",
                    ip[0], ip[1], ip[2], ip[3],
                    subnet[0], subnet[1], subnet[2], subnet[3],
                    gw[0], gw[1], gw[2], gw[3],
                    dns[0], dns[1], dns[2], dns[3]);
            } else {
            Logger.println("[Ethernet] Address changed: No IP address");
            }

            // Tell interested parties the network state, for example, servers,
            // SNTP clients, and other sub-programs that need to know whethe
            // to stop/start/restart/etc
            networkChanged(hasIP, Ethernet.linkState()); });

        if (Persist::data.static_ip)
        {
            IPAddress ip(Persist::data.ip_addr[0],
                         Persist::data.ip_addr[1],
                         Persist::data.ip_addr[2],
                         Persist::data.ip_addr[3]);
            IPAddress mask(Persist::data.mask[0],
                           Persist::data.mask[1],
                           Persist::data.mask[2],
                           Persist::data.mask[3]);
            IPAddress gateway(Persist::data.gateway[0],
                              Persist::data.gateway[1],
                              Persist::data.gateway[2],
                              Persist::data.gateway[3]);
            Logger.println("Starting Ethernet with static IP address...");
            if (!Ethernet.begin(ip, mask, gateway))
            {
                Logger.println("Failed to start Ethernet");
                return;
            }
        }
        else
        {
            Logger.println("Starting Ethernet with DHCP...");
            if (!Ethernet.begin())
            {
                Logger.println("Failed to start Ethernet");
                return;
            }
        }
    }

    // The address or link has changed. For example, a DHCP address arrived.
    void networkChanged(bool hasIP, bool linkState)
    {
        if (!hasIP || !linkState)
        {
            return;
        }

        // Start the server and keep it up
        if (status != ready)
        {
            Logger.println("Starting OPC and web servers");
            OpenPixelControl::setup();
            WebServer::setup();
            // Ota::setup();
            // Mqtt::setup();
            WebSocket::setup();
            status = ready;
        }
    }

    void loop()
    {

        if (status != ready)
            return;

        OpenPixelControl::loop();
        WebServer::loop();
        // Ota::loop();
        // Mqtt::loop();
        WebSocket::loop();

        Ethernet.maintain();
    }

    bool initialized()
    {
        return (status == ready);
    }

}
