/*
 * turbidity.c
 *
 *  Created on: Jan 15, 2026
 *      Author: ashtonhenley
 */
#include "turbidity.h"
#include "math.h"

extern volatile uint16_t adc_buffer [2];

void check_turbidity(uint16_t *turbidity){


	uint16_t adc = adc_buffer[0];
	float voltage = turbidity_to_voltage(adc);

	// Clamp value, not possible to have a voltage greater than 3.3, so check low
	if(voltage <= 0.01){
		*turbidity = 2000;
		return;
	}


	float ntu_f = (voltage - 1.5256)/(-0.0007);

	// Clamp NTU
	if(ntu_f < 0.0f) ntu_f = 0.0f;
	if(ntu_f > 2000.0f) ntu_f = 2000.0f;

	// Round.. adding 0.5 ensures we always round to next integer.
	*turbidity = (int16_t)(ntu_f + 0.5f);


}

float turbidity_to_voltage(uint16_t adc_reading){

	float voltage = (((float)adc_reading * 3.3) / 4095);

	return voltage;
}
