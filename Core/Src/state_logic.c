/*
 * state_logic.c
 *
 *  Created on: Mar 5, 2026
 *      Author: ashtonhenley
 */


#include "state_logic.h"
#include "main.h"
#include "state_machine.h"
#include "DS3231(CLK).h"
#include "mcp23017.h"
#include "turbidity.h"
#include "DS18B20.h"
#include "ph.h"
#include "waterlevel_us.h"
#include <math.h>
#include "physical_controls.h"
#include "LCD.h"
#include "keypad.h"
#include <stdio.h>
// High "heading into" the next state
static uint8_t state_entry = 1; // High on startup (for "entry" into the idle state)

// I2C Handles
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

// UART Handles
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

// External Structs
extern CooldownStruct cooldown;
extern HeatingState current_state;
extern SensorValues sensorvalues;
extern DateTimeStruct curr_date_time;
extern ScheduledWaterChange sched_date_time;
extern OutOfRangeValues rangevalues;
// All external variables
extern bool water_change_flag; // Additionally referenced during routine in the case of a "change out of the menu" mentioned below
extern bool lids;
extern bool res_full_flag;
static uint32_t fill_elapsed = 0;   // total pump runtime accumulated
static uint32_t fill_start_sod = 0; // start time of the current run
uint32_t delta = 0;			  // time elapsed since last check
uint8_t flow_rate = 21;   // Time it takes to pump 1 gallon in seconds
bool overheat = 0;

float FAN_ON_TEMP;
float FAN_OFF_TEMP;
bool fan_on = 0;
static bool inbound_pump_on = 0;

const uint8_t minimum_tank_val = 12; // Distance between sensor and water once water has been drained
const uint8_t maximum_tank_val = 2; // Distance between sensor and water once water has been pumped back into the tank
const uint8_t minimum_res_val = 4; // Distance between sensor and water to allow for a water change to begin

extern uint8_t schedule;

extern bool manualStartFlag;
extern bool menuExit; // Set high upon a menu exit.
// Referenced for idle screen display "out of the menu;" must be reconciled if a change is to begin at that time instead; that is, it must be set low wherever water_change_flag is set high

extern char XX[3];
extern char XXdXX[5];
extern uint16_t XXXX;

extern uint16_t keypadIter;
extern uint8_t screenSwitch;
float estTime = 0; // For time estimates (to be encoded and displayed)

extern char readKey; // For input required during reservoir check routine

// Helpers for numerical value encoding (see encoding buffers declared in main.c)

// Initializes XX to a string in the format "XX"
void num_to_char_2(uint8_t num) { // Assumed: num < 100
	if (num < 10) {
		XX[0] = '0';
		sprintf(XX+1, "%u", num);
	}
	else sprintf(XX, "%u", num);
}

void num_to_char_trbdty(uint16_t num) { // For turbidity only - cap input integers > 100 at 99
	if (num > 99) {
		XX[0] = '9';
		XX[1] = '9';
		XX[2] = '\0';
	}
	else if (num < 10) {
		XX[0] = '0';
		sprintf(XX+1, "%u", num);
	}
	else sprintf(XX, "%u", num);
}

// Initializes XXdXX to a string in the format "XX.XX"
void num_to_char_4(float num) { // Assumed: num < 100
	XXXX = (uint16_t)(num*100);
	num_to_char_2(XXXX/100);
	XXdXX[0] = XX[0];
	XXdXX[1] = XX[1];
	XXdXX[2] = '.';
	num_to_char_2(XXXX%100);
	XXdXX[3] = XX[0];
	XXdXX[4] = XX[1];
}

void state_enter(){
	state_entry = 1;
}
void state_maintain(){
	state_entry = 0;
}

uint32_t get_seconds_of_day(void)
{
	return (uint32_t)curr_date_time.hours * 3600u +
			(uint32_t)curr_date_time.minutes * 60u +
			(uint32_t)curr_date_time.seconds;
}

void idle_state(){

	// Bail out in the case of a manual water change
	if (manualStartFlag){
		water_change_flag = 1;
		current_state = WATER_RES_STATE;
		state_enter();
		manualStartFlag = 0;
		return;
	}

	static uint32_t idle_sod = 0;
	bool time_reached;
	char temp_buf[6];
	char ph_buf[6];

	// Initialize range values for fan
	if(state_entry){
		// Get the current time
		idle_sod = get_seconds_of_day();
		FAN_ON_TEMP = rangevalues.max_temp;
		FAN_OFF_TEMP = rangevalues.max_temp - 2;
	}
	// Check... have we reached our time?
	time_reached = timer_expired(
			idle_sod,
			4u,     // Time in seconds that determines the interval of time_reached, also controls how often we sample sensors
			curr_date_time.hours,
			curr_date_time.minutes,
			curr_date_time.seconds
	);

	if (state_entry || time_reached)
	{
		// If it's time to sample sensors, take samples
		sample_temperature_sensors();
		check_turbidity(&sensorvalues.turbidity_value);
		read_ph(&sensorvalues.ph_value);
		FAN_ON_TEMP = rangevalues.max_temp;
		FAN_OFF_TEMP = rangevalues.max_temp - 2;
		// Only read water level sensors if the reservoir lid is necessarily on (e.g. when the reservoir isn't being filled)
		if(lids){
			read_water_level(&sensorvalues.waterlevel_res, &sensorvalues.waterlevel_tank);
			res_full_flag = (sensorvalues.waterlevel_res < minimum_res_val);
		}
		// Need to check if these sensors values are out of range
		check_envir_flags();
		is_flag_high();

		// Check for a scheduled change
		sched_curr_time();
	}
	if (time_reached) idle_sod = get_seconds_of_day();

	// At this point, the remaining parts of the routine can be bailed out of if a water change is slated to begin
	if (water_change_flag) return;

	/* Are we over max temperature allowed?
	 * if so, turn on fan, otherwise, turn off fan
	 */

	if(!fan_on && sensorvalues.temperature_tank > FAN_ON_TEMP)
	{
		fan_high();
		fan_on = 1;
	}
	else if(fan_on && sensorvalues.temperature_tank < FAN_OFF_TEMP)
	{
		fan_low();
		fan_on = 0;
	}

	// Interface: display idle mode screen with (up-to-date) relevant information. To be done only upon state entry (+ out of the menu) or when sensor values are updated;
	// furthermore, a call after sensing-related processing has occurred allows for ability to prevent a display routine "right before" a sensed/scheduled change occurs (see above)
	if (menuExit || state_entry || time_reached) {

		if (menuExit || state_entry) screenSwitch = 0;
		if (time_reached) screenSwitch = (screenSwitch + 1) % 2;

		num_to_char_4(sensorvalues.temperature_tank);
		temp_buf[0] = XXdXX[0];
		temp_buf[1] = XXdXX[1];
		temp_buf[2] = XXdXX[2];
		temp_buf[3] = XXdXX[3];
		temp_buf[4] = XXdXX[4];
		temp_buf[5] = '\0';
		num_to_char_4(sensorvalues.ph_value);
		ph_buf[0] = XXdXX[0];
		ph_buf[1] = XXdXX[1];
		ph_buf[2] = XXdXX[2];
		ph_buf[3] = XXdXX[3];
		ph_buf[4] = XXdXX[4];
		ph_buf[5] = '\0';
		num_to_char_trbdty(sensorvalues.turbidity_value);
		LCD_ShowIdleOverview(temp_buf, XX, ph_buf);

		menuExit = 0;
		state_maintain();

		switch (screenSwitch) {
		case 0: // Current time: 	|   XX/XX XX:XX XM   |
			LCD_ShowIdleCurrentTime(
					curr_date_time.month,
					curr_date_time.day,
					curr_date_time.hours,
					curr_date_time.minutes
			);
			break;
		case 1: // Scheduled time:	|SCHEDULED XX/XX XXXM|
			LCD_ShowIdleScheduledTime(
					sched_date_time.month,
					sched_date_time.day,
					sched_date_time.hours
			);
			break;
		}

	}

}

void water_res_state(){

	if (state_entry) {

		if(res_full_flag && lids){
			res_full_flag = 0;
			current_state = AQUA_DRAIN_STATE;
			state_enter();
			return;
		}
		LCD_ShowReservoirCheckScreen();

		state_maintain();

	}
	else {
		// Lids may not be on heading into routine for the first time; outside of this, read (e.g. to obtain new progress %)
		if (lids) read_water_level(&sensorvalues.waterlevel_res, &sensorvalues.waterlevel_tank);
		num_to_char_2((uint8_t)((float)sensorvalues.waterlevel_res*3.3333f)); // Percentage = 100 * level/30
		LCD_ShowReservoirProgress(XX);

		LCD_SetCursor(60);
		LCD_WriteString("#)CONFIRM ");

		HAL_Delay(400);
		char k = '0';
		while (k != '#') k = Keypad_ReadDebouncedKeyPress();

		// "Check routine" - gauge water level after every user input (assumption: lids closed, reservoir filled. Can only truly verify the latter)

		lids = 1;

		num_to_char_2((uint8_t)((float)sensorvalues.waterlevel_res*3.3333f));

		while (sensorvalues.waterlevel_res >= minimum_res_val) {

			num_to_char_2((uint8_t)((float)sensorvalues.waterlevel_res*3.3333f)); // Percentage = 100 * level/30
			LCD_ShowReservoirProgress(XX);
			LCD_SetCursor(60);
			LCD_WriteString("#)RETRY!  ");

			HAL_Delay(400);
			char k = '0';
			while (k != '#') k = Keypad_ReadDebouncedKeyPress();

			read_water_level(&sensorvalues.waterlevel_res, &sensorvalues.waterlevel_tank);
		}

	}

	res_full_flag = 0;
	current_state = AQUA_DRAIN_STATE;
	state_enter();
}

void water_drain_state(){
	static uint32_t drain_sod = 0;
	bool time_reached;
	char elapsed_buf[6];

	if (state_entry)
	{
		// Delay for ensuring pumps don't shock PSU
		HAL_Delay(2000);
		// Set outbound pump high
		LCD_ShowDrainScreen(flow_rate);
		outbound_pump_high();

		// Get start time of outbound pump
		drain_sod = get_seconds_of_day();
		state_maintain();
		keypadIter = 0;
		estTime = 0;

		state_maintain();
	}
	if (keypadIter > 165) {
		estTime += 0.1666f;
		num_to_char_4(estTime);
		elapsed_buf[0] = XXdXX[0];
		elapsed_buf[1] = XXdXX[1];
		elapsed_buf[2] = XXdXX[2];
		elapsed_buf[3] = XXdXX[3];
		elapsed_buf[4] = XXdXX[4];
		elapsed_buf[5] = '\0';
		LCD_ShowElapsedTime(elapsed_buf);
		keypadIter = 0;
	}
	if (state_entry == 0)
	{
		// Check water level of main tank to ensure we don't over drain
		read_water_level(&sensorvalues.waterlevel_res, &sensorvalues.waterlevel_tank);

		// Check to see if flow rate in seconds has been reached
		time_reached = timer_expired(
				drain_sod,
				flow_rate,
				curr_date_time.hours,
				curr_date_time.minutes,
				curr_date_time.seconds
		);

		/* If we have reached one gallon drained or if the level of the tank is lower than 20%,
		 * turn off outbound pump and move to heating state
		 */
		if (time_reached || (sensorvalues.waterlevel_tank >= minimum_tank_val))
		{
			outbound_pump_low();

			current_state = HEATING_STATE;
			state_enter();
		}
	}
}

void heating_state(){
	char temp_res[6];
	char temp_tank[6];
	// Sample both temperature sensors
	sample_temperature_sensors();
	if (state_entry) {
		state_maintain();
		keypadIter = 1000;
		LCD_ShowHeatingScreen();
		// Delay for ensuring pumps don't shock PSU
		HAL_Delay(2000);
	}
	if (keypadIter > 999) {
		num_to_char_4(sensorvalues.temperature_res);
		temp_res[0] = XXdXX[0];
		temp_res[1] = XXdXX[1];
		temp_res[2] = XXdXX[2];
		temp_res[3] = XXdXX[3];
		temp_res[4] = XXdXX[4];
		temp_res[5] = '\0';
		num_to_char_4(sensorvalues.temperature_tank);
		temp_tank[0] = XXdXX[0];
		temp_tank[1] = XXdXX[1];
		temp_tank[2] = XXdXX[2];
		temp_tank[3] = XXdXX[3];
		temp_tank[4] = XXdXX[4];
		temp_tank[5] = '\0';
		LCD_ShowHeatingTemps(temp_res, temp_tank);
		keypadIter = 0;
	}
	// Calculate the difference between the two temperatures
	float diff = sensorvalues.temperature_res -
			sensorvalues.temperature_tank;
	/* If the difference between the two is greater than or equal to +1 Deg. F we will turn off
	 * heater & circulating pump, otherwise, we will turn them on / keep them on
	 * Also, we can change the diff value in the if statement to make up for
	 * the heating element still being hot after being turned off
	 */

	if (diff >= 0.0f && fabsf(diff) <= 2.0f)
	{
		heater_low();
		circulating_pump_low();

		current_state = AQUA_FILL_STATE;
		state_enter();
	}
	else
	{
		/* Turn on heater & circulating pump
		 * MCP23017 is setting PA0 High
		 */
		// Delay for ensuring pumps don't shock PSU
		HAL_Delay(2000);
		heater_high();
		circulating_pump_high();
	}
}

void water_fill_state(){
	char elapsed_buf[6];
	// Only perform these commands once


	if (state_entry)
	{
		state_maintain();
		keypadIter = 0;
		estTime = 0;

		// Set inbound pump & flag high
		// Delay for ensuring pumps don't shock PSU
		HAL_Delay(2000);
		LCD_ShowFillScreen(flow_rate);
		inbound_pump_high();
		inbound_pump_on = 1;
		fill_start_sod = get_seconds_of_day();
		state_maintain();
	}
	if (keypadIter > 165) {
		estTime += 0.1666f;
		num_to_char_4(estTime);
		elapsed_buf[0] = XXdXX[0];
		elapsed_buf[1] = XXdXX[1];
		elapsed_buf[2] = XXdXX[2];
		elapsed_buf[3] = XXdXX[3];
		elapsed_buf[4] = XXdXX[4];
		elapsed_buf[5] = '\0';
		LCD_ShowElapsedTime(elapsed_buf);
		keypadIter = 0;
	}
	// Compute time once per loop
	uint32_t curr_sod = get_seconds_of_day();

	uint32_t delta = (curr_sod >= fill_start_sod)
		? (curr_sod - fill_start_sod)
		: (86400u - fill_start_sod + curr_sod);

	// always update reference
	fill_start_sod = curr_sod;

	// only accumulate when pump is ON
	if (inbound_pump_on) {
		fill_elapsed += delta;
	}
	// Monitor temperatures while filling
	sample_temperature_sensors();

	float diff = sensorvalues.temperature_res - sensorvalues.temperature_tank;

	// If temp mismatch gets too large, pause filling and go back to heating
	if (diff < -1.0f)
	{
		overheat = 0;
		inbound_pump_low();
		inbound_pump_on = 0;
		current_state = HEATING_STATE;
		state_enter();
		return;
	}

	if(diff > 1.0f && overheat == 0){
		// Delay for ensuring pumps don't shock PSU
		HAL_Delay(2000);
		inbound_pump_low();
		inbound_pump_on = 0;
		overheat = 1;
		return;
	}
	if((diff < 1.0f) && overheat == 1){
		// Delay for ensuring pumps don't shock PSU
		HAL_Delay(2000);
		overheat = 0;
		inbound_pump_high();
		inbound_pump_on = 1;
		return;

	}
	// Continue fill checks
	read_water_level(&sensorvalues.waterlevel_res, &sensorvalues.waterlevel_tank);

	// Does the time elapsed now plus any previous pump time add to our flow rate?
	bool time_reached = (fill_elapsed >= flow_rate);

	if (time_reached || (sensorvalues.waterlevel_tank <= maximum_tank_val))
	{
		inbound_pump_low();
		inbound_pump_on = 0;
		fill_elapsed = 0; // reset for next water change

		current_state     = IDLE_STATE;
		state_enter();
		water_change_flag = 0;
		update_schedule();

		cooldown.cooldown_flag = 1;
		cooldown.cooldown_sod = get_seconds_of_day();
	}
}
