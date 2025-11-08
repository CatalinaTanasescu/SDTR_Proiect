// dust_sensor_gp2y1014.h

#ifndef __DUST_SENSOR_GP2Y1014_H__
#define __DUST_SENSOR_GP2Y1014_H__

#include "stm32l4xx_hal.h"

// Structura senzorului
typedef struct {
    ADC_HandleTypeDef *adc_handle;
    GPIO_TypeDef *led_port;
    uint16_t led_pin;
} GP2Y1014;

// Functie de citire a densitatii prafului in ug/m3
HAL_StatusTypeDef GP2Y1014_ReadDust(GP2Y1014 *dev, float *density_ug_m3);

#endif // __DUST_SENSOR_GP2Y1014_H__
