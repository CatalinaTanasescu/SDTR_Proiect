#ifndef INC_MODBUS_H_
#define INC_MODBUS_H_

#include "main.h"
#include <stdint.h>  // Asigură-te că ai inclus această bibliotecă pentru tipurile de date standard

#define LSB(x) ((x) & ((1 << 8) - 1))
#define MSB(x) ((x) >> 8)

/*---------------------------------------------------------------------------------------*/
#define MAP_COUNT 3
#define DEVICE_ADDRESS 0xAA
#define SIZE 8
#define CRC_SIZE 2
#define DEFAULT_SIZE 100

extern uint8_t received_request[SIZE];  // Declarare externă pentru a fi folosită și în modbus.c

#define READ_FUNCTION_CODE 0x03
#define WRITE_FUNCTION_CODE 0x06

/*---------------------------------------------------------------------------------------*/

#define EXCEPTION_ILLEGAL_FUNCTION 0x01
#define EXCEPTION_DATA_ADDRESS 0x02
#define EXCEPTION_PARITY_ERROR 0x03
#define EXCEPTION_SLAVE_FAILURE 0x04

// Structura ModbusEntry trebuie definită aici
struct ModbusEntry {
    uint16_t register_ID;
    uint8_t register_count;
    uint8_t register_values[2];
};

// Variabila globala mb_map este definita in main.c
extern struct ModbusEntry mb_map[MAP_COUNT];

uint16_t calc_crc(uint8_t *received_byte, size_t size);
void send_response(uint8_t *response_array);
void treat_request(uint8_t *received_byte, uint8_t *response_array, size_t size, struct ModbusEntry *mb_map);
void read_holding_registers(uint8_t *received_byte, struct ModbusEntry *mb_map, uint8_t *response_array);
void write_single_register(uint8_t *received_byte, struct ModbusEntry *mb_map, uint8_t *response_array);

#endif /* INC_MODBUS_H_ */
