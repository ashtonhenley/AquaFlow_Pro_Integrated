/*
 * DS18B20.c
 *
 *  Created on: Nov 7, 2025
 *      Author: ashtonhenley
 */



#include "main.h"
#include "DS18B20.h"
#include <stdint.h>

extern SensorValues sensorvalues;
extern UART_HandleTypeDef huart1;   // huart1 is defined in main.c
extern UART_HandleTypeDef huart3;
// HAL-only helper to change baud
uint8_t DS18B20_Init(UART_HandleTypeDef *huart)
{
    uint8_t ResetByte = 0xF0, PresenceByte;


    UART_SetBaud(9600, huart); // Set to 9600 while sending reset pulse
    // Send reset pulse (0xF0)
    HAL_UART_Transmit(huart, &ResetByte, 1, 1);
    // Wait for the presence pulse
    HAL_UART_Receive(huart, &PresenceByte, 1, 1);
    UART_SetBaud(115200, huart); // All other commands are sent over 115200
    // Check presence pulse
    if (PresenceByte != ResetByte){
        return 1; // Presence pulse detected
    }
    else{
        return 0; // No presence pulse detected
    }
}

uint8_t DS18B20_ReadBit(UART_HandleTypeDef *huart)
{
    uint8_t ReadBitCMD = 0xFF;
    uint8_t RxBit;

    // Send Read Bit CMD
    HAL_UART_Transmit(huart, &ReadBitCMD, 1, 1);
    // Receive The Bit
    HAL_UART_Receive(huart, &RxBit, 1, 1);

    return (RxBit & 0x01);
}

uint8_t DS18B20_ReadByte(UART_HandleTypeDef *huart)
{
    uint8_t RxByte = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        RxByte >>= 1;
        if (DS18B20_ReadBit(huart))
        {
            RxByte |= 0x80;
        }
    }
    return RxByte;
}

void DS18B20_WriteByte(uint8_t data, UART_HandleTypeDef *huart)
{
    uint8_t TxBuffer[8];
    for (int i=0; i<8; i++)
    {
      if ((data & (1<<i)) != 0){
          TxBuffer[i] = 0xFF;
      }
      else{
          TxBuffer[i] = 0;
      }
    }
    HAL_UART_Transmit(huart, TxBuffer, 8, 10);
}

void DS18B20_SampleTemp(UART_HandleTypeDef *huart)
{
    DS18B20_Init(huart);
    DS18B20_WriteByte(0xCC, huart);  // Skip ROM   (ROM-CMD)
    DS18B20_WriteByte(0x44, huart);  // Convert T  (F-CMD)
}

float DS18B20_ReadTemp(UART_HandleTypeDef *huart)
{
    uint8_t Temp_LSB, Temp_MSB;
    uint16_t Temp;
    float Temperature;
    DS18B20_Init(huart);
    DS18B20_WriteByte(0xCC, huart);  // Skip ROM         (ROM-CMD)
    DS18B20_WriteByte(0xBE, huart);  // Read Scratchpad  (F-CMD)
    Temp_LSB = DS18B20_ReadByte(huart);
    Temp_MSB = DS18B20_ReadByte(huart);
    Temp = ((Temp_MSB<<8))|Temp_LSB;
    Temperature = (float)Temp*0.1125 + 32; // Temp/16 is value in celsius. (9/5)/16 = 0.1125 (for fahrenheit)
    return Temperature;
}

void UART_SetBaud(uint32_t baud, UART_HandleTypeDef *huart)
{
    HAL_UART_DeInit(huart);
    huart->Init.BaudRate = baud;
    HAL_HalfDuplex_Init(huart);

}
void sample_temperature_sensors(){
	DS18B20_SampleTemp(&huart1);
	sensorvalues.temperature_res = DS18B20_ReadTemp(&huart1);

	DS18B20_SampleTemp(&huart3);
	sensorvalues.temperature_tank = DS18B20_ReadTemp(&huart3);
}
