/*
 * ph.c
 *
 *  Created on: Jan 27, 2026
 *      Author: ashtonhenley
 */
#include <stdint.h>
#include "ph.h"
extern volatile uint16_t adc_buffer [2];


static ph_sensor ph_instance = {
		.acidVoltage = 2079.12f,
		.neutralVoltage = 1611.72f,
		.voltage = 1611.72f,

};
// Create a pointer to an instance of ph_sensor struct
ph_sensor *ph_struct = &ph_instance;

void read_ph(float *ph_value){
	// Second buffer of adc_buffer is the pH ADC value
	// Take this value, and convert to voltage (mV), then store in struct
	ph_struct->voltage = (float)(((uint32_t)adc_buffer[1] * 3300U) >> 12); // mV
	// 3300u ensures that 3300 is perceived as unsigned,
	// RS 12 will divide by 4096 (small error instead of 4095) much faster than actual division.
	// Now, Calculate slope
	float slope = 9.0f / (ph_struct->neutralVoltage - ph_struct->acidVoltage);
	// Calculate intercept
	float intercept = 7.0f - slope * (ph_struct->neutralVoltage - 2079.12f) / 3.0f;
	// Set ph_value to the calculated pH
	*ph_value = slope*((ph_struct->voltage - 2079.12f)/3.0f) + intercept;
}
