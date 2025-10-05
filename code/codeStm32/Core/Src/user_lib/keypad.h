#ifndef KEYPAD_H
#define KEYPAD_H

#include "stm32f1xx_hal.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdarg.h>

#include "gpio.h"

#define delay_ms HAL_Delay
#define delay_read 5  //thoi gian tre quet phim
#define on 1
#define off 0

#define out0(port,pin) HAL_GPIO_WritePin(port,pin,0)
#define out1(port,pin) HAL_GPIO_WritePin(port,pin,1)

//dinh nghia cac cot ban phim
#define r1 HAL_GPIO_ReadPin(R1_GPIO_Port, R1_Pin)
#define r2 HAL_GPIO_ReadPin(R2_GPIO_Port, R2_Pin)
#define r3 HAL_GPIO_ReadPin(R3_GPIO_Port, R3_Pin)
#define r4 HAL_GPIO_ReadPin(R4_GPIO_Port, R4_Pin)

unsigned char read_address_key();
char read_key();




#endif

