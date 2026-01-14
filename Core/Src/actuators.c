#include "actuators.h"
#include "main.h" // important pentru a avea acces la *_Pin și *_GPIO_Port

void actuators_init(void)
{
    // GPIO e deja inițializat din CubeMX, deci nu faci nimic aici.
}

void set_fan(uint8_t on)
{
	 HAL_GPIO_WritePin(IN1_FAN_GPIO_Port, IN1_FAN_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(IN2_FAN_GPIO_Port, IN2_FAN_Pin, GPIO_PIN_RESET); // Mereu low
}

void set_pump(uint8_t on)
{
	 HAL_GPIO_WritePin(IN1_POMPA_GPIO_Port, IN1_POMPA_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(IN2_POMPA_GPIO_Port, IN2_POMPA_Pin, GPIO_PIN_RESET); // Mereu low
}

void set_piston(uint8_t on)
{
    HAL_GPIO_WritePin(IN1_PISTON_GPIO_Port, IN1_PISTON_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IN2_PISTON_GPIO_Port, IN2_PISTON_Pin, !on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void set_bec(uint8_t on)
{
    HAL_GPIO_WritePin(IN1_BEC_GPIO_Port, IN1_BEC_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IN2_BEC_GPIO_Port, IN2_BEC_Pin, !on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
