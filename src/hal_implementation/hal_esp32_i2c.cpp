#include <string.h>
#include <stdio.h>
#include <Arduino.h>
#include <Wire.h>
extern "C"
{
#include <cryptoauthlib.h>
}
// #define I2C1_SDA_PIN 16
// #define I2C1_SCL_PIN 4
#define I2C1_SDA_PIN 21
#define I2C1_SCL_PIN 22

#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#endif

#define MAX_I2C_BUSES 2 // ESP32 has 2 I2C bus
/**
 * @brief Minimum size, in bytes, of the internal private structure used to describe
 * I2C commands link.
 */
#define I2C_INTERNAL_STRUCT_SIZE (24)

typedef struct atcaI2Cmaster
{
    int id;
    // i2c_config_t conf;
    int ref_ct;
} ATCAI2CMaster_t;

ATCAI2CMaster_t i2c_hal_data[MAX_I2C_BUSES];

TwoWire I2C_one = TwoWire(0);
TwoWire I2C_two = TwoWire(1);

const char *TAG = "HAL_I2C";

/** \brief method to change the bus speec of I2C
 * \param[in] iface  interface on which to change bus speed
 * \param[in] speed  baud rate (typically 100000 or 400000)
 */
ATCA_STATUS hal_i2c_change_baud(ATCAIface iface, uint32_t speed)
{
    Serial.print("Clock speed is: ");
    Serial.println(speed);

    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus = cfg->atcai2c.bus;

    if (i2c_hal_data[bus].id == 1)
    {
        I2C_one.setClock(speed);
    }
    else if (i2c_hal_data[bus].id == 0)
    {
        I2C_two.setClock(speed);
    }

    // Wire.setClock(speed);
    return ATCA_SUCCESS;
}

/** \brief
    - this HAL implementation assumes you've included the START Twi libraries in your project, otherwise,
    the HAL layer will not compile because the START TWI drivers are a dependency *
 */

/** \brief hal_i2c_init manages requests to initialize a physical interface.  it manages use counts so when an interface
 * has released the physical layer, it will disable the interface for some other use.
 * You can have multiple ATCAIFace instances using the same bus, and you can have multiple ATCAIFace instances on
 * multiple i2c buses, so hal_i2c_init manages these things and ATCAIFace is abstracted from the physical details.
 */

/** \brief initialize an I2C interface using given config
 * \param[in] hal - opaque ptr to HAL data
 * \param[in] cfg - interface configuration
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_init(ATCAIface iface, ATCAIfaceCfg *cfg)
{
    int bus = cfg->atcai2c.bus;
    i2c_hal_data[bus].id = bus;
    i2c_hal_data[bus].ref_ct = 1;

    bool isBegin = -1;

    if (i2c_hal_data[bus].id == 1)
    {
        isBegin = I2C_one.begin(I2C1_SDA_PIN, I2C1_SCL_PIN, cfg->atcai2c.baud);
    }
    else if (i2c_hal_data[bus].id == 0)
    {
        isBegin = I2C_two.begin(I2C1_SDA_PIN, I2C1_SCL_PIN, cfg->atcai2c.baud);
    }

    // bool isBegin = Wire.begin(I2C1_SDA_PIN, I2C1_SCL_PIN);
    if (isBegin == 1)
    {
        Serial.println("I2C bus initiated successfully.");
    }
    else if (isBegin == 0 || isBegin == -1)
    {
        Serial.println("ERROR: Not able to initialize i2C communications.");
        return ATCA_WAKE_FAILED;
    }

    iface->hal_data = &i2c_hal_data[bus];
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C post init
 * \param[in] iface  instance
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus = cfg->atcai2c.bus;
    String payload = "";
    for (int i = 0; i < txlength; i++)
    {
        if (txdata[i] <= 0x0f)
            payload += "0";
        payload += String(txdata[i], HEX);
    }
    int address = cfg->atcai2c.slave_address;
    Serial.println(
        "I2C send " +
        String(txlength) +
        " bytes to addr 0x" +
        String(address, HEX) + ": " + payload);

    if (i2c_hal_data[bus].id == 1)
    {
        I2C_one.beginTransmission(address);
        I2C_one.write(txdata, txlength);
        I2C_one.endTransmission();
    }
    else if (i2c_hal_data[bus].id == 0)
    {
        I2C_two.beginTransmission(address);
        I2C_two.write(txdata, txlength);
        I2C_two.endTransmission();
    }
    // Wire.beginTransmission(address);
    // Wire.write(txdata, txlength);
    // Wire.endTransmission();
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t address, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus = cfg->atcai2c.bus;
    int wantRead = *rxlength;
    // Wire.requestFrom((int)cfg->atcai2c.slave_address, wantRead);
    // int i = 0;
    // while (Wire.available() && i < *rxlength)
    // {
    //     rxdata[i++] = Wire.read();
    // }
    // *rxlength = i;

    if (i2c_hal_data[bus].id == 1)
    {
        I2C_one.requestFrom((int)cfg->atcai2c.slave_address, wantRead);
        int i = 0;
        while (I2C_one.available() && i < *rxlength)
        {
            rxdata[i++] = I2C_one.read();
        }
        *rxlength = i;
    }
    else if (i2c_hal_data[bus].id == 0)
    {
        I2C_two.requestFrom((int)cfg->atcai2c.slave_address, wantRead);
        int i = 0;
        while (I2C_two.available() && i < *rxlength)
        {
            rxdata[i++] = I2C_two.read();
        }
        *rxlength = i;
    }

    Serial.println(
        "I2C received " +
        String((int)*rxlength) +
        " bytes from addr 0x" +
        String(cfg->atcai2c.slave_address, HEX));
    if (*rxlength == 0)
        return ATCA_COMM_FAIL;
    return ATCA_SUCCESS;
}

/** \brief manages reference count on given bus and releases resource if no more refences exist
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_release(void *hal_data)
{
    ATCAI2CMaster_t *hal = (ATCAI2CMaster_t *)hal_data;
    if (hal && --(hal->ref_ct) <= 0)
    {
        I2C_one.end();
        I2C_two.end();
        // Wire.end();
    }
    return ATCA_SUCCESS;
}

/** \brief Perform control operations for the kit protocol
 * \param[in]     iface          Interface to interact with.
 * \param[in]     option         Control parameter identifier
 * \param[in]     param          Optional pointer to parameter value
 * \param[in]     paramlen       Length of the parameter
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_control(ATCAIface iface, uint8_t option, void *param, size_t paramlen)
{
    (void)param;
    (void)paramlen;

    if (iface && iface->mIfaceCFG)
    {
        if (ATCA_HAL_CHANGE_BAUD == option)
        {
            return hal_i2c_change_baud(iface, *(uint32_t *)param);
        }
        else
        {
            return ATCA_UNIMPLEMENTED;
        }
    }
    return ATCA_BAD_PARAM;
}
