/*
 * waterlevel_us.c
 *
 *  Created on: Mar 13, 2026
 *      Author: ashtonhenley
 */
#include "waterlevel_us.h"
#include "stdint.h"
#include "stddef.h"
#include "main.h"

const uint8_t HEADER_BYTE = 0xFF;
const uint16_t READ_TIMEOUT_MS = 200;
const int ERROR_DISTANCE = -1;  // Error return value

extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;

size_t readN(UART_HandleTypeDef *huart, uint8_t *buf, size_t len)
{
    size_t offset = 0;
    uint32_t start = HAL_GetTick();

    while (offset < len)
    {
        if (HAL_UART_Receive(huart, &buf[offset], 1, 10) == HAL_OK)
        {
            offset++;
        }

        if ((HAL_GetTick() - start) > READ_TIMEOUT_MS)
        {
            break;
        }
    }

    return offset;
}



int wl_get_distance(UART_HandleTypeDef *huart){

    uint8_t data[4];
    uint8_t byte;
    uint32_t start = HAL_GetTick();

    while(HAL_GetTick() - start < READ_TIMEOUT_MS){

        if(readN(huart, &byte, 1) == 1){

            if(byte == HEADER_BYTE){

                data[0] = byte;

                if(readN(huart, &data[1], 3) == 3){

                    uint8_t checksum = (data[0] + data[1] + data[2]) & 0xFF;

                    if(checksum == data[3]){
                        return (data[1] << 8) | data[2];
                    }
                }
            }
        }
    }

    return ERROR_DISTANCE;
}

void read_water_level(int *waterlevel_res, int *waterlevel_tank){
	int tmp_wl_res = wl_get_distance(&huart5);
	int tmp_wl_tank = wl_get_distance(&huart6);

	// Keep last valid value
	if(tmp_wl_res != ERROR_DISTANCE){
		*waterlevel_res = tmp_wl_res / 10; // value in cm
	}

	if(tmp_wl_tank != ERROR_DISTANCE){
		*waterlevel_tank = tmp_wl_tank / 10; // value in cm
	}


}
