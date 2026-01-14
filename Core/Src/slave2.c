#include "slave2.h"
#include "main.h"
#include <stdint.h>
#include <string.h>
#include "cmsis_os2.h"

extern UART_HandleTypeDef huart1;

#define SLAVE2_ADDR     0x02
#define MODBUS_FC_WRITE_MULTIPLE_REGS 0x10
uint8_t slave2_response[8];
volatile uint8_t rx_complete_slave2 = 0;
extern struct senzor_data sensorData;

extern osMutexId_t sensorDataMutexHandle;
extern osMutexId_t uartMutexHandle;

extern uint8_t uart_rx_buffer[32];

extern uint8_t slave1_rx_ready;
extern uint8_t slave2_rx_ready;

extern uint8_t slave1_rx[16];
extern uint8_t slave2_rx[16];



// Registrele de control
#define REG_FAN         0x0001
#define REG_PUMP        0x0002
#define REG_PISTON      0x0003
#define REG_BEC         0x0004

static uint16_t calc_crc(uint8_t *data, size_t length)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; i++) {
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

static void slave2_send_all(uint16_t fan, uint16_t pump, uint16_t piston, uint16_t bec)
{
    /*uint8_t frame[17];
    frame[0] = SLAVE2_ADDR;
    frame[1] = MODBUS_FC_WRITE_MULTIPLE_REGS;
    frame[2] = 0x00;
    frame[3] = 0x01;  // Start addr: 0x0001
    frame[4] = 0x00;
    frame[5] = 0x04;  // 4 registre
    frame[6] = 8;     // Byte count

    // Datele efective: FAN, PUMP, PISTON, BEC
    frame[7] = (fan >> 8) & 0xFF;
    frame[8] = fan & 0xFF;
    frame[9] = (pump >> 8) & 0xFF;
    frame[10] = pump & 0xFF;
    frame[11] = (piston >> 8) & 0xFF;
    frame[12] = piston & 0xFF;
    frame[13] = (bec >> 8) & 0xFF;
    frame[14] = bec & 0xFF;

    // CRC
    uint16_t crc = calc_crc(frame, 15);
    frame[15] = crc & 0xFF;
    frame[16] = (crc >> 8) & 0xFF;

    rx_complete_slave2 = 0;
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, slave2_response, 8);
    HAL_Delay(5);
    HAL_UART_Transmit(&huart1, frame, 17, 1000);

    uint32_t start = HAL_GetTick();
    while (!rx_complete_slave2 && (HAL_GetTick() - start < 5000));

       // Verificare minimă răspuns
    if (rx_complete_slave2) {
        if (slave2_response[0] == SLAVE2_ADDR && slave2_response[1] == 0x10) {
            // Răspuns valid
        }
    }

    memset(slave2_response, 0, sizeof(slave2_response));
    */

	// ===========================
	    // 1. Protecție UART cu MUTEX
	    // ===========================
	    osMutexAcquire(uartMutexHandle, osWaitForever);

	    // Resetăm flag-ul de recepție
	    slave2_rx_ready = 0;

	    // ===========================
	    // 2. Construim cadrul Modbus
	    // ===========================
	    uint8_t frame[17];

	    frame[0] = SLAVE2_ADDR;                     // 0x02
	    frame[1] = MODBUS_FC_WRITE_MULTIPLE_REGS;   // 0x10

	    frame[2] = 0x00;                            // Start address high
	    frame[3] = 0x01;                            // Start address low (reg 1)

	    frame[4] = 0x00;                            // Number of registers high
	    frame[5] = 0x04;                            // 4 registers

	    frame[6] = 8;                                // Byte count

	    frame[7]  = fan >> 8;     frame[8]  = fan & 0xFF;
	    frame[9]  = pump >> 8;    frame[10] = pump & 0xFF;
	    frame[11] = piston >> 8;  frame[12] = piston & 0xFF;
	    frame[13] = bec >> 8;     frame[14] = bec & 0xFF;

	    // Adăugăm CRC
	    uint16_t crc = calc_crc(frame, 15);
	    frame[15] = crc & 0xFF;
	    frame[16] = crc >> 8;

	    // ===========================
	    // 3. Pornim recepția IDLE
	    // ===========================
	    HAL_UARTEx_ReceiveToIdle_IT(&huart1, uart_rx_buffer, sizeof(uart_rx_buffer));

	    // ===========================
	    // 4. Trimitem cadrul Modbus
	    // ===========================
	    HAL_UART_Transmit(&huart1, frame, sizeof(frame), HAL_MAX_DELAY);

	    // ===========================
	    // 5. Așteptăm răspunsul slave2
	    // ===========================
	    uint32_t t0 = HAL_GetTick();
	    while (!slave2_rx_ready && (HAL_GetTick() - t0 < 100));

	    // ===========================
	    // 6. Validăm răspunsul (opțional)
	    // ===========================
	    if (slave2_rx_ready)
	    {
	        // Răspunsul corect are formatul:
	        // [addr][0x10][hi start][lo start][hi nr reg][lo nr reg][CRClo][CRChi]

	        if (slave2_rx[0] == SLAVE2_ADDR && slave2_rx[1] == 0x10)
	        {
	            // Optional: verificare CRC
	            uint16_t crc_calc = calc_crc(slave2_rx, 6);
	            uint16_t crc_recv = (slave2_rx[7] << 8) | slave2_rx[6];

	            if (crc_calc == crc_recv)
	            {
	                // totul OK
	                // aici un flag de "write ok"
	            }
	        }
	    }

	    // ===========================
	    // 7. Eliberăm UART-ul
	    // ===========================
	    osMutexRelease(uartMutexHandle);
}

// 🔁 Funcția principală apelată din task
void control_slave2_actuators(float temp, int lux, int praf, int soil_moisture) {
    uint16_t fan = 0;
    uint16_t piston = 0;
    uint16_t pump = 0;
    uint16_t bec = 0;

    if (temp > 27.0f) fan = 1;
    if (praf > 220) piston = 1;
    if (lux < 300) bec = 1;
    if (soil_moisture < 40) pump = 1;

    slave2_send_all(fan, pump, piston, bec);

    osMutexAcquire(sensorDataMutexHandle, osWaitForever);

    // Salvează în structura globală
    sensorData.fan = fan;
    sensorData.piston = piston;
    sensorData.pump = pump;
    sensorData.bec = bec;

    // De exemplu, dacă umiditatea solului < 400 → pompez apă
    // pump = (umiditate < prag) ? 1 : 0;
    osMutexRelease(sensorDataMutexHandle);

}
