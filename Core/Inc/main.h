/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct{
	uint16_t turbidity_value;
	int waterlevel_tank;
	int waterlevel_res;
	volatile float temperature_res;
	volatile float temperature_tank;
	float ph_value;
}SensorValues;

typedef struct{
	bool cooldown_flag;
	uint32_t cooldown_sod;
}CooldownStruct;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOF
#define USART2_TX_Pin GPIO_PIN_2
#define USART2_TX_GPIO_Port GPIOA
#define USART2_RX_Pin GPIO_PIN_3
#define USART2_RX_GPIO_Port GPIOA
#define LED_GREEN_Pin GPIO_PIN_5
#define LED_GREEN_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define KEY_R4_Pin GPIO_PIN_9
#define KEY_R4_GPIO_Port GPIOC
#define KEY_C3_Pin GPIO_PIN_0
#define KEY_C3_GPIO_Port GPIOD
#define KEY_R2_Pin GPIO_PIN_1
#define KEY_R2_GPIO_Port GPIOD
#define KEY_R1_Pin GPIO_PIN_2
#define KEY_R1_GPIO_Port GPIOD
#define KEY_C4_Pin GPIO_PIN_3
#define KEY_C4_GPIO_Port GPIOD
#define KEY_C3D4_Pin GPIO_PIN_4
#define KEY_C3D4_GPIO_Port GPIOD
#define KEY_C2_Pin GPIO_PIN_5
#define KEY_C2_GPIO_Port GPIOD
#define KEY_C1_Pin GPIO_PIN_6
#define KEY_C1_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOF
#define USART2_TX_Pin GPIO_PIN_2
#define USART2_TX_GPIO_Port GPIOA
#define USART2_RX_Pin GPIO_PIN_3
#define USART2_RX_GPIO_Port GPIOA
#define LED_GREEN_Pin GPIO_PIN_5
#define LED_GREEN_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define KEY_R4_Pin GPIO_PIN_9
#define KEY_R4_GPIO_Port GPIOC
#define KEY_R3_Pin GPIO_PIN_0
#define KEY_R3_GPIO_Port GPIOD
#define KEY_R2_Pin GPIO_PIN_1
#define KEY_R2_GPIO_Port GPIOD
#define KEY_R1_Pin GPIO_PIN_2
#define KEY_R1_GPIO_Port GPIOD
#define KEY_C4_Pin GPIO_PIN_3
#define KEY_C4_GPIO_Port GPIOD
#define KEY_C3_Pin GPIO_PIN_4
#define KEY_C3_GPIO_Port GPIOD
#define KEY_C2_Pin GPIO_PIN_5
#define KEY_C2_GPIO_Port GPIOD
#define KEY_C1_Pin GPIO_PIN_6
#define KEY_C1_GPIO_Port GPIOD

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
