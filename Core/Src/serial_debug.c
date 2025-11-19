//#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h>    // STDOUT_FILENO, STDERR_FILENO

#include "main.h"
#include "serial_debug.h"


#ifdef SWO_DEBUG
//overwrite printf() output to send via ITM (SWO)
int _write(int file, char* data, int len) {
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }

    for (int i = 0; i < len; i++) {
        ITM_SendChar(data[i]);
    }

    // return # of bytes written - as best we can tell
    return len;
}
#endif

#if defined(COM_PORT_DEBUG)
/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
//in putty Enable the "Implicit CR in every LF" option https://the.earth.li/~sgtatham/putty/0.60/htmldoc/Chapter4.html#i27
//отладил режим прерываний в выводе отладки
//#define PRINTF_POLL_UART
#if PRINTF_UART == PRINTF_UART_POLL
#include "usart.h"

int _write(int file, char* data, int len) {
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }
    HAL_UART_Transmit(&UartDebug, (uint8_t*) data, (uint16_t) len, 0xFFFF);
    return len;
}
#endif

#if PRINTF_UART == PRINTF_UART_IRQ
#include "usart.h"
//переменная означает, что идёт передача отладочной информации по прерываниям
//необходимо дождаться окончания передачи блока
//переменная используется для запуска передачи информации в очереди,
//если она появилась в процессе работы
//переменная нужна, чтобы не перезапустить передачу заново, если она уже идёт,
//но ещё не закончилась
//переходит в false по прерыванию окончания передачи буфера
volatile bool debug_uart_run = false;
//указатели на начало и конец кольцевого буфера
volatile uint16_t pos_print = 0;
volatile uint16_t pos_debug = 0;

uint8_t debug_buffer[2048];

void UART_Debug_Transmit(UART_HandleTypeDef* huart) {
        debug_uart_run = false;
        flush_debug();
        return true;
}

void flush_debug() {
    if (debug_uart_run)
        return;
    if (pos_print != pos_debug) {
        //int len_for_send;
        volatile bool fast_quit = false;
        if (pos_print <= pos_debug) {    //нормальный ход
            __HAL_ENTER_CRITICAL_SECTION();
            int len_for_send = pos_debug - pos_print;
            int pos_for_send = pos_print;
            if (len_for_send == 0) {
                fast_quit = true;
            } else {
                pos_print += len_for_send;
            }

            __HAL_EXIT_CRITICAL_SECTION();
            if (fast_quit)
                return;

            if ((pos_for_send + len_for_send) > sizeof(debug_buffer)) {
                critical_stop();
            }
            //идёт передача отладочной информации по прерываниям
            debug_uart_run = true;
            HAL_UART_Transmit_IT(&UartDebug, &debug_buffer[pos_for_send], len_for_send);
        } else {
            //отправим до конца массива и сдвигаем указатель на ноль
            __HAL_ENTER_CRITICAL_SECTION();
            int len_for_send = sizeof(debug_buffer) - pos_print;
            int pos_for_send = pos_print;
            pos_print = 0;
            __HAL_EXIT_CRITICAL_SECTION();

            if ((pos_for_send + len_for_send) > sizeof(debug_buffer)) {
                critical_stop();
            }
            ////идёт передача отладочной информации по прерываниям
            debug_uart_run = true;
            HAL_UART_Transmit_IT(&UartDebug, &debug_buffer[pos_for_send], len_for_send);
        }
    }
}

int _write(int file, char* data, int len) {
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }
    int len_for_write = 0;
    int total_len = len;

    if ((pos_debug + len) >= sizeof(debug_buffer)) {
        __HAL_ENTER_CRITICAL_SECTION();
        int pos_for_write = pos_debug;
        int len_for_write = sizeof(debug_buffer) - pos_debug;
        pos_debug = 0;
        __HAL_EXIT_CRITICAL_SECTION();
        len -= len_for_write;
        memcpy(&debug_buffer[pos_for_write], data, len_for_write);
    }

    if (len > sizeof(debug_buffer)) {
        len = sizeof(debug_buffer);
    }

    __HAL_ENTER_CRITICAL_SECTION();
    int pos_for_write = pos_debug;
    len_for_write = len;
    pos_debug += len;
    if (pos_debug >= sizeof(debug_buffer)) {
        __HAL_EXIT_CRITICAL_SECTION();
        critical_stop();
        pos_debug = 0;
    }
    __HAL_EXIT_CRITICAL_SECTION();

    memcpy((char*) &debug_buffer[pos_for_write], data, len_for_write);

    if (!debug_uart_run) {
        flush_debug();
    }

    return total_len;
}

#endif

//отладочный дамп памяти
//на входе указатель начала области памяти и его длина, которую функция распечатает в отладку
void print_dump(uint8_t* buf, uint16_t len) {
    uint16_t offset = 0;
    while (len) {
        printf("%04x ", offset);
        for (uint8_t x = 0; x < (len > 16 ? 16 : len); x++) {
            printf("%02x ", *buf++);
        }
        len -= len > 16 ? 16 : len;
        offset += 16;
        printf("\n");
    }
}
#endif
