/**
 * @file board_pins.h
 * @brief GPIO / bus mapping for PET_COLLAR_1 (final_3.pdf, V1.0)
 *
 * Verify nets on the schematic before production. UART naming follows ESP32
 * perspective (TX = ESP output, RX = ESP input).
 */
#pragma once

#include "driver/gpio.h"
#include "driver/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* I2C — OPT3001, LSM6DSOX, MAX30105 (U10)                                    */
/* -------------------------------------------------------------------------- */
#define BOARD_I2C_PORT          I2C_NUM_0
#define BOARD_I2C_SDA_GPIO      GPIO_NUM_21   /* net: I2C_SDA */
#define BOARD_I2C_SCL_GPIO      GPIO_NUM_22   /* net: I2C_CLK */
#define BOARD_I2C_FREQ_HZ       400000

#define BOARD_LIGHT_INT_GPIO    GPIO_NUM_25   /* net: LIGHT_INIT */
#define BOARD_MEMS_INT_GPIO     GPIO_NUM_33   /* net: MEMS_INIT */
#define BOARD_RESP_INT_GPIO     GPIO_NUM_32   /* net: RESP_INT */

/* -------------------------------------------------------------------------- */
/* UART — A7670E 4G/LTE modem (PPP data path)                                 */
/* -------------------------------------------------------------------------- */
#define BOARD_UART_MODEM        UART_NUM_1
#define BOARD_MODEM_TX_GPIO     GPIO_NUM_16   /* net: GSM_TX */
#define BOARD_MODEM_RX_GPIO     GPIO_NUM_17   /* net: GSM_RX */
#define BOARD_MODEM_RST_GPIO    GPIO_NUM_27   /* net: GSM_RST */
#define BOARD_MODEM_UART_BAUD   115200

/* -------------------------------------------------------------------------- */
/* UART — L89HA GNSS (active when Wi-Fi is unavailable)                       */
/* -------------------------------------------------------------------------- */
#define BOARD_UART_GPS          UART_NUM_2
#define BOARD_GPS_TX_GPIO       GPIO_NUM_12   /* net: GPS_TX */
#define BOARD_GPS_RX_GPIO       GPIO_NUM_14   /* net: GPS_RX */
#define BOARD_GPS_RST_GPIO      GPIO_NUM_18   /* net: GPS_RST */
#define BOARD_GPS_UART_BAUD     9600

/* -------------------------------------------------------------------------- */
/* ADC — thermistor + battery divider                                         */
/* -------------------------------------------------------------------------- */
#define BOARD_TEMP_ADC_GPIO     GPIO_NUM_34   /* net: TEMP_ADC  (ADC1_CH6) */
#define BOARD_VBAT_ADC_GPIO     GPIO_NUM_35   /* net: VBAT_ADC  (ADC1_CH7) */
#define BOARD_VBAT_ADC_EN_GPIO  GPIO_NUM_5    /* net: VBAT_ADC_ON */
#define BOARD_TEMP_VDD_GPIO     GPIO_NUM_26   /* net: ESP_VDD_CTRL / thermistor rail */

/* -------------------------------------------------------------------------- */
/* Board / system controls                                                     */
/* -------------------------------------------------------------------------- */
#define BOARD_POWER_HOLD_GPIO   GPIO_NUM_23   /* net: POWER_HOLD */

/* -------------------------------------------------------------------------- */
/* Boot / strapping                                                           */
/* -------------------------------------------------------------------------- */
#define BOARD_BOOT_KEY_GPIO     GPIO_NUM_0    /* net: BOOT_KEY */

/* -------------------------------------------------------------------------- */
/* Timing helpers (ms)                                                        */
/* -------------------------------------------------------------------------- */
#define BOARD_WIFI_FAIL_THRESHOLD_MS   (3 * 60 * 1000)  /* 3 min before LTE */
#define BOARD_GPS_FIX_TIMEOUT_MS       (120 * 1000)
#define BOARD_MODEM_BOOT_MS            8000

#ifdef __cplusplus
}
#endif
