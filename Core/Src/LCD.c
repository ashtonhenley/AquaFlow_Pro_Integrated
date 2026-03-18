// LCD.c: rudimentary library for I2C communication with an LCD (NHD-0420D3Z) via an STM32G0-series microcontroller

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "LCD.h"
#include "main.h"

// Provide as an extern the I2C handle TO BE USED (application-dependent - could be hi2cX for X = 1, 2, etc.)
extern I2C_HandleTypeDef hi2c1;

#define LCD_ADDR 0x50 // 8-bit I2C device address (default = 0x50 - for the purposes of this library, not to be changed "mid-application")

// - Commands -
// Delays not listed are negligible (500 us or less; usually 100 us)

// For initialization
#define PREFIX 0xFE // Precedes all commands
#define DISPLAY_ON 0x41
#define DISPLAY_OFF 0x42
#define UNDERLINE_ON 0x47 // 1.5 ms
#define UNDERLINE_OFF 0x48 // 1.5 ms
#define BLINK_ON 0x4B 
#define BLINK_OFF 0x4C
#define SET_CONTRAST 0x52 // Value between 1 and 50; default is 40
#define SET_BRIGHTNESS 0x53 // Value between 1 (0%) and 8 (100%); default is 8

// For "standard use" (only pertinent commands are defined)
#define SET_CURSOR 0x45 // Specified cursor location is a byte corresponding to a column (C1-C20) on a certain line:
/* 1: 0x00 - 0x13
 * 2: 0x40 - 0x53
 * 3: 0x14 - 0x27
 * 4: 0x54 - 0x67 */
#define CURSOR_HOME 0x46 // To R1C1; 1.5 ms
#define BACKSPACE 0x4E // Moves the cursor to the previous column (to the left) and deletes the character on that column
#define CLEAR 0x51 // Clears the screen and moves the cursor to R1C1. 1.5 ms
#define DEG 0xDF

static void LCD_Write12Hour(uint8_t hours) {
	uint8_t hr = hours % 12u;
	char hr_buf[3];

	if (hr == 0u) {
		LCD_WriteString("12");
		return;
	}

	snprintf(hr_buf, sizeof(hr_buf), "%02u", hr);
	LCD_WriteString(hr_buf);
}

 // - Functions -

void LCD_SendCommand(uint8_t cmd) {
	uint8_t buf[2] = { PREFIX, cmd };
	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, buf, 2, HAL_MAX_DELAY);
	if (cmd == UNDERLINE_ON || cmd == UNDERLINE_OFF || cmd == CURSOR_HOME || cmd == CLEAR) HAL_Delay(2);
}

void LCD_SetContrast(uint8_t value) {
	uint8_t buf[3] = { PREFIX, SET_CONTRAST, value };
	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, buf, 3, HAL_MAX_DELAY);
}

void LCD_SetBrightness(uint8_t value) {
	uint8_t buf[3] = { PREFIX, SET_BRIGHTNESS, value };
	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, buf, 3, HAL_MAX_DELAY);
}

// Use a "simplified" argument pos with a value on the range [0, 79] - to be converted to the corresponding value within the given line's DDRAM address range
void LCD_SetCursor(uint8_t pos) {

	uint8_t convertedPos = pos;

	// Line 1: no change (0 thru 19 = 0x00 thru 0x13)
	// Line 2: map 20 thru 39 to 0x40 thru 0x53 (i.e. offset to 0x40 (= 64): 64 - 20 = 44)
	if (pos >= 20 && pos <= 39) convertedPos += 44;
	// Line 3: map 40 thru 59 to 0x14 thru 0x27 (i.e. offset to 0x14 (= 20): 20 - 40 = -20)
	if (pos >= 40 && pos <= 59) convertedPos -= 20;
	// Line 4: map 60 thru 79 to 0x54 thru 0x67 (i.e. offset to 0x54 (= 84): 84 - 60 = 24)
	if (pos >= 60 && pos <= 79) convertedPos += 24;

	uint8_t buf[3] = { PREFIX, SET_CURSOR, convertedPos };
	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, buf, 3, HAL_MAX_DELAY);
}

// Initialization
void LCD_Init(void) {

	HAL_Delay(105); // If initialization is to be done just after power-on, wait (at least) 100 ms before issuing any commands

	// Arbitrary specified starting contrast/brightness combination
	LCD_SetContrast(30);
	LCD_SetBrightness(5);

	LCD_SendCommand(DISPLAY_ON);
	LCD_SendCommand(CLEAR);
	LCD_SetCursor(0x00); // Set cursor (to 0x00 = R1C1) prior to sending of text so that the DDRAM address doesn't remain undefined (following power-on)

}



void LCD_WriteCharacter(char c) {
	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, (uint8_t*)&c, 1, HAL_MAX_DELAY);
}

void LCD_WriteString(const char* str) {
	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

void LCD_ShowIdleOverview(const char* tank_temp, const char* turbidity, const char* ph_value) {
	LCD_SendCommand(CLEAR);

	LCD_WriteString("TMPMAIN   ");
	LCD_WriteString(tank_temp);
	LCD_WriteCharacter(' ');
	LCD_WriteCharacter(DEG);
	LCD_WriteCharacter('F');

	LCD_SetCursor(20);
	LCD_WriteString("TRBDTY    ");
	LCD_WriteString(turbidity);
	LCD_WriteString(" NTU");

	LCD_SetCursor(40);
	LCD_WriteString("PH        ");
	LCD_WriteString(ph_value);
}

void LCD_ShowIdleCurrentTime(uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes) {
	char buf[3];

	LCD_SetCursor(60);
	LCD_WriteString("   ");

	snprintf(buf, sizeof(buf), "%02u", month);
	LCD_WriteString(buf);
	LCD_WriteCharacter('/');

	snprintf(buf, sizeof(buf), "%02u", day);
	LCD_WriteString(buf);
	LCD_WriteCharacter(' ');

	LCD_Write12Hour(hours);
	LCD_WriteCharacter(':');

	snprintf(buf, sizeof(buf), "%02u", minutes);
	LCD_WriteString(buf);
	LCD_WriteCharacter(' ');
	LCD_WriteString((hours < 12u) ? "AM" : "PM");
}

void LCD_ShowIdleScheduledTime(uint8_t month, uint8_t day, uint8_t hours) {
	char buf[3];

	LCD_SetCursor(60);
	LCD_WriteString("SCHEDULED ");

	snprintf(buf, sizeof(buf), "%02u", month);
	LCD_WriteString(buf);
	LCD_WriteCharacter('/');

	snprintf(buf, sizeof(buf), "%02u", day);
	LCD_WriteString(buf);
	LCD_WriteCharacter(' ');

	LCD_Write12Hour(hours);
	LCD_WriteString((hours < 12u) ? "AM" : "PM");
}

void LCD_ShowReservoirCheckScreen(void) {
	LCD_SendCommand(CLEAR);
	LCD_WriteString("CHANGING: RESCHECK");
	LCD_SetCursor(20);
	LCD_WriteString("TARGET    1 gal");
}

void LCD_ShowReservoirProgress(const char* progress) {
	LCD_SetCursor(40);
	LCD_WriteString("PROGRESS  ");
	LCD_WriteString(progress);
	LCD_WriteCharacter('%');
}

void LCD_ShowDrainScreen(uint8_t estimate_seconds) {
	char buf[4];

	LCD_SendCommand(CLEAR);
	LCD_WriteString("CHANGING: DRAIN");
	LCD_SetCursor(20);
	LCD_WriteString("TARGET    1 gal");
	LCD_SetCursor(40);
	LCD_WriteString("ESTIMATE  ");
	snprintf(buf, sizeof(buf), "%u", estimate_seconds);
	LCD_WriteString(buf);
	LCD_WriteString(" sec");
	LCD_SetCursor(60);
	LCD_WriteString("ELAPSED         sec");
}

void LCD_ShowHeatingScreen(void) {
	LCD_SendCommand(CLEAR);
	LCD_WriteString("CHANGING: HEATING");
	LCD_SetCursor(20);
	LCD_WriteString("TEMPRES          ");
	LCD_WriteCharacter(DEG);
	LCD_WriteCharacter('F');
	LCD_SetCursor(40);
	LCD_WriteString("TEMPMAIN        ");
	LCD_WriteCharacter(DEG);
	LCD_WriteCharacter('F');
}

void LCD_ShowHeatingTemps(const char* reservoir_temp, const char* tank_temp) {
	LCD_SetCursor(30);
	LCD_WriteString(reservoir_temp);
	LCD_SetCursor(50);
	LCD_WriteString(tank_temp);
}

void LCD_ShowFillScreen(uint8_t estimate_seconds) {
	char buf[4];

	LCD_SendCommand(CLEAR);
	LCD_WriteString("CHANGING: FILL");
	LCD_SetCursor(20);
	LCD_WriteString("TARGET    1 gal");
	LCD_SetCursor(40);
	LCD_WriteString("ESTIMATE  ");
	snprintf(buf, sizeof(buf), "%u", estimate_seconds);
	LCD_WriteString(buf);
	LCD_WriteString(" sec");
	LCD_SetCursor(60);
	LCD_WriteString("ELAPSED         sec");
}

void LCD_ShowElapsedTime(const char* elapsed_time) {
	LCD_SetCursor(70);
	LCD_WriteString(elapsed_time);
}
