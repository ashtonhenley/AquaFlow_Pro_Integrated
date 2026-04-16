/*
 * turbidity.c
 *
 *  Created on: Jan 15, 2026
 *      Author: ashtonhenley
 */
#include "turbidity.h"
#include "math.h"

#include "turbidity.h"

extern volatile uint16_t adc_buffer[2];

void check_turbidity(uint16_t *turbidity)
{
    uint16_t adc = adc_buffer[0];

    // Multiply first to preserve precision
    int32_t ntu = (100 * (2200 - (int32_t)adc)) / 600;

    // Clamp
    if (ntu < 0) ntu = 0;
    if (ntu > 100) ntu = 100;

    *turbidity = (uint16_t)ntu;
}