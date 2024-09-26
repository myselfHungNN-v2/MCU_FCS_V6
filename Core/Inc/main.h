/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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
#define Button_9_Pin GPIO_PIN_13
#define Button_9_GPIO_Port GPIOC
#define Button_8_Pin GPIO_PIN_14
#define Button_8_GPIO_Port GPIOC
#define A0_Button_1_Pin GPIO_PIN_15
#define A0_Button_1_GPIO_Port GPIOC
#define E0_Button_1_Pin GPIO_PIN_0
#define E0_Button_1_GPIO_Port GPIOD
#define A1_Button_1_Pin GPIO_PIN_1
#define A1_Button_1_GPIO_Port GPIOD
#define A2_Button_1_Pin GPIO_PIN_0
#define A2_Button_1_GPIO_Port GPIOC
#define E1_1_Pin GPIO_PIN_1
#define E1_1_GPIO_Port GPIOC
#define ADC_Out_17_Pin GPIO_PIN_0
#define ADC_Out_17_GPIO_Port GPIOA
#define ADC_Out_16_Pin GPIO_PIN_1
#define ADC_Out_16_GPIO_Port GPIOA
#define Sig_ADC_Vol_Pin GPIO_PIN_2
#define Sig_ADC_Vol_GPIO_Port GPIOA
#define S2_ADC_Pin GPIO_PIN_3
#define S2_ADC_GPIO_Port GPIOA
#define S3_ADC_Pin GPIO_PIN_4
#define S3_ADC_GPIO_Port GPIOA
#define S0_ADC_Pin GPIO_PIN_5
#define S0_ADC_GPIO_Port GPIOA
#define S1_ADC_Pin GPIO_PIN_6
#define S1_ADC_GPIO_Port GPIOA
#define Allow_Fire_Pin GPIO_PIN_7
#define Allow_Fire_GPIO_Port GPIOA
#define Mode_Fire_Pin GPIO_PIN_4
#define Mode_Fire_GPIO_Port GPIOC
#define S0_Fire_Pin GPIO_PIN_5
#define S0_Fire_GPIO_Port GPIOC
#define S1_Fire_Pin GPIO_PIN_0
#define S1_Fire_GPIO_Port GPIOB
#define S3_Fire_Pin GPIO_PIN_1
#define S3_Fire_GPIO_Port GPIOB
#define S2_Fire_Pin GPIO_PIN_2
#define S2_Fire_GPIO_Port GPIOB
#define InHiBit_Fire_Pin GPIO_PIN_11
#define InHiBit_Fire_GPIO_Port GPIOB
#define Fire_17_Pin GPIO_PIN_12
#define Fire_17_GPIO_Port GPIOB
#define Fire_16_Pin GPIO_PIN_13
#define Fire_16_GPIO_Port GPIOB
#define DATA_Pin GPIO_PIN_14
#define DATA_GPIO_Port GPIOB
#define EN__Pin GPIO_PIN_15
#define EN__GPIO_Port GPIOB
#define CLK__Pin GPIO_PIN_6
#define CLK__GPIO_Port GPIOC
#define LED_16_Pin GPIO_PIN_7
#define LED_16_GPIO_Port GPIOC
#define LED_17_Pin GPIO_PIN_8
#define LED_17_GPIO_Port GPIOC
#define WRN_Pin GPIO_PIN_9
#define WRN_GPIO_Port GPIOC
#define DE_485_1_Pin GPIO_PIN_8
#define DE_485_1_GPIO_Port GPIOA
#define A0_Button_2_Pin GPIO_PIN_15
#define A0_Button_2_GPIO_Port GPIOA
#define E0_Button_2_Pin GPIO_PIN_10
#define E0_Button_2_GPIO_Port GPIOC
#define A1_Button_2_Pin GPIO_PIN_11
#define A1_Button_2_GPIO_Port GPIOC
#define A2_Button_2_Pin GPIO_PIN_12
#define A2_Button_2_GPIO_Port GPIOC
#define E1_2_Pin GPIO_PIN_2
#define E1_2_GPIO_Port GPIOD
#define Sensor_17_Pin GPIO_PIN_3
#define Sensor_17_GPIO_Port GPIOB
#define Sensor_16_Pin GPIO_PIN_4
#define Sensor_16_GPIO_Port GPIOB
#define S3_Sensor_Pin GPIO_PIN_5
#define S3_Sensor_GPIO_Port GPIOB
#define S2_Sensor_Pin GPIO_PIN_6
#define S2_Sensor_GPIO_Port GPIOB
#define Sig_Sensor_Pin GPIO_PIN_7
#define Sig_Sensor_GPIO_Port GPIOB
#define S1_Sensor_Pin GPIO_PIN_8
#define S1_Sensor_GPIO_Port GPIOB
#define S0_Sensor_Pin GPIO_PIN_9
#define S0_Sensor_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
