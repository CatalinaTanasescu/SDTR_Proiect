/*
 * BH1750_(light_intensity_sensor).c
 *
 *  Created on: Mar 30, 2025
 *      Author: Marmelada
 */


#include "BH1750_(light_intensity_sensor).h"
#include "stm32l4xx_hal.h"
#include "math.h"

HAL_StatusTypeDef BH1750_Init(BH1750 *dev) {
    HAL_StatusTypeDef res;
    uint8_t cmd;

    cmd = BH1750_POWER_ON;
    res = HAL_I2C_Master_Transmit(dev->hi2c, dev->address, &cmd, 1, HAL_MAX_DELAY);
    HAL_Delay(10);

    cmd = BH1750_RESET;
    res |= HAL_I2C_Master_Transmit(dev->hi2c, dev->address, &cmd, 1, HAL_MAX_DELAY);
    HAL_Delay(10);

    cmd = BH1750_CONT_H_RES;
    res |= HAL_I2C_Master_Transmit(dev->hi2c, dev->address, &cmd, 1, HAL_MAX_DELAY);
    HAL_Delay(180);  // Timp minim de conversie pentru H-Res Mode

    return res;
}

HAL_StatusTypeDef BH1750_ReadLux(BH1750 *dev, float *lux) {
    uint8_t data[2];
    HAL_StatusTypeDef res;

    res = HAL_I2C_Master_Receive(dev->hi2c, dev->address, data, 2, HAL_MAX_DELAY);

    if (res == HAL_OK) {
        uint16_t raw = (data[0] << 8) | data[1];
        *lux = (float)raw / 1.2f; // Conform datasheet: Lux = raw / 1.2
    }

    return res;
}
