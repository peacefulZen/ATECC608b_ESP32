#include <Arduino.h>
extern "C"
{
#include "cryptoauthlib.h"
}
#include "Configuration.h"

uint8_t KEY_SLOT = (uint8_t)9;

ATCAIfaceCfg cfg;
ATCA_STATUS status;

// AES KEY (len 32)
uint8_t key[32] = "SznNBRhLA3r3qrJe8RxEdk4KxHwya3j";

// Exemple of configuration for ATECC608A. Look at page 8 on the datasheet for ATECC608A-TFLXTLS
static uint8_t configuration[112] =
    {
        // The first 16 bytes can't be modified
        0x60,       // I2C address<br> *Byte 16*
        0x00,       // Reserved, must be zero *Byte 17*
        0x00,       // CountMatch *Byte 18*
        0x00,       // ChipMode,0x00 means the device in in high speed *byte19*
                    ///////////////////////////////////////////////////////////////////////////////////////////////
                    //*bytes 20 through 51* The data zone
        0x83, 0x20, //  Slot Config Slot 1                     ***This where I think the issue is what this code
        0x85, 0x20, //  Slot Config Slot 2                     After the "lock configuration" prompt goes away,
        0x8F, 0x20, //  Slot Config Slot 3                      the chip is no longer usable*****
        0xC4, 0x8F, //  Slot Config Slot 4
        0x8F, 0x8F, //  Slot Config Slot 5
        0x8F, 0x8F, //  Slot Config Slot 6
        0x9F, 0x8F, //  Slot Config Slot 7
        0x0F, 0x0F, //  Slot Config Slot 8
        0x8F, 0x0F, //  Slot Config Slot 9
        0x8F, 0x0F, //  Slot Config Slot 10
        0x8F, 0x0F, //  Slot Config Slot 11
        0x8F, 0x0F, //  Slot Config Slot 12
        0x8F, 0x0F, //  Slot Config Slot 13
        0x00, 0x00, //  Slot Config Slot 14
        0x00, 0x00, //  Slot Config Slot 15
        0xAF, 0x8F, //  Slot Config Slot 16
                    ////////////////////////////////////////////////////////////////////
                    //*Bytes 52 though 59
        0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
        //*Bytes 60 through 67
        0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
        0x00,       // UseLock *Byte 68*
        0x00,       // VolatileKeyPermission *Byte 69*
        0x00, 0x00, // SecureBoot *Byte 70 and 71*
        0x00,       // KdflvLoc *Byte 72*
        0x00, 0x00, // KdflvStr *Byte 73 and 74*
                    //  reserved, must be zero *Bytes 75 through 83*
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,                   // UserExtra *Byte 84*
        0x00,                   // UserExtraAdd *Byte 85*
        0x00,                   // LockValue *Byte 86*
        0x00,                   // LockConfig *Byte 87*
        0xFF, 0xFF,             // SlotLocked *Byte 88 and 89*
        0x00, 0x00,             // ChipOptions *Byte 90 and 91*
        0x00, 0x00, 0x00, 0x00, // X509format *Byte 92 through 95

        // KeyConfig *Byte 96 through 127*
        0x33, 0x00, //  Key Config Slot 1
        0x33, 0x00, //  Key Config Slot 2
        0x33, 0x00, //  Key Config Slot 3
        0x1C, 0x00, //  Key Config Slot 4
        0x1C, 0x00, //  Key Config Slot 5
        0x1C, 0x00, //  Key Config Slot 6
        0x1C, 0x00, //  Key Config Slot 7
        0x3C, 0x00, //  Key Config Slot 8
        0x1A, 0x00, //  Key Config Slot 9
        0x3A, 0x00, //  Key Config Slot 10
        0x1A, 0x00, //  Key Config Slot 11
        0x3A, 0x00, //  Key Config Slot 12
        0x3A, 0x00, //  Key Config Slot 13
        0x3C, 0x00, //  Key Config Slot 14
        0x3C, 0x00, //  Key Config Slot 15
        0x1C, 0x00  //  Key Config Slot 16
};
bool menu(String message)
{
  Serial.println(message);
  while (!Serial.available())
  {
    ;
  }
  String answer = Serial.readString();
  answer.trim();
  //   return (answer == "yes") ? true : menu(message);
  // }
  Serial.print("Answer is: ");
  Serial.println(answer);
  if (answer == "yes")
  {
    Serial.println("Answer == yes. Returning TRUE");
    Serial.println(answer);
    return true;
  }
  else
  {
    Serial.println("Answer NOT EQUAL to yes. RETURNING FALSE");
    Serial.println(answer);
    return false;
  }
}

void setup()
{
  Serial.begin(74880);

  // Init the constuctor for the library
  cfg.iface_type = ATCA_I2C_IFACE;  // Type of communication -> I2C mode
  cfg.devtype = ATECC608A;          // Type of chip
  cfg.atcai2c.slave_address = 0x60; // I2C address of Adafruit device
  // cfg.atcai2c.slave_address = 0x30;
  cfg.atcai2c.bus = 0;
  cfg.atcai2c.baud = 400000;
  // cfg.atcai2c.baud = 100000;
  cfg.wake_delay = 1500; // Delay of wake up (1500 ms)
  cfg.rx_retries = 20;
  Serial.println("\nFirmware start!");
  // by default logs to stderr, which is not visible printed by printf() in Arduino.
  // redirect to stdout instead to makeit  visible.
  atca_trace_config(stdout);
}

void loop()
{
  status = atcab_init(&cfg);
  if (status == ATCA_SUCCESS)
  {
    // Write the configuration
    if (!menu(F("Do you want to write the configuration ?")))
    {
      return;
    }
    status = write_configuration(&cfg, configuration, sizeof(configuration));
    if (status == ATCA_SUCCESS)
    {
      // Lock the configuration Zone
      Serial.println("Successfully wrote configuration file to disk.");
      if (!menu(F("Do you want to lock the Configuration zone (No more change can be done after that) ?")))
      {
        return;
      }
      status = lock_zone(&cfg, LOCK_ZONE_CONFIG);
      if (status == ATCA_SUCCESS)
      {
        Serial.println("Successfully locked config data.");
        // Write the key in the given slot
        if (!menu(F("Do you want to write the key in the given slot ?")))
        {
          return;
        }
        status = write_key_slot(&cfg, KEY_SLOT, key, sizeof(key));
        if (status == ATCA_SUCCESS)
        {
          Serial.println("Successfully locked key data.");
          // Lock the Data Zone
          if (!menu(F("Do you want to lock the Data zone (No more change can be done after that) ?")))
          {
            return;
          }
          status = lock_zone(&cfg, LOCK_ZONE_DATA);
          if (status == ATCA_SUCCESS)
          {
            Serial.println("Successfully locked key data.");
            ATCA_STATUS lock_config = check_lock_zone(&cfg, LOCK_ZONE_CONFIG);
            ATCA_STATUS lock_data = check_lock_zone(&cfg, LOCK_ZONE_DATA);
            if (lock_config == ATCA_SUCCESS && lock_data == ATCA_SUCCESS)
            {
              Serial.println("Your chip has been set, you can use it now !");
              return;
            }
            else
            {
              if (lock_config != ATCA_SUCCESS)
              {
                Serial.println("Your config zone has not been locked !");
              }
              if (lock_data != ATCA_SUCCESS)
              {
                Serial.println("Your data zone has not been locked !");
                return;
              }
            }
          }
          else
          {
            // See file atca_status.h for the code Error
            Serial.print(F("Impossible to Lock data | Code Error 0x"));
            Serial.println(status, HEX);
            Serial.println();
            return;
          }
        }
        else
        {
          // See file atca_status.h for the code Error
          Serial.print(F("Impossible to Write the key | Code Error 0x"));
          Serial.println(status, HEX);
          Serial.println();
          return;
        }
      }
      else
      {
        // See file atca_status.h for the code Error
        Serial.print(F("Impossible to Lock configuration | Code Error 0x"));
        Serial.println(status, HEX);
        Serial.println();
        return;
      }
    }
    else
    {
      // See file atca_status.h for the code Error
      Serial.print(F("Impossible to Write configuration | Code Error 0x"));
      Serial.println(status, HEX);
      Serial.println();
      return;
    }
  }
  else
  {
    // See file atca_status.h for the code Error
    Serial.print(F("Impossible to Init configuration | Code Error 0x"));
    Serial.println(status, HEX);
    Serial.println();
    return;
  }
}
