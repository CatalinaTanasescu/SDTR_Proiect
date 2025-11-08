
#include "bmp180.h"

const uint8_t BMP180_CALIB_DATA_MSB[11] = { 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 0xbc, 0xbe };
const uint8_t BMP180_CALIB_DATA_LSB[11] = { 0xab, 0xad, 0xaf, 0xb1, 0xb3, 0xb5, 0xb7, 0xb9, 0xbb, 0xbd, 0xbf };


extern I2C_HandleTypeDef hi2c1;

void BMP180_WriteReg(uint8_t reg, uint8_t cmd) {
	uint8_t arr[2] = { reg, cmd };
	HAL_I2C_Master_Transmit(&hi2c1, 0xEE, arr, 2, 1000);

}



uint8_t BMP180_ReadReg(uint8_t reg) {
	HAL_I2C_Master_Transmit(&hi2c1, 0xEE, &reg, 1, 1000);
	uint8_t result;
	HAL_I2C_Master_Receive(&hi2c1, 0xEE, &result, 1, 1000);
	return result;
}


// --------------------------------------------------------------------------------------------

//-----------https://github.com/eepj/BMP180_for_STM32_HAL/blob/master/bmp180_for_stm32_hal.c

void BMP180_ReadCalibrationData(Bmp180CalibrationData *data) {

	data->ac1 = (BMP180_ReadReg(BMP180_CALIB_DATA_MSB[BMP180_INDEX_AC1]) << 8) | BMP180_ReadReg(BMP180_CALIB_DATA_LSB[BMP180_INDEX_AC1]);
	data->ac2 = (BMP180_ReadReg(BMP180_CALIB_DATA_MSB[BMP180_INDEX_AC2]) << 8) | BMP180_ReadReg(BMP180_CALIB_DATA_LSB[BMP180_INDEX_AC2]);
	data->ac3 = (BMP180_ReadReg(BMP180_CALIB_DATA_MSB[BMP180_INDEX_AC3]) << 8) | BMP180_ReadReg(BMP180_CALIB_DATA_LSB[BMP180_INDEX_AC3]);
	data->ac4 = (BMP180_ReadReg(BMP180_CALIB_DATA_MSB[BMP180_INDEX_AC4]) << 8) | BMP180_ReadReg(BMP180_CALIB_DATA_LSB[BMP180_INDEX_AC4]);
	data->ac5= (BMP180_ReadReg(BMP180_CALIB_DATA_MSB[BMP180_INDEX_AC5]) << 8) | BMP180_ReadReg(BMP180_CALIB_DATA_LSB[BMP180_INDEX_AC5]);
	data->ac6 = (BMP180_ReadReg(BMP180_CALIB_DATA_MSB[BMP180_INDEX_AC6]) << 8) | BMP180_ReadReg(BMP180_CALIB_DATA_LSB[BMP180_INDEX_AC6]);
	data->b1 = (BMP180_ReadReg(BMP180_CALIB_DATA_MSB[BMP180_INDEX_B1]) << 8) | BMP180_ReadReg(BMP180_CALIB_DATA_LSB[BMP180_INDEX_B1]);
	data->b2 = (BMP180_ReadReg(BMP180_CALIB_DATA_MSB[BMP180_INDEX_B2]) << 8) | BMP180_ReadReg(BMP180_CALIB_DATA_LSB[BMP180_INDEX_B2]);
	data->mb = (BMP180_ReadReg(BMP180_CALIB_DATA_MSB[BMP180_INDEX_MB]) << 8) | BMP180_ReadReg(BMP180_CALIB_DATA_LSB[BMP180_INDEX_MB]);
	data->mc = (BMP180_ReadReg(BMP180_CALIB_DATA_MSB[BMP180_INDEX_MC]) << 8) | BMP180_ReadReg(BMP180_CALIB_DATA_LSB[BMP180_INDEX_MC]);
	data->md = (BMP180_ReadReg(BMP180_CALIB_DATA_MSB[BMP180_INDEX_MD]) << 8) | BMP180_ReadReg(BMP180_CALIB_DATA_LSB[BMP180_INDEX_MD]);

}





int16_t BMP180_ReadTemperature() {


    uint8_t start_reg = CONTROL_REGISTER_VALUE ;
    uint8_t start_command = 0x2E; // masurare temperatura din reg de ctrl

    BMP180_WriteReg(start_reg, start_command);

    HAL_Delay(4.5);

    int32_t ut = (BMP180_ReadReg(0xf6) << 8) | BMP180_ReadReg(0xf7);


    return ut;
}

float BMP180_CalculateTemperature(uint16_t uncompensatedTemperature,Bmp180CalibrationData *calibrationData) {
	int32_t x1 = ((uncompensatedTemperature - calibrationData->ac6)
			* (int32_t) calibrationData->ac5) >> 15;
	int32_t x2 = ((int32_t) calibrationData->mc << 11)
			/ (x1 + calibrationData->md);

	float temperature = (int32_t) ((x1 + x2 + 8) >> 4);
	temperature /= 10;

	return temperature;
}


// --------------------------------buffer circular---------------------------------------------



void cbuff_new(cbuff_t *storage_buffer, uint16_t size)
{
	storage_buffer->size = size;
	storage_buffer->end = 0;
	storage_buffer->start = 0;
	storage_buffer->count = 0;
}
void cbuff_add(cbuff_t * cb, uint16_t elem)
{
	uint8_t end = cb->end;

	if(cb->count && ((end % cb->size) == cb->start))
	{
		cb->start = (cb->start + 1 ) % cb->size;
		cb->count --;
	}

	cb->buff[cb->end] = elem;
	cb->end = (cb->end+1) % cb->size;
	cb->count ++;
}

uint16_t avg_temperature(cbuff_t * cb)
{
	uint8_t start = cb->start ;
	uint8_t end = cb->end ;
	uint8_t i, count = 0;
	uint16_t avg_temp, temp = 0;

	for(i = start; count < cb->count; i = (i + 1)%cb->size)
	{
		count++;
		temp += cb->buff[i];

		if(i == (end - 1))
		{
			break;
		}
	}

	avg_temp = temp/ cb->count;

	return avg_temp;


}
