#include <string.h>
#include <stdio.h>
#include <Arduino.h>
#include <Wire.h>
extern "C" {
#include <cryptoauthlib.h>
}
#define SDA_PIN 4 // Define the GPIO pin for SDA
#define SCL_PIN 5 // Define the GPIO pin for SCL

#define MAX_I2C_BUSES 1 // ESP8266 has 1 I2C bus

typedef struct atcaI2Cmaster
{
    int id;
    int ref_ct;
} ATCAI2CMaster_t;

ATCAI2CMaster_t i2c_hal_data[MAX_I2C_BUSES];

ATCA_STATUS hal_i2c_change_baud(ATCAIface iface, uint32_t speed)
{
    Wire.setClock(speed);
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_init(ATCAIface iface, ATCAIfaceCfg *cfg)
{
    int bus = 0; // ESP8266 has only one I2C bus
    i2c_hal_data[bus].id = bus;
    i2c_hal_data[bus].ref_ct = 1;

    Wire.begin(SDA_PIN, SCL_PIN);

    iface->hal_data = &i2c_hal_data[bus];

    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    // Not required on ESP8266
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    String payload = "";
    for(int i=0; i < txlength; i++) {
        if(txdata[i] <= 0x0f) 
            payload += "0";
        payload += String(txdata[i], HEX);
    }
    int address = cfg->atcai2c.slave_address;
    Serial.println(
        "I2C send " + 
        String(txlength) + 
        " bytes to addr 0x" + 
        String(address, HEX) + ": " + payload);
    Wire.beginTransmission(address);
    Wire.write(txdata, txlength);
    Wire.endTransmission();
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t address, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    Wire.requestFrom((int)cfg->atcai2c.slave_address, (int)*rxlength);

    int i = 0;
    while (Wire.available() && i < *rxlength)
    {
        rxdata[i++] = Wire.read();
    }

    *rxlength = i;
    Serial.println(
        "I2C received " + 
        String((int)*rxlength) + 
        " bytes from addr 0x" + 
        String(cfg->atcai2c.slave_address, HEX));

    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_release(void *hal_data)
{
    // Not required on ESP8266
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_control(ATCAIface iface, uint8_t option, void *param, size_t paramlen)
{
    // Not supported on ESP8266
    return ATCA_UNIMPLEMENTED;
}
