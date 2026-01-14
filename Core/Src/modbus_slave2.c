#include "modbus_slave2.h"
#include "actuators.h"
#include "main.h"
#include <string.h>

extern UART_HandleTypeDef huart1;

typedef struct {
    uint16_t reg;
    uint16_t value;
    uint8_t pending;
} ModbusCommand;

ModbusCommand current_command = {0};

uint16_t modbus_crc16(uint8_t *buf, uint8_t len) {
    uint16_t crc = 0xFFFF;
    for (uint8_t pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void modbus_slave_process_uart(uint8_t *rx, uint8_t len) {
    if (len < 8) return;

    uint16_t received_crc = (rx[len - 1] << 8) | rx[len - 2];
    uint16_t computed_crc = modbus_crc16(rx, len - 2);
    if (received_crc != computed_crc) return;

    if (rx[0] != 0x02) return;

    if (rx[1] == 0x10) {
        if (len < 9) return;

        uint16_t start_addr = (rx[2] << 8) | rx[3];
        uint16_t quantity = (rx[4] << 8) | rx[5];
        uint8_t byte_count = rx[6];

        if (byte_count != quantity * 2) return;
        if (len < (7 + byte_count + 2)) return;

        for (uint16_t i = 0; i < quantity; i++) {
            uint16_t value = (rx[7 + i * 2] << 8) | rx[8 + i * 2];
            uint16_t reg_addr = start_addr + i;

            switch (reg_addr) {
                case 0x0001: set_fan(value); break;
                case 0x0002: set_pump(value); break;
                case 0x0003: set_piston(value); break;
                case 0x0004: set_bec(value); break;
                default: break;
            }
        }

        uint8_t tx_buf[32];
        uint8_t pos = 0;

        tx_buf[pos++] = 0x02;         // Slave ID
        tx_buf[pos++] = 0x90;         // Custom function code: 0x90 = status reply
        tx_buf[pos++] = (start_addr >> 8) & 0xFF;
        tx_buf[pos++] = start_addr & 0xFF;
        tx_buf[pos++] = (quantity >> 8) & 0xFF;
        tx_buf[pos++] = quantity & 0xFF;
        tx_buf[pos++] = quantity * 2;

        for (uint16_t i = 0; i < quantity; i++) {
            uint16_t reg = start_addr + i;
            uint16_t state = 0x0000;

            switch (reg) {
                case 0x0001: state = HAL_GPIO_ReadPin(IN1_FAN_GPIO_Port, IN1_FAN_Pin); break;
                case 0x0002: state = HAL_GPIO_ReadPin(IN1_POMPA_GPIO_Port, IN1_POMPA_Pin); break;
                case 0x0003: state = HAL_GPIO_ReadPin(IN1_PISTON_GPIO_Port, IN1_PISTON_Pin); break;
                case 0x0004: state = HAL_GPIO_ReadPin(IN1_BEC_GPIO_Port, IN1_BEC_Pin); break;
                default: state = 0x0000; break;
            }

            tx_buf[pos++] = (state >> 8) & 0xFF;
            tx_buf[pos++] = state & 0xFF;
        }

        uint16_t crc = modbus_crc16(tx_buf, pos);
        tx_buf[pos++] = crc & 0xFF;
        tx_buf[pos++] = (crc >> 8) & 0xFF;

        HAL_UART_Transmit(&huart1, tx_buf, pos, HAL_MAX_DELAY);
    }
}

uint8_t modbus_command_pending(void) {
    return current_command.pending;
}

void modbus_command_execute(UART_HandleTypeDef *huart) {
    if (!current_command.pending) return;

    switch (current_command.reg) {
        case 0x0001: set_fan(current_command.value); break;
        case 0x0002: set_pump(current_command.value); break;
        case 0x0003: set_piston(current_command.value); break;
        case 0x0004: set_bec(current_command.value); break;
        default: break;
    }

    uint8_t tx_buf[8];
    tx_buf[0] = 0x02;
    tx_buf[1] = 0x06;
    tx_buf[2] = (current_command.reg >> 8) & 0xFF;
    tx_buf[3] = current_command.reg & 0xFF;
    tx_buf[4] = (current_command.value >> 8) & 0xFF;
    tx_buf[5] = current_command.value & 0xFF;

    uint16_t crc = modbus_crc16(tx_buf, 6);
    tx_buf[6] = crc & 0xFF;
    tx_buf[7] = (crc >> 8) & 0xFF;

    HAL_UART_Transmit(huart, tx_buf, 8, HAL_MAX_DELAY);
    current_command.pending = 0;
}
