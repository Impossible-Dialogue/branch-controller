#include <MacAddress.h>
#include <Util.h>

namespace MacAddress {
 
    uint8_t mac[6];

    void read() {
        
        for(uint8_t by=0; by<2; by++) mac[by]=(HW_OCOTP_MAC1 >> ((1-by)*8)) & 0xFF;
        for(uint8_t by=0; by<4; by++) mac[by+2]=(HW_OCOTP_MAC0 >> ((3-by)*8)) & 0xFF;
    }

}