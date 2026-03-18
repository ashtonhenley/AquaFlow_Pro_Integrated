/*
 * DS18B20.h
 *
 *  Created on: Nov 7, 2025
 *      Author: ashtonhenley
 */

#include <stdint.h>

#ifndef SRC_DS18B20_H_
#define SRC_DS18B20_H_

uint8_t DS18B20_Init(UART_HandleTypeDef *huart);
uint8_t DS18B20_ReadBit(UART_HandleTypeDef *huart);
uint8_t DS18B20_ReadByte(UART_HandleTypeDef *huart);
void    DS18B20_WriteByte(uint8_t data, UART_HandleTypeDef *huart);
void    DS18B20_SampleTemp(UART_HandleTypeDef *huart);
float   DS18B20_ReadTemp(UART_HandleTypeDef *huart);
void UART_SetBaud(uint32_t baud, UART_HandleTypeDef *huart);

void sample_temperature_sensors();

#endif /* SRC_DS18B20_H_ */
