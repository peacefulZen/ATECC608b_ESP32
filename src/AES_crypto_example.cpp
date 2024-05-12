#include <Arduino.h>
extern "C"
{
#include "cryptoauthlib.h"
}
#include "aes_cbc.h"
#include <EEPROM.h>
// Key Slot number
uint8_t KEY_SLOT = (uint8_t)9;

ATCAIfaceCfg cfg;
ATCA_STATUS status;

void setup()
{
    Serial.begin(74880);

    // Init the constuctor for the library
    cfg.iface_type = ATCA_I2C_IFACE;  // Type of communication -> I2C mode
    cfg.devtype = ATECC608B;          // Type of chip
    cfg.atcai2c.slave_address = 0X60; // I2C address of Adafruit device
    // cfg.atcai2c.slave_address = 0x30;
    cfg.atcai2c.bus = 1;
    // cfg.atcai2c.baud = 400000;
    cfg.atcai2c.baud = 100000;
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
    if (status != ATCA_SUCCESS)
    {
        Serial.println(F("atcab_init() failed : Code -> 0x"));
        Serial.println(status, HEX);
        return;
    }
    uint addr = 0; // Address for EEPROM
    uint8_t plaintext[32] = "EB4655C2-372F-4133-82B9-AC3DECA";
    uint8_t iv[IV_LENGTH_CBC];
    uint8_t cypherdata[sizeof(plaintext)];
    uint8_t decryptdata[sizeof(plaintext)];
    Serial.println("Beginning of the encryption !");
    status = aes_cbc_encrypt(&cfg, plaintext, sizeof(plaintext), iv, cypherdata, KEY_SLOT);
    if (status == ATCA_SUCCESS)
    {
        Serial.println("encrypted");
        EEPROM.begin(512); // commit 512 bytes of ESP8266 flash (for "EEPROM" emulation)
        EEPROM.put(addr, cypherdata);
        EEPROM.commit();
        Serial.println("Beginning of the decryption !");
        uint8_t FP_cypherdata[sizeof(plaintext)]; // Calling it something else to make sure we are in fact pulling it from EEPROM
        EEPROM.get(addr, FP_cypherdata);
        status = aes_cbc_decrypt(&cfg, FP_cypherdata, sizeof(FP_cypherdata), iv, decryptdata, KEY_SLOT);
        if (status == ATCA_SUCCESS)
        {
            Serial.print("Decrypted text is : ");
            for (size_t i = 0; i < sizeof(decryptdata); i++)
            {
                Serial.print((char)decryptdata[i]);
            }
            Serial.println("");
        }
        else
        {
            // See file atca_status.h for the code Error
            Serial.print(F("Impossible do the decryption | Code Error 0x"));
            Serial.println(status, HEX);
            return;
        }
    }
    else
    {
        // See file atca_status.h for the code Error
        Serial.print(F("Impossible do the encryption | Code Error 0x"));
        Serial.println(status, HEX);
        return;
    }
    delay(1000);
}