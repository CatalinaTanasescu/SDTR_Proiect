#ifndef __SLAVE2_H
#define __SLAVE2_H

#include <stdint.h>
#include "tcpserver.h"

void control_slave2_actuators(float temp, int lux, int praf, int soil_moisture);
extern volatile uint8_t rx_complete_slave2;     // ✅ DOAR declarație
extern uint8_t slave2_response[8];
#endif
