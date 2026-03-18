#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_
#include <stdint.h>
#include "main.h"

void Keypad_Init(uint8_t col);
void Keypad_Iterate(void);
char Keypad_ReadDebouncedKeyPress(void);
char Keypad_DebugReadDebouncedKeyPress(void);

#endif // INC_KEYPAD_H_
