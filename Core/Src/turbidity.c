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

    float ntu_f = 5.0f * ((float)adc - 2140.0f);

    // Clamp range
    if (ntu_f < 0.0f) ntu_f = 0.0f;
    if (ntu_f > 100.0f) ntu_f = 100.0f;

    *turbidity = (uint16_t)(ntu_f + 0.5f);
}