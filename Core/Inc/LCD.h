#ifndef INC_LCD_H_
#define INC_LCD_H_
#include <stdint.h>
#include <string.h>
#include "main.h"

#define LCD_ADDR 0x50
#define PREFIX 0xFE
#define DISPLAY_ON 0x41
#define DISPLAY_OFF 0x42
#define UNDERLINE_ON 0x47
#define UNDERLINE_OFF 0x48
#define BLINK_ON 0x4B
#define BLINK_OFF 0x4C
#define SET_CONTRAST 0x52
#define SET_BRIGHTNESS 0x53
#define SET_CURSOR 0x45
#define CURSOR_HOME 0x46
#define BACKSPACE 0x4E
#define CLEAR 0x51

void LCD_SendCommand(uint8_t cmd);
void LCD_SetContrast(uint8_t value);
void LCD_SetBrightness(uint8_t value);
void LCD_SetCursor(uint8_t pos);
void LCD_Init(void);
void LCD_WriteCharacter(char c);
void LCD_WriteString(char* str);
void LCD_ShowIdleOverview(char* tank_temp, char* turbidity, char* ph_value);
void LCD_ShowIdleCurrentTime(uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes);
void LCD_ShowIdleScheduledTime(uint8_t month, uint8_t day, uint8_t hours);
void LCD_ShowReservoirCheckScreen(void);
void LCD_ShowReservoirProgress(char* progress);
void LCD_ShowDrainScreen(uint8_t estimate_seconds);
void LCD_ShowHeatingScreen(void);
void LCD_ShowHeatingTemps(char* reservoir_temp, char* tank_temp);
void LCD_ShowFillScreen(uint8_t estimate_seconds);
void LCD_ShowElapsedTime(char* elapsed_time);

#endif // INC_LCD_H_
