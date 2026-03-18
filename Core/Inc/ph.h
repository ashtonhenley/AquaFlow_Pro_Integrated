/*
 * ph.h
 *
 *  Created on: Jan 27, 2026
 *      Author: ashtonhenley
 */

#ifndef INC_PH_H_
#define INC_PH_H_

typedef struct{
	float acidVoltage;  // Value in mV of sensor in acid solution
	float neutralVoltage; // Value in mV of sensor in neutral solution
	float voltage;      // Voltage from ADC
}ph_sensor;

void read_ph(float *ph_value);

#endif /* INC_PH_H_ */
