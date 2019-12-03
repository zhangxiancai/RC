/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define RTC_ASYNCH_PREDIV    0x7F
#define RTC_SYNCH_PREDIV     0xF9  /* 32Khz/128 - 1 */


#define BASE_ADDR         0X08037300  //激活标志位
#define SAVE_SIZE  100
uint8_t save_buf[SAVE_SIZE];//
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
/*
 * PCR102  4层板
 */
//#define PCR102  //节点
#ifdef  PCR102

#define A111_SPI_SS_Pin GPIO_PIN_12
#define A111_SPI_SS_GPIO_Port GPIOB

#define A111_ENABLE_Pin GPIO_PIN_8
#define A111_ENABLE_GPIO_Port GPIOC
#define A111_SENSOR_INTERRUPT_Pin GPIO_PIN_11
#define A111_SENSOR_INTERRUPT_GPIO_Port GPIOB


#define Power_1_8_EN_Pin GPIO_PIN_6
#define Power_1_8_EN_GPIO_Port GPIOA

#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOC
/*
 * PCR103  2层板
 */
#else

#define A111_SPI_SS_Pin GPIO_PIN_6
#define A111_SPI_SS_GPIO_Port GPIOC
#define A111_ENABLE_Pin GPIO_PIN_7
#define A111_ENABLE_GPIO_Port GPIOC
#define A111_SENSOR_INTERRUPT_Pin GPIO_PIN_12
#define A111_SENSOR_INTERRUPT_GPIO_Port GPIOB

#define Power_1_8_EN_Pin GPIO_PIN_1
#define Power_1_8_EN_GPIO_Port GPIOA


#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOC

#endif


#define A111_SPI_SCK_Pin GPIO_PIN_13
#define A111_SPI_SCK_GPIO_Port GPIOB
#define A111_SPI_MISO_Pin GPIO_PIN_14
#define A111_SPI_MISO_GPIO_Port GPIOB
#define A111_SPI_MOSI_Pin GPIO_PIN_15
#define A111_SPI_MOSI_GPIO_Port GPIOB



#define SPI_SS_ON   HAL_GPIO_WritePin(A111_SPI_SS_GPIO_Port, A111_SPI_SS_Pin, GPIO_PIN_SET);
#define SPI_SS_OFF  HAL_GPIO_WritePin(A111_SPI_SS_GPIO_Port, A111_SPI_SS_Pin, GPIO_PIN_RESET);

#define A111_ENABLE_ON   HAL_GPIO_WritePin(A111_ENABLE_GPIO_Port, A111_ENABLE_Pin, GPIO_PIN_SET);
#define A111_ENABLE_OFF  HAL_GPIO_WritePin(A111_ENABLE_GPIO_Port, A111_ENABLE_Pin, GPIO_PIN_RESET);

#define CLK_ON   HAL_GPIO_WritePin(A111_SPI_SCK_GPIO_Port, A111_SPI_SCK_Pin, GPIO_PIN_SET);
#define CLK_OFF  HAL_GPIO_WritePin(A111_SPI_SCK_GPIO_Port, A111_SPI_SCK_Pin, GPIO_PIN_RESET);

#define MISO_ON   HAL_GPIO_WritePin(A111_SPI_MISO_GPIO_Port, A111_SPI_MISO_Pin, GPIO_PIN_SET);
#define MISO_OFF  HAL_GPIO_WritePin(A111_SPI_MISO_GPIO_Port, A111_SPI_MISO_Pin, GPIO_PIN_RESET);

#define MOSI_ON   HAL_GPIO_WritePin(A111_SPI_MOSI_GPIO_Port, A111_SPI_MOSI_Pin, GPIO_PIN_SET);
#define MOSI_OFF  HAL_GPIO_WritePin(A111_SPI_MOSI_GPIO_Port, A111_SPI_MOSI_Pin, GPIO_PIN_RESET);

/* USER CODE BEGIN Private defines */
#define LED1_ON   HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
#define LED1_tog  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
#define LED1_OFF  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

#define LED2_ON   HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
#define LED2_OFF  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
#define LED2_tog  HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

#define POWER_ON   HAL_GPIO_WritePin(Power_1_8_EN_GPIO_Port, Power_1_8_EN_Pin, GPIO_PIN_SET);
#define POWER_OFF	 HAL_GPIO_WritePin(Power_1_8_EN_GPIO_Port, Power_1_8_EN_Pin, GPIO_PIN_RESET);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
