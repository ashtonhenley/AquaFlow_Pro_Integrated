/*
 * mcp23017.h
 *
 *  Created on: Feb 27, 2026
 *      Author: ashtonhenley
 */

#ifndef INC_MCP23017_H_
#define INC_MCP23017_H_

//
/*
 * TABLE FOR DETERMINING MCP23017 ADDRESS:
 * A2   A1   A0          BINARY       HEX ADDR
 * 0    0    0         0010 0000        0x20
 * 0    0    1         0010 0001        0x21
 * 0    1    0         0010 0010        0x22
 * 0    1    1         0010 0011        0x23
 * 1    0    0         0010 0100        0x24
 * 1    0    1		   0010 0101        0x25
 * 1    1    0		   0010 0110        0x26
 * 1    1    1         0010 0111        0x27
 *
 */

// Define all possible I2C addresses
#define MCP23017_ADDR_20 0x20
#define MCP23017_ADDR_21 0x21
#define MCP23017_ADDR_22 0x22
#define MCP23017_ADDR_23 0x23
#define MCP23017_ADDR_24 0x24
#define MCP23017_ADDR_25 0x25
#define MCP23017_ADDR_26 0x26
#define MCP23017_ADDR_27 0x27

#define REG_IODIRA_BANK0 0x00
#define REG_IODIRB_BANK0 0x01
#define REG_IPOLA_BANK0 0x02
#define REG_IPOLB_BANK0 0x03
#define REG_GPINTENA_BANK0 0x04
#define REG_GPINTENB_BANK0 0x05
#define REG_DEFVALA_BANK0 0x06
#define REG_DEFVALB_BANK0 0x07
#define REG_INTCONA_BANK0 0x08
#define REG_INTCONB_BANK0 0x09

#define REG_IOCON 0x0A
#define IOCON_DEFAULT_BITS 0x04

#define REG_GPPUA_BANK0 0x0C
#define REG_GPPUB_BANK0 0x0D
#define REG_INTFA_BANK0 0x0E
#define REG_INTFB_BANK0 0x0F
#define REG_INTCAPA_BANK0 0x10
#define REG_INTCAPB_BANK0 0x11
#define REG_GPIOA_BANK0 0x12
#define REG_GPIOB_BANK0 0x13
#define REG_OLATA_BANK0 0x14
#define REG_OLATB_BANK0 0x15

// Struct for I2C peripheral and address of MCP23017
typedef struct{
	I2C_HandleTypeDef *hi2c;
	uint8_t addr;
}MCP23017_HandleTypeDef;

HAL_StatusTypeDef MCP23017_IO_CONTROL(MCP23017_HandleTypeDef *htd, uint8_t IO_CONTROLBYTEA, uint8_t IO_CONTROLBYTEB);
HAL_StatusTypeDef MC23017_INPUT_POLARITY(MCP23017_HandleTypeDef *htd, uint8_t POLARITY_BYTEA, uint8_t POLARITY_BYTEB);
HAL_StatusTypeDef MC23017_IOCON_CONFIG(MCP23017_HandleTypeDef *htd);
HAL_StatusTypeDef MC23017_GPINTEN_CONFIG(MCP23017_HandleTypeDef *htd, uint8_t CONFIG_BYTEA, uint8_t CONFIG_BYTEB);
HAL_StatusTypeDef MC23017_DEFVAL_CONFIG(MCP23017_HandleTypeDef *htd, uint8_t CONFIG_BYTEA, uint8_t CONFIG_BYTEB);
HAL_StatusTypeDef MC23017_INTCON_CONFIG(MCP23017_HandleTypeDef *htd, uint8_t CONFIG_BYTEA, uint8_t CONFIG_BYTEB);
HAL_StatusTypeDef MC23017_GPPU_CONFIG(MCP23017_HandleTypeDef *htd, uint8_t CONFIG_BYTEA, uint8_t CONFIG_BYTEB);
HAL_StatusTypeDef MC23017_INTF_READ(MCP23017_HandleTypeDef *htd, uint8_t *FLAGSA, uint8_t *FLAGSB);
HAL_StatusTypeDef MC23017_INTCAP_READ(MCP23017_HandleTypeDef *htd, uint8_t FLAGS[2]);
HAL_StatusTypeDef MC23017_GPIO_WRITE(MCP23017_HandleTypeDef *htd, uint8_t gpioa_bits, uint8_t gpiob_bits);
HAL_StatusTypeDef MC23017_GPIO_READ(MCP23017_HandleTypeDef *htd, uint8_t gpio_bits[2]);
HAL_StatusTypeDef MC23017_OLAT_WRITE(MCP23017_HandleTypeDef *htd, uint8_t olata_bits, uint8_t olatb_bits);
HAL_StatusTypeDef MC23017_OLAT_READ(MCP23017_HandleTypeDef *htd, uint8_t olat_bits[2]);
HAL_StatusTypeDef MCP23017_Init(MCP23017_HandleTypeDef *htd);
#endif /* INC_MCP23017_H_ */
