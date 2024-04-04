#include <TcpServer.h>
#include <Util.h>
#include <SPI.h>
#include <BranchController.h>
#include <QNEthernet.h>
#include <Display.h>
#include <Persist.h>
#include <MacAddress.h>
#include <OpenPixelControl.h>
#include <WebServer.h>
#include <Ota.h>

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
        dbgprintf("MAC = %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        // Listen for link changes
        Ethernet.onLinkState([](bool state)
                             {
            if (state) {
            dbgprintf("[Ethernet] Link ON, %d Mbps, %s duplex\r\n",
                    Ethernet.linkSpeed(),
                    Ethernet.linkIsFullDuplex() ? "Full" : "Half");
            } else {
            dbgprintf("[Ethernet] Link OFF\r\n");
            }
            networkChanged(Ethernet.localIP() != INADDR_NONE, state); });

        // Listen for address changes
        Ethernet.onAddressChanged([]()
                                  {
            IPAddress ip = Ethernet.localIP();
            bool hasIP = (ip != INADDR_NONE);
            if (hasIP) {
            IPAddress subnet = Ethernet.subnetMask();
            IPAddress gw = Ethernet.gatewayIP();
            IPAddress dns = Ethernet.dnsServerIP();

            dbgprintf("[Ethernet] Address changed:\r\n"
                    "    Local IP = %u.%u.%u.%u\r\n"
                    "    Subnet   = %u.%u.%u.%u\r\n"
                    "    Gateway  = %u.%u.%u.%u\r\n"
                    "    DNS      = %u.%u.%u.%u\r\n",
                    ip[0], ip[1], ip[2], ip[3],
                    subnet[0], subnet[1], subnet[2], subnet[3],
                    gw[0], gw[1], gw[2], gw[3],
                    dns[0], dns[1], dns[2], dns[3]);
            } else {
            dbgprintf("[Ethernet] Address changed: No IP address\r\n");
            }

            // Tell interested parties the network state, for example, servers,
            // SNTP clients, and other sub-programs that need to know whethe
            // to stop/start/restart/etc
            networkChanged(hasIP, Ethernet.linkState()); });

        dbgprintf("Starting Ethernet with DHCP...\r\n");
        if (!Ethernet.begin())
        {
            dbgprintf("Failed to start Ethernet\r\n");
            return;
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
            dbgprintf("Starting OPC and web servers\n");
            OpenPixelControl::setup();
            WebServer::setup();
            Ota::setup();
            status = ready;
        }
    }

    void loop()
    {

        if (status != ready)
            return;

        OpenPixelControl::loop();
        WebServer::loop();

        Ethernet.maintain();
    }

    bool initialized()
    {
        return (status == ready);
    }

}
