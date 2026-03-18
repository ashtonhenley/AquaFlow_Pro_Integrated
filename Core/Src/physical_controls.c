/*
 * physical_controls.c
 *
 *  Created on: Mar 6, 2026
 *      Author: ashtonhenley
 */

#include "physical_controls.h"
#include "main.h"
#include "mcp23017.h"
extern MCP23017_HandleTypeDef htd;

void outbound_pump_high(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
}
void outbound_pump_low(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
}
void inbound_pump_high(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}
void inbound_pump_low(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}
void heater_high(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}
void heater_low(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
}
void fan_high(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
}
void fan_low(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
}
void circulating_pump_high(){
	MC23017_OLAT_WRITE(&htd, 0x01, 0x00);
}
void circulating_pump_low(){
	MC23017_OLAT_WRITE(&htd, 0x00, 0x00);
}
