/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SERIAL_DEBUG_H
#define __SERIAL_DEBUG_H

#include <stdbool.h>
#include <stdio.h>

#define DEBUG_ERROR (0)
#define DEBUG_WARN (1)
#define DEBUG_INFO (2)
#define DEBUG_TRACE (3)
#define DEBUG_HIGH_TRACE (4)

#if !defined(__HAL_ENTER_CRITICAL_SECTION)
#define __HAL_ENTER_CRITICAL_SECTION()                                                                                 \
    uint32_t PriMsk;                                                                                                   \
    PriMsk = __get_PRIMASK();                                                                                          \
    __set_PRIMASK(1);
#endif
#if !defined(__HAL_EXIT_CRITICAL_SECTION)
#define __HAL_EXIT_CRITICAL_SECTION() __set_PRIMASK(PriMsk);
#endif

#if !defined(__HAL_LOCAL_ENTER_CRITICAL_SECTION)
//секция запрета прерывания для повторного использования в функциях, без объявления uint32_t PriMsk;
#define __HAL_LOCAL_ENTER_CRITICAL_SECTION()                                                                           \
    PriMsk = __get_PRIMASK();                                                                                          \
    __set_PRIMASK(1);
#endif

#if !defined(critical_stop)
//в рабочей версии сделать нормальный обработчик критических ошибок!!!!
#define critical_stop() Error_Handler()
//#define critical_stop() (void) (0)
#endif


#ifdef DEBUG
//write debug to USART
#define COM_PORT_DEBUG

//write debug to Serial Wire Debug (SWD)
//#define SWO_DEBUG

//If you need to force debugging in the firmware
#define MANUAL_DEBUG_ON 1

//Debugging is enabled in the firmware by a software setting, for example, a variable that is written to the EEPROM
//#define MANUAL_DEBUG_ON 0

void flush_debug();

//output debug information in poll mode
#define PRINTF_UART_POLL 1
//output debug information in irq mode
#define PRINTF_UART_IRQ 2

//#define PRINTF_UART PRINTF_UART_POLL
#define PRINTF_UART PRINTF_UART_IRQ


//прерывания UART1 очень низкий приоритет для вывода отладочной информации
#define USARTx_INT_PRIORITY ((uint32_t) 3)    /// USART1 interrupt priority

#define USARTx USART1
#define USARTx_CLK_ENABLE() __HAL_RCC_USART1_CLK_ENABLE();
#define USARTx_CLK_DISABLE() __HAL_RCC_USART1_CLK_DISABLE();
#define USARTx_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_Deinit() HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN)

#define USARTx_FORCE_RESET() __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET() __HAL_RCC_USART1_RELEASE_RESET()
#define USARTx_IRQn USART1_IRQn

/* Definition for USARTx Pins */
#define USARTx_TX_PIN GPIO_PIN_9
#define USARTx_TX_GPIO_PORT GPIOA
#define USARTx_TX_AF GPIO_AF1_USART1
#define USARTx_RX_PIN GPIO_PIN_10
#define USARTx_RX_GPIO_PORT GPIOA
#define USARTx_RX_AF GPIO_AF1_USART1

#define UartDebug huart1

#define MP_DEBUG_PRINT(VERBOSE_LEVEL, FMT...)                                                                          \
    if (get_verbose_debug(VERBOSE_LEVEL) && get_debug_comport())                                                       \
    printf(FMT)

#define MP_DEBUG_DUMP(VERBOSE_LEVEL, BUF, LEN)                                                                         \
    if (get_verbose_debug(VERBOSE_LEVEL) && get_debug_comport())                                                       \
    print_dump(BUF, LEN)


#endif

#if MANUAL_DEBUG_ON == 1

//uncomment to set debug level to TRACE
#define get_verbose_debug(debug_level) (debug_level <= DEBUG_TRACE)
//раскоментировать для установки уровня отладки только ошибок
//#define get_verbose_debug(debug_level) (debug_level <= DEBUG_INFO)
//#define get_verbose_debug(debug_level) (debug_level<=DEBUG_WARN)
//#define get_verbose_debug(debug_level) (debug_level<=DEBUG_ERROR)

//Force permanent set of debugging manually
#define get_debug_comport() (true)

#else

//debug level depending on the variable stored in the settings
#define get_verbose_debug(debug_level) (debug_level <= config.otladka.verbose)
//Is debug output necessary depending on the variable's value.
//By setting this variable to 0, you can programmatically disable all debug output.
#define get_debug_comport() (config.otladka.comport)

#endif

#ifndef DEBUG

#define MP_DEBUG_PRINT(...) (void) (0)
#define MP_DEBUG_DUMP(VERBOSE_LEVEL, BUF, LEN) (void) (0)

#endif

#endif    //__SERIAL_DEBUG_H
