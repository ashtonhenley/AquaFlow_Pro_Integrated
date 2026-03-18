/*
 * state_machine.h
 *
 *  Created on: Nov 12, 2025
 *      Author: ashtonhenley
 */

#ifndef INC_STATE_MACHINE_H_
#define INC_STATE_MACHINE_H_
#include <stdbool.h>
#include <stdint.h>
typedef struct{
	bool turbidity;
	bool low_ph;
	bool high_ph;
}ConditionFlags;

typedef struct{
	float max_temp;
	uint8_t turbidity;
	float low_ph;
	float high_ph;
}OutOfRangeValues;

typedef enum
{
    IDLE_STATE,
    WATER_RES_STATE,
    HEATING_STATE,
    AQUA_DRAIN_STATE,
    AQUA_FILL_STATE
} HeatingState;

void sched_curr_time();
void is_flag_high();
void handle_water_state();
void check_envir_flags();
void update_schedule();
#endif /* INC_STATE_MACHINE_H_ */
