/*
 * my_debug.h
 *
 *  Created on: Jan 25, 2024
 *      Author: maiva
 */

#ifndef MY_DEBUG_H_
#define MY_DEBUG_H_

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define DEBUG_EN 1

#define APP_FRAME_START '@'
#define APP_FRAME_STOP '#'

typedef struct
{
	char char_data_rec;
	char arr_data_rec[1000];
	bool data_done_frame;
	uint16_t arr_data_index;
	bool en_get_char_data;
}Uart_debug;

extern Uart_debug my_debug;
extern Uart_debug esp32_uart;

void uartIntHandle(Uart_debug *myUart);
void uartIntClear(Uart_debug *myUart);

bool trim(char *source,char *des,char key_start[30],char key_stop[30]);
bool trim_with_length(char *source,char *des,char key_start[30],unsigned int length);
void user_application();
uint8_t delay_user(uint32_t t);
void debug_print(const char *fmt, ...);
void debugSendFrame(const char *fmt, ...);

#endif /* USER_MY_DEBUG_H_ */


