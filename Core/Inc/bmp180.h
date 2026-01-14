#ifndef INC_BMP180_H_
#define INC_BMP180_H_

#include "main.h"

#define BMP180_I2C_ADDR 0x77 // 0xEE >> 1 = 0x77 ?
#define CONTROL_REGISTER_VALUE  0XF4
#define BMP180_WRITE 0xEE //0xEE >> 1 = 0x77
#define BMP180_READ 0xEF // lsb setat 1
#define MAX_NO_TEMP 0x04



typedef struct Bmp180CalibrationData {
	int16_t ac1;
	int16_t ac2;
	int16_t ac3;
	uint16_t ac4;
	uint16_t ac5;
	uint16_t ac6;
	int16_t b1;
	int16_t b2;
	int16_t mb;
	int16_t mc;
	int16_t md;
} Bmp180CalibrationData;

typedef enum BMP180_CalIndex {
	BMP180_INDEX_AC1,
	BMP180_INDEX_AC2,
	BMP180_INDEX_AC3,
	BMP180_INDEX_AC4,
	BMP180_INDEX_AC5,
	BMP180_INDEX_AC6,
	BMP180_INDEX_B1,
	BMP180_INDEX_B2,
	BMP180_INDEX_MB,
	BMP180_INDEX_MC,
	BMP180_INDEX_MD,
} BMP180_CalIndex;

extern const uint8_t BMP180_EEPROM_ADDR_MSB[11];
extern const uint8_t BMP180_EEPROM_ADDR_LSB[11] ;

void BMP180_Init();

uint16_t Read_I2C(uint8_t reg);

void BMP180_ReadCalibrationData(Bmp180CalibrationData *data);
int16_t BMP180_ReadTemperature();
float BMP180_CalculateTemperature(uint16_t uncompensatedTemperature,
		Bmp180CalibrationData *calibrationData);


typedef struct cbuff_{
	uint16_t buff[MAX_NO_TEMP];
	uint8_t start; // index de start sau offset
	uint8_t end; // index de final sau offset
	uint16_t size; // cate elem sunt in array
	uint8_t count; //cate elem sunt in array - in mod curent, scade / creste cand stergem / adaugam
} cbuff_t;





void cbuff_new(cbuff_t *storage_buffer, uint16_t size);
void cbuff_add(cbuff_t * cb, uint16_t elem);
uint16_t avg_temperature(cbuff_t * cb);
#endif /* INC_BMP180_H_ */
