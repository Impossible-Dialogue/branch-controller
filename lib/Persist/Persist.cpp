#include <Persist.h>
#include <Logger.h>
#include <EEPROM.h>

namespace Persist
{

    persistence_t data;

    void setup()
    {

        // set some default values

        memset(&data, 0, sizeof(data));
        data.cb = sizeof(data);
        data.rgbSolidColor = BLACK;
        data.pattern = (uint8_t)1; // LED::patternTest
        data.brightness = BRIGHTNESS;
        data.max_power = 1000000; // mw
        data.first_color = 'r';
        data.static_ip = false;
        data.ip_addr[0] = 192;
        data.ip_addr[1] = 168;
        data.ip_addr[2] = 1;
        data.ip_addr[3] = 128;
        data.center_orientation = 0;

        Logger.printf("Initializing Persisted Data\n");

        byte bSig[2];
        bSig[0] = EEPROM.read(0);
        bSig[1] = EEPROM.read(1);

        Logger.printf("Signature read - %d %d\n", bSig[0], bSig[1]);
        if (bSig[0] == 'b' && bSig[1] == 'C')
        {
            // read EEPROM now - start with cb!
            uint16_t cbOnDisk = 0;
            uint8_t *pb = (uint8_t *)&cbOnDisk;
            pb[0] = EEPROM.read(2);
            pb[1] = EEPROM.read(3);

            Logger.printf("Data on EEPROM is %d bytes\n", cbOnDisk);

            if (cbOnDisk != sizeof(data))
            {
                Logger.printf("Data size on EEPROM doesn't match the code (%d!=%d). Reverting to default values.\n", cbOnDisk, sizeof(data));
                write();
            }
            else
            {
                // Don't read more than sizeof(data) or cbOnDisk
                uint8_t *pbData = (uint8_t *)&data;
                uint16_t cbToRead = min(cbOnDisk, sizeof(data)) - 2; // -2 because we're not reading cb again
                Logger.printf("\n");
                Logger.printf("\n");
                for (uint16_t i = 0; i < cbToRead; i++)
                {
                    pbData[i + 2] = EEPROM.read(i + 4); // skip over signature and cb
                    // Logger.printf("%x ", pbData[i+2]);
                }
                Logger.printf("\n");
                Logger.printf("\n");
            }
        }
        else
        {
            Logger.printf("Signature not found - not reading from EEPROM\n");
            write();
        }

        Logger.printf("cb: %d color: %x pattern: %d  brightness: %d\n"
                      "       max_power: %d  first_color: %c \n"
                      "       static ip: %d  ip addr: %d.%d.%d.%d\n",
                      data.cb,
                      data.rgbSolidColor,
                      data.pattern,
                      data.brightness,
                      data.max_power,
                      data.first_color,
                      data.static_ip,
                      data.ip_addr[0],
                      data.ip_addr[1],
                      data.ip_addr[2],
                      data.ip_addr[3]);
    }

    void write()
    {

        Logger.printf("Persist::Write with %d bytes\n", data.cb);

        for (uint16_t i = 0; i < data.cb; i++)
        {
            uint8_t *pb = (uint8_t *)&data;
            EEPROM.write(i + 2, pb[i]);
        }
        // if we haven't crashed yet, write the signature marks
        EEPROM.write(0, 'b');
        EEPROM.write(1, 'C');

        Logger.printf("Persist::Write done\n");
    }

}