#include "Ota.h"

#if !(defined(CORE_TEENSY) && defined(__IMXRT1062__) && defined(ARDUINO_TEENSY41))
#error Only Teensy 4.1 supported
#endif

#include <AsyncWebServer_Teensy41.h>
#include <QNEthernet.h>
#include <Util.h>

// extern "C"
// {
#include "FXUtil.h"   // read_ascii_line(), hex file support
#include "FlashTxx.h" // TLC/T3x/T4x/TMM flash primitives
// }

using namespace qindesign::network;

namespace Ota
{

  AsyncWebServer server(8000);

  static char line[96]; // buffer for hex lines
  int line_index = 0;
  static char data[32] __attribute__((aligned(8))); // buffer for hex data
  hex_info_t hex = {
      // intel hex info struct
      data, 0, 0, 0,    //   data,addr,num,code
      0, 0xFFFFFFFF, 0, //   base,min,max,
      0, 0              //   eof,lines
  };
  bool ota_status = false; // 1=running
  bool ota_final = false;
  uint32_t buffer_addr, buffer_size;

  void setup()
  {
    Serial.printf("\nOTA through Ethernet for ");
    Serial.printf(BOARD_NAME);
    Serial.printf("\n");
    Serial.printf("FLASH_SIZE %d\n", int(FLASH_SIZE));
    Serial.printf("FLASH_RESERVE %d\n", int(FLASH_RESERVE));
    Serial.printf("FLASH_BASE_ADDR %08lX\n", FLASH_BASE_ADDR);
    Serial.printf("\n");

#ifdef DEBUG
    // buffer will begin at first sector ABOVE code and below FLASH_RESERVE
    // start at bottom of FLASH_RESERVE and work down until non-erased flash found
    uint32_t baddr;
    baddr = FLASH_BASE_ADDR + FLASH_SIZE - FLASH_RESERVE - 4;
    Serial.printf("buffer_addr: %08lX\n", baddr);
    Serial.printf("value: %08lX\n", *((uint32_t *)baddr));
    while (baddr > 0 && *((uint32_t *)baddr) == 0xFFFFFFFF)
      baddr -= 4;
    baddr += 4; // first address above code
    // Serial.printf("buffer_addr post code search: %08lX\n", baddr);

    // increase buffer_addr to next sector boundary (if not on a sector boundary)
    if ((baddr % FLASH_SECTOR_SIZE) > 0)
      baddr += FLASH_SECTOR_SIZE - (baddr % FLASH_SECTOR_SIZE);
    Serial.printf("buffer_addr post sector adj: %08lX\n", baddr);
    uint32_t bsize = FLASH_BASE_ADDR - baddr + FLASH_SIZE - FLASH_RESERVE;
    Serial.printf("buffer size: %d\n", bsize);
#endif

    server.onNotFound(handleNotFound);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String html = "<body><h1>OTA through Ethernet demo code for Teensy41</h1><br><h2>select and send your binary file:</h2><br><div><form method='POST' enctype='multipart/form-data' action='/'><input type='file' name='file'><button type='submit'>Send</button></form></div></body>";
    request->send(200, "text/html", html); });

    server.on("/", HTTP_POST, handleRequest, handleUpload);

    server.begin();
  }

  void loop()
  {
  }

  void handleRequest(AsyncWebServerRequest *request)
  {
    AsyncWebParameter *p = request->getParam(0);
    Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
    if (ota_final)
    {
      Serial.printf("\nhex file: %1d lines %1lu bytes (%08lX - %08lX)\n",
                hex.lines, hex.max - hex.min, hex.min, hex.max);

// check FSEC value in new code -- abort if incorrect
#if defined(KINETISK) || defined(KINETISL)
      uint32_t value = *(uint32_t *)(0x40C + buffer_addr);
      if (value != 0xfffff9de)
      {
        out->printf("new code contains correct FSEC value %08lX\n", value);
      }
      else
      {
        out->printf("abort - FSEC value %08lX should be FFFFF9DE\n", value);
        ota_final = false;
      }
#endif
    }

    if (ota_final)
    {
      // check FLASH_ID in new code - abort if not found
      if (check_flash_id(buffer_addr, hex.max - hex.min))
      {
        Serial.printf("new code contains correct target ID %s\n", FLASH_ID);
      }
      else
      {
        Serial.printf("abort - new code missing string %s\n", FLASH_ID);
        ota_final = false;
      }
    }

    AsyncWebServerResponse *response = request->beginResponse((!ota_final) ? 500 : 200, "text/plain", (!ota_final) ? "OTA Failed... Going for reboot" : "OTA Success! Going for reboot");
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);

    if (ota_final)
    {
      Serial.printf("calling flash_move() to load new firmware...\n");
      flash_move(FLASH_BASE_ADDR, buffer_addr, hex.max - hex.min);
      REBOOT;
    }
    else
    {
      Serial.printf("erase FLASH buffer / free RAM buffer...\n");
      firmware_buffer_free(buffer_addr, buffer_size);
      delay(15000);
      REBOOT;
    }
  }

  void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
  {
    if (!ota_status)
    {
      Serial.printf("Starting OTA...\n");
      if (firmware_buffer_init(&buffer_addr, &buffer_size) == 0)
      {
        Serial.printf("unable to create buffer\n");
      }
      else
      {
        Serial.printf("created buffer = %1luK %s (%08lX - %08lX)\n",
                  buffer_size / 1024, IN_FLASH(buffer_addr) ? "FLASH" : "RAM",
                  buffer_addr, buffer_addr + buffer_size);
        ota_status = true;
      }
    }
    if (ota_status)
    {
      if (len)
      {
        size_t i = 0;
        while (i < len)
        {
          if (data[i] == 0x0A || (line_index == sizeof(line) - 1))
          {                       // '\n'
            line[line_index] = 0; // null-terminate
            // Serial.printf( "%s\n", line );
            if (parse_hex_line((const char *)line, hex.data, &hex.addr, &hex.num, &hex.code) == 0)
            {
              Serial.printf("abort - bad hex line %s\n", line);
              return request->send(400, "text/plain", "abort - bad hex line");
            }
            else if (process_hex_record(&hex) != 0)
            { // error on bad hex code
              Serial.printf("abort - invalid hex code %d\n", hex.code);
              return request->send(400, "text/plain", "invalid hex code");
            }
            else if (hex.code == 0)
            { // if data record
              uint32_t addr = buffer_addr + hex.base + hex.addr - FLASH_BASE_ADDR;
              if (hex.max > (FLASH_BASE_ADDR + buffer_size))
              {
                Serial.printf("abort - max address %08lX too large\n", hex.max);
                return request->send(400, "text/plain", "abort - max address too large");
              }
              else if (!IN_FLASH(buffer_addr))
              {
                memcpy((void *)addr, (void *)hex.data, hex.num);
              }
              else if (IN_FLASH(buffer_addr))
              {
                int error = flash_write_block(addr, hex.data, hex.num);
                if (error)
                {
                  Serial.printf("abort - error %02X in flash_write_block()\n", error);
                  return request->send(400, "text/plain", "abort - error in flash_write_block()");
                }
              }
            }
            hex.lines++;
            line_index = 0;
          }
          else if (data[i] != 0x0D)
          { // '\r'
            line[line_index++] = data[i];
          }
          i++;
        }
      }
      if (final)
      { // if the final flag is set then this is the last frame of data
        Serial.printf("Transfer finished\n");
        ota_final = true;
      }
      else
      {
        return;
      }
    }
    else
    {
      return request->send(400, "text/plain", "OTA could not begin");
    }
  }

  void handleNotFound(AsyncWebServerRequest *request)
  {
    request->send(404, "text/plain", "Not found");
  }

}