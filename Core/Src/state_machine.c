/*
 * state_machine.c
 *
 *  Created on: Nov 12, 2025
 *      Author: ashtonhenley
 */

#include "state_machine.h"
#include "main.h"
#include "DS3231(CLK).h"
#include "state_logic.h"
#include "physical_controls.h"

extern ScheduledWaterChange sched_date_time;
extern CooldownStruct cooldown;
extern DateTimeStruct curr_date_time;
extern bool water_change_flag;
extern SensorValues sensorvalues;
extern uint8_t schedule;
// Start in idle state
HeatingState current_state = IDLE_STATE;
// Create an instance of ConditionFlags and initialize to 0
ConditionFlags tankFlags = {0};
// Create an instance of OutOfRangeValues and initialize to preset values
OutOfRangeValues rangevalues =
{
	.max_temp = 70,
    .turbidity = 15,
    .low_ph    = 6.5,
    .high_ph   = 8.5
};

void sched_curr_time()
{
    if (cooldown.cooldown_flag == 0)
    {
        if (curr_date_time.year    == sched_date_time.year  &&
            curr_date_time.month   == sched_date_time.month &&
            curr_date_time.day     == sched_date_time.day   &&
            curr_date_time.hours   == sched_date_time.hours &&
            curr_date_time.minutes == sched_date_time.minutes)
        {
        	// Reset fan
        	fan_low();
            current_state = WATER_RES_STATE;
            state_enter();
            water_change_flag = 1;
        }
    }
}

void update_schedule() {

	update_date_time();

	// Set sched_date_time to current time + (offset) days
	uint8_t offDay = curr_date_time.day + schedule;
	uint8_t offMonth = curr_date_time.month;
	uint16_t offYear = curr_date_time.year;

	uint8_t monthRollover;
	switch (curr_date_time.month) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12: // Months with 31 days
		monthRollover = 31;
		break;
	case 2: // February: "every year divisible by 4" is a leap year;
			// note that, because DateTimeStruct stores the "year of the decade," the identification of whether a century's first year is a leap year (i.e. divisible by 400) is impossible
		monthRollover = 28 + (curr_date_time.year % 4 == 0);
		break;
	default: // All other months (30 days)
		monthRollover = 30;
	}

	if (offDay > monthRollover) {
		offDay %= monthRollover;
		offMonth++;
		if (offMonth > 12) {
			offMonth %= 12;
			offYear = (offYear + 1) % 100;
		}
	}

	sched_date_time = (ScheduledWaterChange){
				.day = offDay,
				.month = offMonth,
				.year = offYear,
				.minutes = curr_date_time.minutes,
				.hours = curr_date_time.hours
	};

}

void check_envir_flags()
{
    tankFlags.low_ph    = (sensorvalues.ph_value < rangevalues.low_ph);
    tankFlags.high_ph   = (sensorvalues.ph_value > rangevalues.high_ph);
    tankFlags.turbidity = (sensorvalues.turbidity_value > rangevalues.turbidity);
}

void is_flag_high()
{
    if (cooldown.cooldown_flag == 0)
    {
        if (tankFlags.turbidity || tankFlags.low_ph || tankFlags.high_ph)
        {
        	// Reset fan
        	fan_low();
            current_state     = WATER_RES_STATE;
            state_enter();
            water_change_flag = 1;

            tankFlags.low_ph    = 0;
            tankFlags.high_ph   = 0;
            tankFlags.turbidity = 0;
        }
    }
}

void handle_water_state()
{
    switch (current_state)
    {
        case IDLE_STATE:
        {
            idle_state();
        }
        break;

        case WATER_RES_STATE:
        {
        	water_res_state();
        }
        break;

        case AQUA_DRAIN_STATE:
        {
            water_drain_state();
        }
        break;

        case HEATING_STATE:
        {
        	heating_state();
        }
        break;

        case AQUA_FILL_STATE:
        {
        	water_fill_state();
        break;
    }
    }
}
