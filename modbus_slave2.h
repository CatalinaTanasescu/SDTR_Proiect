#ifndef __MODBUS_SLAVE2_H
#define __MODBUS_SLAVE2_H

#include <stdint.h>
#include "stm32l4xx_hal.h"

void modbus_slave_process_uart(uint8_t *rx, uint8_t len);
uint16_t modbus_crc16(uint8_t *buf, uint8_t len);
void modbus_command_execute(UART_HandleTypeDef *huart);  // pentru completitudine
uint8_t modbus_command_pending(void);  // dacă vrei să verifici în task


#endif
