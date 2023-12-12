/* Auto-generated config file atca_config.h */
#ifndef ATCA_CONFIG_H
#define ATCA_CONFIG_H

/* Included HALS */
//#define ATCA_HAL_KIT_UART
//#define ATCA_HAL_KIT_HID
#define ATCA_HAL_I2C
//#define ATCA_HAL_SPI
//#define ATCA_HAL_KIT_BRIDGE
//#define ATCA_HAL_CUSTOM
//#define ATCA_HAL_SWI_UART
//#define ATCA_HAL_1WIRE

/* Included device support */
#define ATCA_ATSHA204A_SUPPORT
#define ATCA_ATSHA206A_SUPPORT
#define ATCA_ATECC108A_SUPPORT
#define ATCA_ATECC508A_SUPPORT
#define ATCA_ATECC608_SUPPORT
//#define ATCA_ECC204_SUPPORT
//#define ATCA_TA010_SUPPORT
//#define ATCA_SHA104_SUPPORT
//#define ATCA_SHA105_SUPPORT

/* Linked device support library */
//#define ATCA_TA_SUPPORT

/** Device Override - Library Assumes ATECC608B support in checks */
//#define ATCA_ATECC608A_SUPPORT

/** Define to enable compatibility with legacy HALs
   (HALs with embedded device logic)*/
#//define ATCA_HAL_LEGACY_API

/** To use dynamically registered HALs without any of the provided
implementations its necessary to specify a value here - using this
in addition to specifying a provide hal may result in compilation
problems - it will need to be the same as the number of the hal options
selected plus however additional slots one would like */
//#define ATCA_MAX_HAL_CACHE @ATCA_MAX_HAL_CACHE@

/** Define if cryptoauthlib is to use the maximum execution time method */
//#define ATCA_NO_POLL

/* \brief How long to wait after an initial wake failure for the POST to
 *         complete.
 * If Power-on self test (POST) is enabled, the self test will run on waking
 * from sleep or during power-on, which delays the wake reply.
 */
#ifndef ATCA_POST_DELAY_MSEC
#define ATCA_POST_DELAY_MSEC 25
#endif

/***************** Diagnostic & Test Configuration Section *****************/

/** Enable debug messages */
#define ATCA_PRINTF

/** Enable preprocessor warning messages */
#define ATCA_PREPROCESSOR_WARNING

/** Enable jwt functionality */
#define ATCA_JWT_EN

/** Enable to build in test hooks */
//#define ATCA_TESTS_ENABLED

/******************** Features Configuration Section ***********************/

/** Define certificate templates to be supported. */
//#define ATCA_TNGTLS_SUPPORT
//#define ATCA_TNGLORA_SUPPORT
//#define ATCA_TFLEX_SUPPORT
//#define ATCA_TNG_LEGACY_SUPPORT

/** Define Software Crypto Library to Use - if none are defined use the
    cryptoauthlib version where applicable */
//#define ATCA_MBEDTLS
//#define ATCA_OPENSSL
//#define ATCA_WOLFSSL

#ifdef ATCA_WOLFSSL
#define WOLFSSL_USER_SETTINGS
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#endif
#endif

/** Additional Runtime Configuration */
//#define ATCA_LIBRARY_CONF  "@ATCA_LIBRARY_CONF@"

/** Define to build atcab_ functions rather that defining them as macros */
//#define ATCA_USE_ATCAB_FUNCTIONS

/** Define to enable older API forms that have been replaced */
#define ATCA_ENABLE_DEPRECATED

/** Enable Strict ISO/C99 compliance */
//#define ATCA_STRICT_C99

/******************** Device Configuration Section *************************/

/** Enable the delete command */
//#define CALIB_DELETE_EN

/******************** Packet Size Configuration Section *************************/

/** Provide Maximum packet size for the command to be sent and received */
#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE         1072U
#endif

//#define MULTIPART_BUF_EN

/******************** Platform Configuration Section ***********************/

/** Define if the library is not to use malloc/free */
//#define ATCA_NO_HEAP

/** Define platform provided functions */
//#define ATCA_PLATFORM_MALLOC      @ATCA_PLATFORM_MALLOC@
//#define ATCA_PLATFORM_FREE        @ATCA_PLATFORM_FREE@
//#define ATCA_PLATFORM_STRCASESTR  @ATCA_PLATFORM_STRCASESTR@
//#define ATCA_PLATFORM_MEMSET_S    @ATCA_PLATFORM_MEMSET_S@

#define atca_delay_ms   hal_delay_ms
#define atca_delay_us   hal_delay_us

#include <stdio.h>

#endif // ATCA_CONFIG_H
