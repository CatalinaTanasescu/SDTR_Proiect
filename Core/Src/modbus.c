#include "modbus.h"
#include "main.h"
#include "soil_moisture.h"
#include "bmp180.h"
#include "BH1750_(light_intensity_sensor).h"
#include "dust_sensor_gp2y1014.h"
#include <string.h>


extern UART_HandleTypeDef huart1;
extern BH1750 bh1750_dev;
extern GP2Y1014 dust_sensor;
extern Bmp180CalibrationData calibrationData;
extern uint8_t response_size;
extern struct ModbusEntry mb_map[MAP_COUNT];

// Funcție CRC
uint16_t calc_crc(uint8_t *data, size_t size) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < size; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

// Trimite răspunsul prin UART
void send_response(uint8_t *response_array) {
    HAL_UART_Transmit(&huart1, response_array, response_size, 1000);
}

// Tratare cerere
void treat_request(uint8_t *received_byte, uint8_t *response_array, size_t size, struct ModbusEntry *mb_map) {
    if (size < 8) return;

    uint16_t received_crc = (received_byte[size - 1] << 8) | received_byte[size - 2];
    uint16_t calculated_crc = calc_crc(received_byte, size - 2);
    if (received_crc != calculated_crc) return;

    if (received_byte[0] != DEVICE_ADDRESS || received_byte[1] != READ_FUNCTION_CODE) return;

    uint16_t start_address = (received_byte[2] << 8) | received_byte[3];
    uint16_t num_registers = (received_byte[4] << 8) | received_byte[5];
    if (start_address != 0x0001 || num_registers != 0x0004) return;

    // ==== Citire senzor BMP180 (temperatură) ====
    int16_t uncomp_temp = BMP180_ReadTemperature();
    float temperature = BMP180_CalculateTemperature(uncomp_temp, &calibrationData);
    int16_t temperature_value = (int16_t)(temperature * 10);

    // ==== Citire senzor lumină BH1750 ====
    float lux = 0;
    BH1750_ReadLux(&bh1750_dev, &lux);
    uint16_t lux_value = (uint16_t)(lux);  // se poate înmulți și cu 10 dacă vrei precizie mai mare

    // ==== Citire senzor praf GP2Y1014 ====
    float dust_density = 0;
    GP2Y1014_ReadDust(&dust_sensor, &dust_density);
    uint16_t dust_value = (uint16_t)(dust_density);  // se poate înmulți și cu 10


    // ==== Citire senzor umiditate sol ====
    uint32_t soil_analog = SoilMoisture_ReadAnalog();
    uint16_t soil_percentage = (uint16_t)(CalculateMoisturePercentage(soil_analog));

    // Umplem mb_map cu valorile senzorilor
    mb_map[0].register_ID = 0x0001;
    mb_map[0].register_count = 1;
    mb_map[0].register_values[0] = (temperature_value >> 8) & 0xFF;
    mb_map[0].register_values[1] = temperature_value & 0xFF;

    mb_map[1].register_ID = 0x0002;
    mb_map[1].register_count = 1;
    mb_map[1].register_values[0] = (lux_value >> 8) & 0xFF;
    mb_map[1].register_values[1] = lux_value & 0xFF;

    mb_map[2].register_ID = 0x0003;
    mb_map[2].register_count = 1;
    mb_map[2].register_values[0] = (dust_value >> 8) & 0xFF;
    mb_map[2].register_values[1] = dust_value & 0xFF;

    mb_map[3].register_ID = 0x0004;
    mb_map[3].register_count = 1;
    mb_map[3].register_values[0] = (soil_percentage >> 8) & 0xFF;
    mb_map[3].register_values[1] = soil_percentage & 0xFF;


    // Construim răspunsul
    response_array[0] = DEVICE_ADDRESS;
    response_array[1] = READ_FUNCTION_CODE;
    response_array[2] = 8;  // 4 registre * 2 bytes = 8

    response_array[3] = mb_map[0].register_values[0];
    response_array[4] = mb_map[0].register_values[1];
    response_array[5] = mb_map[1].register_values[0];
    response_array[6] = mb_map[1].register_values[1];
    response_array[7] = mb_map[2].register_values[0];
    response_array[8] = mb_map[2].register_values[1];
    response_array[9] = mb_map[3].register_values[0];
    response_array[10] = mb_map[3].register_values[1];

    response_size = 11;

    uint16_t crc = calc_crc(response_array, response_size);
    response_array[11] = crc & 0xFF;
    response_array[12] = (crc >> 8) & 0xFF;

    response_size = 13;
}
