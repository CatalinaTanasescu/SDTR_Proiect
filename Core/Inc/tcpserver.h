/*
 * tcpserver.h
 *
 *  Created on: Apr 20, 2022
 *      Author: controllerstech
 */

#ifndef INC_TCPSERVER_H_
#define INC_TCPSERVER_H_

struct senzor_data {
    float temp;
    int lux;
    int praf;
    int soil_moisture;
    uint8_t fan;
    uint8_t pump;
    uint8_t piston;
    uint8_t bec;
};

extern struct senzor_data sensorData;

void tcpserver_init (void);

#endif /* INC_TCPSERVER_H_ */
