/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SERIAL_DEBUG_H
#define __SERIAL_DEBUG_H

#include <stdio.h>
#include <stdbool.h>



#define DEBUG_ERROR (0)
#define DEBUG_WARN (1)
#define DEBUG_INFO (2)
#define DEBUG_TRACE (3)
#define DEBUG_HIGH_TRACE (4)


#ifdef DEBUG
//отладка пишет в USART
#define COM_PORT_DEBUG

//отладка пишет в Serial Wire Debug (SWD)
//#define SWO_DEBUG

//если надо принудительно включить отладку в прошивке
#define MANUAL_DEBUG_ON 1

//отладка включается в прошивке програмной установкой
//#define MANUAL_DEBUG_ON 0



//#define MP_DEBUG_PRINT(VERBOSE_LEVEL,FMT...) printf(__VA_ARGS__)
//#define MP_DEBUG_PRINT(VERBOSE_LEVEL,FMT...) if (get_verbose_debug(VERBOSE_LEVEL)) printf(__VA_ARGS__)

#define MP_DEBUG_PRINT(VERBOSE_LEVEL, FMT...)                                                                          \
    if (get_verbose_debug(VERBOSE_LEVEL) && get_debug_comport())                                                       \
    printf(FMT)

#define MP_DEBUG_DUMP(VERBOSE_LEVEL, BUF, LEN)                                                                         \
    if (get_verbose_debug(VERBOSE_LEVEL) && get_debug_comport())                                                       \
    print_dump(BUF, LEN)


#endif

#if MANUAL_DEBUG_ON == 1

//раскоментировать для установки уровня отладки TRACE
#define get_verbose_debug(debug_level) (debug_level <= DEBUG_TRACE)
//раскоментировать для установки уровня отладки только ошибок
//#define get_verbose_debug(debug_level) (debug_level <= DEBUG_INFO)
//#define get_verbose_debug(debug_level) (debug_level<=DEBUG_WARN)
//#define get_verbose_debug(debug_level) (debug_level<=DEBUG_ERROR)

//принудительная постоянная установка отладки вручную
#define get_debug_comport() (true)

#else

//уровень отладки в зависимости от настроек
#define get_verbose_debug(debug_level) (debug_level <= config.otladka.verbose)
//вывод отладки в зависимости от настроек
#define get_debug_comport() (config.otladka.comport)

#endif

#ifndef DEBUG

#define MP_DEBUG_PRINT(...) (void) (0)
#define MP_DEBUG_DUMP(VERBOSE_LEVEL, BUF, LEN) (void) (0)

#endif



#endif //__SERIAL_DEBUG_H
