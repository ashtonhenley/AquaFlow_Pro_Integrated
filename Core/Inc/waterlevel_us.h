/*
 * waterlevel_us.h
 *
 *  Created on: Mar 13, 2026
 *      Author: ashtonhenley
 */

#ifndef INC_WATERLEVEL_US_H_
#define INC_WATERLEVEL_US_H_
#include "main.h"

void read_water_level(int *waterlevel_res, int *waterlevel_tank);

size_t readN(UART_HandleTypeDef *huart, uint8_t *buf, size_t len);
int wl_get_distance(UART_HandleTypeDef *huart);

#endif /* INC_WATERLEVEL_US_H_ */
