/*
 * mcp23017.c
 *
 *  Created on: Feb 27, 2026
 *      Author: ashtonhenley
 */
#include <stdint.h>
#include "main.h"
#include "mcp23017.h"

extern I2C_HandleTypeDef hi2c1;

MCP23017_HandleTypeDef htd={
	.hi2c = &hi2c1,
	.addr = (MCP23017_ADDR_20 << 1)
};


static HAL_StatusTypeDef MCP23017_WriteAB(MCP23017_HandleTypeDef *htd, uint8_t reg, uint8_t a, uint8_t b){
	uint8_t data[2] = {a, b};
    return HAL_I2C_Mem_Write(htd->hi2c,htd->addr, reg, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY);
}
static HAL_StatusTypeDef MCP23017_WriteA(MCP23017_HandleTypeDef *htd, uint8_t reg, uint8_t a){
	uint8_t data[1] = {a};
    return HAL_I2C_Mem_Write(htd->hi2c,htd->addr, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

/* I/O Direction Control, set bit to 1 for input, 0 as output.
 * Byte setup: IO7 IO6 IO5 ... IO0
*/
HAL_StatusTypeDef MCP23017_IO_CONTROL(MCP23017_HandleTypeDef *htd, uint8_t IO_CONTROLBYTEA, uint8_t IO_CONTROLBYTEB){
	return MCP23017_WriteAB(htd, REG_IODIRA_BANK0, IO_CONTROLBYTEA, IO_CONTROLBYTEB);
}

/* Polarity Control. Only affects a pin that is taking input. 1 will reflect opposite logic, 0 will reflect same logic
 * Same byte setup as I/O: IO7 ... IO0
 */
HAL_StatusTypeDef MC23017_INPUT_POLARITY(MCP23017_HandleTypeDef *htd, uint8_t POLARITY_BYTEA, uint8_t POLARITY_BYTEB){
	return MCP23017_WriteAB(htd, REG_IPOLA_BANK0, POLARITY_BYTEA, POLARITY_BYTEB);
}

/* IOCONFIG Control, this uses the default IOCON setup I've chosen. This makes the BANK bit 0.
 * Everything else is 0, except for the ODR bit, which will set open-drain output.
 */
HAL_StatusTypeDef MC23017_IOCON_CONFIG(MCP23017_HandleTypeDef *htd){
	return MCP23017_WriteA(htd, REG_IOCON, IOCON_DEFAULT_BITS);
}

/* Interrupt on change: 1 on a bit will enable GPIO input pin for interrupt-on-change event
 * Same byte setup as usual *** NOTE: The DEFVAL and INTCON registers must also be configured ***
 */
HAL_StatusTypeDef MC23017_GPINTEN_CONFIG(MCP23017_HandleTypeDef *htd, uint8_t CONFIG_BYTEA, uint8_t CONFIG_BYTEB){
	return MCP23017_WriteAB(htd, REG_GPINTENA_BANK0, CONFIG_BYTEA, CONFIG_BYTEB);
}

/* Default compare register: The default comparison value is configured in the DEFVAL register.
 * If enabled, to compare against the DEFVAL register, an opposite value on the associated pin will cause an interrupt to occur.
 */
HAL_StatusTypeDef MC23017_DEFVAL_CONFIG(MCP23017_HandleTypeDef *htd, uint8_t CONFIG_BYTEA, uint8_t CONFIG_BYTEB){
	return MCP23017_WriteAB(htd, REG_DEFVALA_BANK0, CONFIG_BYTEA, CONFIG_BYTEB);
}

/* Interrupt control register: if 1, pin value is compared against the associated bit in the DEFVAL register, otherwise, pin value
 * is compared against the previous pin value.
 */
HAL_StatusTypeDef MC23017_INTCON_CONFIG(MCP23017_HandleTypeDef *htd, uint8_t CONFIG_BYTEA, uint8_t CONFIG_BYTEB){
	return MCP23017_WriteAB(htd, REG_INTCONA_BANK0, CONFIG_BYTEA, CONFIG_BYTEB);
}

/* Pull-Up Resistor configuration register: controls the pull-up resistors for the port pins
 * Only works for pins set as inputs
 *  1 means pull-up enabled, disabled otherwise.
 */
HAL_StatusTypeDef MC23017_GPPU_CONFIG(MCP23017_HandleTypeDef *htd, uint8_t CONFIG_BYTEA, uint8_t CONFIG_BYTEB){
	return MCP23017_WriteAB(htd, REG_GPPUA_BANK0, CONFIG_BYTEA, CONFIG_BYTEB);
}

/* Interrupt flag register: The INTF register reflects the interrupt condition on the port pins of any pin
 * that is enabled for interrupts via the GPINTEN register. A set bit indicated associated pin caused the interrupt.
 */
HAL_StatusTypeDef MC23017_INTF_CONFIG(MCP23017_HandleTypeDef *htd, uint8_t FLAGS[2]){
	return HAL_I2C_Mem_Read(htd->hi2c, htd->addr, REG_INTFA_BANK0, I2C_MEMADD_SIZE_8BIT, FLAGS, 2, HAL_MAX_DELAY);
}

/* The INTCAP register captures the GPIO port value at the time the interrupt occurred. The register is
* read-only and is updated only when an interrupt occurs. The register remains unchanged until the
* interrupt is cleared via a read of INTCAP or GPIO.
*
* 1 is logic-high, 0 is  logic-low
*/
HAL_StatusTypeDef MC23017_INTCAP_CONFIG(MCP23017_HandleTypeDef *htd, uint8_t FLAGS[2]){
	return HAL_I2C_Mem_Read(htd->hi2c, htd->addr, REG_INTCAPA_BANK0, I2C_MEMADD_SIZE_8BIT, FLAGS, 2, HAL_MAX_DELAY);
}
/* The GPIO register reflects the value on the port.
 * Reading from this register reads the port. Writing to this register modifies the Output Latch (OLAT) register.
 */
HAL_StatusTypeDef MC23017_GPIO_WRITE(MCP23017_HandleTypeDef *htd, uint8_t gpioa_bits, uint8_t gpiob_bits){
	return MCP23017_WriteAB(htd, REG_GPIOA_BANK0, gpioa_bits, gpiob_bits);
}

HAL_StatusTypeDef MC23017_GPIO_READ(MCP23017_HandleTypeDef *htd, uint8_t gpio_bits[2]){
	return HAL_I2C_Mem_Read(htd->hi2c, htd->addr, REG_GPIOA_BANK0, I2C_MEMADD_SIZE_8BIT, gpio_bits, 2, HAL_MAX_DELAY);
}
/* The OLAT register provides access to the output latches. A read from this register results in
 * a read of the OLAT and not the port itself. A write to  this register modifies the output latches that
 * modifies the pins configured as outputs.
 * 1 is logic-high, 0 is logic-low
 *
 */
HAL_StatusTypeDef MC23017_OLAT_READ(MCP23017_HandleTypeDef *htd, uint8_t olat_bits[2]){
	return HAL_I2C_Mem_Read(htd->hi2c, htd->addr, REG_OLATA_BANK0, I2C_MEMADD_SIZE_8BIT, olat_bits, 2, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MC23017_OLAT_WRITE(MCP23017_HandleTypeDef *htd, uint8_t olata_bits, uint8_t olatb_bits){
	return MCP23017_WriteAB(htd, REG_OLATA_BANK0, olata_bits, olatb_bits);
}


/* To create an initialization function, we will do the following:
 * 1) Initialize IOCON Register w/ Default Bits for project
 * 2) Set all GPIO Pins as outputs
 * 3) Set all GPIO Pins LOW By Defualt
 * 4) Disable all Pull-Up Resistors
 * 5) Disable all interrupts
 */
HAL_StatusTypeDef MCP23017_Init(MCP23017_HandleTypeDef *htd)
{
    HAL_StatusTypeDef ret;

    // Is I2C Device Ready?
    ret = HAL_I2C_IsDeviceReady(htd->hi2c, htd->addr, 3, 100);
    if (ret != HAL_OK) return ret;

    // 1) Configure IOCON
    ret = MC23017_IOCON_CONFIG(htd);
    if (ret != HAL_OK) return ret;

    // 4) Set all pins as outputs
    ret = MCP23017_IO_CONTROL(htd, 0x00, 0x00);
    if (ret != HAL_OK) return ret;

    // 3) Configure GPIO
    ret =  MC23017_OLAT_WRITE(htd, 0x00, 0x00);
    if (ret != HAL_OK) return ret;

    // 4) Disable pullups
    ret = MC23017_GPPU_CONFIG(htd, 0x00, 0x00);
    if (ret != HAL_OK) return ret;

    // 5) Disable interrupts
    ret = MC23017_GPINTEN_CONFIG(htd, 0x00, 0x00);
    if (ret != HAL_OK) return ret;

    return HAL_OK;
}
