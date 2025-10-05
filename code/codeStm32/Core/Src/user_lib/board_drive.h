/*
 * board_drive.h
 *
 *  Created on: Sep 24, 2025
 *      Author: maiva
 */

#ifndef _BOARD_DRIVE_H_
#define _BOARD_DRIVE_H_

#include "main.h"

#define on 1
#define off 0

#define buzz(stt) HAL_GPIO_WritePin(BUZZ_GPIO_Port , BUZZ_Pin, stt)
#define rl1(stt) HAL_GPIO_WritePin(RL1_GPIO_Port , RL1_Pin, stt)
#define rl2(stt) HAL_GPIO_WritePin(RL2_GPIO_Port , RL2_Pin, stt)
#define rl3(stt) HAL_GPIO_WritePin(RL3_GPIO_Port , RL3_Pin, stt)
#define lamp1(stt) HAL_GPIO_WritePin(LAMP1_GPIO_Port , LAMP1_Pin, stt)
#define lamp2(stt) HAL_GPIO_WritePin(LAMP2_GPIO_Port , LAMP2_Pin, stt)



#endif /* SRC_USER_LIB_BOARD_DRIVE_H_ */
