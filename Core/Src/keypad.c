// keypad.c: library for debounced, interrupt-based, "column-staggered" interfacing with a keypad (Adafruit 3844) via an STM32G0-series microcontroller

#include <stdint.h>
#include "keypad.h"
#include "main.h"

#define DEBOUNCED_COUNT 5 // Debouncing: a key ij of row i and column j is accepted when i is stably detected over N successive iterations

// Array of ordered key values (column-major order)
static const char keys[16] = { '1', '4', '7', '*', '2', '5', '8', '0', '3', '6', '9', '#', 'A', 'B', 'C', 'D' };

static uint8_t activeCol = 0; // To be initialized
static uint8_t lastRowStatesByCol[] = { 0, 0, 0, 0 }; // Last 4 "column iterations worth" of row states (an iteration's row states constitute a 4-bit code: r4 down to r1)
static uint8_t stableDetections[] = { 0, 0, 0, 0 }; // By column

static char debouncedKey = 0; // Default value 0 used to indicate either a lack of keys pressed (this "lack" is still sufficiently debounced) or a recent read(/discard) of debouncedKey.
// The latter always guarantees a "rising edge" in the accepting of the next key that, if desired, can be checked within the given higher-level API

// To be used to pull high only the column corresponding to parameter col (for the purposes of this library, invariably activeCol)
static void pullCol(uint8_t col)
{
    HAL_GPIO_WritePin(KEY_C1_GPIO_Port, KEY_C1_Pin, (col == 0));
    HAL_GPIO_WritePin(KEY_C2_GPIO_Port, KEY_C2_Pin, (col == 1));
    HAL_GPIO_WritePin(KEY_C3_GPIO_Port, KEY_C3_Pin, (col == 2));
    HAL_GPIO_WritePin(KEY_C4_GPIO_Port, KEY_C4_Pin, (col == 3));
}

// Yields a 4-bit code storing the states of r4 "down to" r1
static uint8_t pollRows(void)
{
    return HAL_GPIO_ReadPin(KEY_R1_GPIO_Port, KEY_R1_Pin) |
           (HAL_GPIO_ReadPin(KEY_R2_GPIO_Port, KEY_R2_Pin) << 1) |
           (HAL_GPIO_ReadPin(KEY_R3_GPIO_Port, KEY_R3_Pin) << 2) |
           (HAL_GPIO_ReadPin(KEY_R4_GPIO_Port, KEY_R4_Pin) << 3);
}

// When called prior to the starting of the time base of the "iteration interrupt," allows an iteration's worth of time for lines to settle as needed before iterations begin (hence, "initialization").
// Allows for the specification of the activeCol to start with (1 thru 4 for c1-c4)
void Keypad_Init(uint8_t col)
{
    activeCol = (col - 1) & 0x03; // "& 0x03" limits values to the range [0, 3] as a precaution (equivalently, take (col - 1) % 4)
    pullCol(activeCol);
}

// The iterative "per-column" detection routine - when sufficiently debounced, maps the (detected) key ID either to the corresponding key or to 0 in the default ("no key") case
void Keypad_Iterate(void)
{
    uint8_t rows = pollRows(); // Row states for activeCol (the current iteration)

    // Detection and debouncing
    if (rows == lastRowStatesByCol[activeCol]) {
    	if (stableDetections[activeCol] < DEBOUNCED_COUNT) stableDetections[activeCol]++;
    	else if (rows) {
    		for (int i = 0; i < 4; i++) {
    			if (rows & (1 << i)) {
    				debouncedKey = keys[4*activeCol + i]; // Accept "highest-priority" key ("from r1 upwards")
    				break;
    			}
    		}
    	}
    	else debouncedKey = 0; // Default case otherwise
    }
    else {
    	stableDetections[activeCol] = 0;
    	lastRowStatesByCol[activeCol] = rows; // Overwrite row states for column activeCol's last iteration (redundant assignment in the case that rows and lastRowStatesByCol[activeCol] match)
    }

    // Prepare for next iteration
    activeCol = (activeCol + 1) & 0x03;
    pullCol(activeCol); // The pulling of the next iteration's activeCol at the end of the current one implements "built-in" delay necessary for the debouncing mechanism
}

// Read(/discard) the current accepted key
char Keypad_ReadDebouncedKeyPress(void)
{
    char k = debouncedKey;
    debouncedKey = 0; // When the accepted debounced key is read, return debouncedKey to default (if the debounced key remains pressed, this is immediately undone next iteration, but the "falling edge" nonetheless occurs)
    return k;
}

// Getter function to obtain debouncedKey's state without performing a formal read(/discard) (for debugging)
char Keypad_DebugReadDebouncedKeyPress(void)
{
	return debouncedKey;
}
