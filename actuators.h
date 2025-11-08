#ifndef __ACTUATORS_H
#define __ACTUATORS_H

#include <stdint.h>

void set_fan(uint8_t on);
void set_pump(uint8_t on);
void set_piston(uint8_t on);
void set_bec(uint8_t on);

#endif
