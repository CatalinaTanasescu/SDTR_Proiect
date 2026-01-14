#include "soil_moisture.h"
#include "stm32l4xx_hal.h"

// External variables
extern ADC_HandleTypeDef hadc1;

// Calibration values
#define NUM_SAMPLES 5
#define CALIBRATION_DRY 3900
#define CALIBRATION_WET 1800

/**
 * @brief Reads and averages the soil moisture analog value
 * @retval uint32_t Averaged ADC raw value
 */
uint32_t SoilMoisture_ReadAnalog(void) {
    uint32_t total_value = 0;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        HAL_ADC_Start(&hadc1);

        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) { // MAX 10ms asteptare
            total_value += HAL_ADC_GetValue(&hadc1);
        }

        HAL_ADC_Stop(&hadc1);
    }

    return total_value / NUM_SAMPLES;
}

/**
 * @brief Maps the ADC value to a moisture percentage (0% wet, 100% dry)
 * @param analogValue Raw ADC value
 * @retval int Moisture percentage
 */
int CalculateMoisturePercentage(uint32_t analogValue) {
    if (analogValue <= CALIBRATION_WET) {
        return 100;
    } else if (analogValue >= CALIBRATION_DRY) {
        return 0;
    } else {
    	return 100 - ((100 * (analogValue - CALIBRATION_WET)) / (CALIBRATION_DRY - CALIBRATION_WET));
    }
}
