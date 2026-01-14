/*
 * dust_sensor_gp2y1014.c
 *
 *  Created on: Mar 30, 2025
 *      Author: Marmelada
 */


// dust_sensor_gp2y1014.c

#include "dust_sensor_gp2y1014.h"
#include "stm32l4xx_hal.h"
#include <math.h>

HAL_StatusTypeDef GP2Y1014_ReadDust(GP2Y1014 *dev, float *density_ug_m3) {
    uint32_t adc_raw = 0;
    float voltage = 0.0f;

    // Pornim LED-ul senzorului
    HAL_GPIO_WritePin(dev->led_port, dev->led_pin, GPIO_PIN_SET);
    HAL_Delay(1); // Delay de 1ms pentru stabilizare

    // Citim valoarea de la ADC
    HAL_ADC_Start(dev->adc_handle);
    if (HAL_ADC_PollForConversion(dev->adc_handle, 10) == HAL_OK) {
        adc_raw = HAL_ADC_GetValue(dev->adc_handle);
    }
    HAL_ADC_Stop(dev->adc_handle);

    // Oprim LED-ul senzorului pentru economie de energie
    HAL_GPIO_WritePin(dev->led_port, dev->led_pin, GPIO_PIN_RESET);

    // Conversie ADC -> tensiune (presupunem Vref = 3.3V, 12-bit ADC)
    voltage = (adc_raw * 3.3f) / 4095.0f;

    // Formula aproximativa pentru densitate praf (conform datasheet Sharp)
    float voltage_offset = 0.6f; // offset minim la 0 praf
    float sensitivity = 0.5f;    // V per 100 ug/m3 => 0.5V pentru 100 ug/m3

    float density = (voltage - voltage_offset) * 1000.0f / sensitivity;

    // Protecție la valori negative
    if (density < 0) density = 0;

    *density_ug_m3 = density;
    return HAL_OK;
}
