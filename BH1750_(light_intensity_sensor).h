#ifndef BH1750_H
#define BH1750_H

#include "stm32l4xx_hal.h" // Sau modifică în funcție de seria ta STM32

#define BH1750_ADDR_LOW   (0x23 << 1)  // ADDR la GND
#define BH1750_ADDR_HIGH  (0x5C << 1)  // ADDR la VCC

#define BH1750_POWER_ON   0x01
#define BH1750_RESET      0x07
#define BH1750_CONT_H_RES 0x10

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t address;
} BH1750;

HAL_StatusTypeDef BH1750_Init(BH1750 *dev);
HAL_StatusTypeDef BH1750_ReadLux(BH1750 *dev, float *lux);

#endif
