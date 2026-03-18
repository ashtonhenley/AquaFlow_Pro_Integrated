/*
 * turbidity.h
 *
 *  Created on: Jan 15, 2026
 *      Author: ashtonhenley
 */

#include "main.h"
#include "stdint.h"

#ifndef INC_TURBIDITY_H_
#define INC_TURBIDITY_H_

void check_turbidity(uint16_t *turbidity);
float turbidity_to_voltage(uint16_t adc_reading);


#endif /* INC_TURBIDITY_H_ */
