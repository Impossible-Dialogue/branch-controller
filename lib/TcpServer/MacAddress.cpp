#include <MacAddress.h>
#include <Util.h>
#include <Display.h>

namespace MacAddress {
 
    uint8_t mac[6];

    void read() {
        
        for(uint8_t by=0; by<2; by++) mac[by]=(HW_OCOTP_MAC1 >> ((1-by)*8)) & 0xFF;
        for(uint8_t by=0; by<4; by++) mac[by+2]=(HW_OCOTP_MAC0 >> ((3-by)*8)) & 0xFF;

        char rgch[22];
        sprintf(rgch, "MAC %02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        Display::status(1, rgch);

    }

}