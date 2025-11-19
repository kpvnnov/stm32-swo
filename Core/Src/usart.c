/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
void UART_Debug_Transmit(UART_HandleTypeDef* huart);
/* USER CODE END 0 */

UART_HandleTypeDef UartDebug;

/* USART1 init function */

void MX_USART1_UART_Init(void) {

    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    UartDebug.Instance = USARTx;
    UartDebug.Init.BaudRate = 115200;
    UartDebug.Init.WordLength = UART_WORDLENGTH_8B;
    UartDebug.Init.StopBits = UART_STOPBITS_1;
    UartDebug.Init.Parity = UART_PARITY_NONE;
    UartDebug.Init.Mode = UART_MODE_TX_RX;
    UartDebug.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartDebug.Init.OverSampling = UART_OVERSAMPLING_16;
    UartDebug.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    UartDebug.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_HalfDuplex_Init(&UartDebug) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART1_Init 2 */
    HAL_UART_RegisterCallback(&UartDebug, HAL_UART_TX_COMPLETE_CB_ID, UART_Debug_Transmit);


    /* USER CODE END USART1_Init 2 */
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (uartHandle->Instance == &UartDebug) {
        /* USER CODE BEGIN USARTx_MspInit 0 */

        /* USER CODE END USARTx_MspInit 0 */
        /* USARTx clock enable */
        USARTx_CLK_ENABLE();

        USARTx_TX_GPIO_CLK_ENABLE();
        /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    */
        GPIO_InitStruct.Pin = USARTx_TX_PIN;
        GPIO_InitStruct.Mode = USARTx_TX_AF;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = USARTx_TX_AF;
        HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USARTx_IRQn, USARTx_INT_PRIORITY, 0);
        HAL_NVIC_EnableIRQ(USARTx_IRQn);
        /* USER CODE BEGIN USART1_MspInit 1 */

        /* USER CODE END USART1_MspInit 1 */
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle) {

    if (uartHandle->Instance == USARTx) {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        USARTx_CLK_DISABLE();

        /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    */
        USARTx_TX_GPIO_Deinit();

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USARTx_IRQn);
        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
