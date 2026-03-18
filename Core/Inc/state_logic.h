/*
 * state_logic.h
 *
 *  Created on: Mar 5, 2026
 *      Author: ashtonhenley
 */
#include <stdint.h>
#ifndef INC_STATE_LOGIC_H_
#define INC_STATE_LOGIC_H_
void state_enter();
void state_leave();

uint32_t get_seconds_of_day(void);
void idle_state();
void water_res_state();
void water_drain_state();
void heating_state();
void water_fill_state();
void num_to_char_2(uint8_t num);
void num_to_char_4(float num);

#endif /* INC_STATE_LOGIC_H_ */
