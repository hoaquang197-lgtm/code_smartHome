/*
 * app.h
 *
 *  Created on: Sep 27, 2025
 *      Author: maiva
 */

#ifndef _APP_H_
#define _APP_H_

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "mk_dht11.h"
#include "servo.h"

#define lamp3(value)  TIM3_SetDuty_CH1(value)
#define lamp4(value)  TIM3_SetDuty_CH2(value)

void delay_us (uint16_t us);
void door(char vi_tri);

#define OPEN 1
#define CLOSE 2

// ==== Tuỳ chỉnh theo servo & cơ khí ====
#define SERVO_MIN_ANGLE   0.0f     // (độ)
#define SERVO_MAX_ANGLE   180.0f   // (độ)
#define SERVO_MIN_US      500      // xung ở góc min (µs)  thường 500..1000
#define SERVO_MAX_US      2500     // xung ở góc max (µs)  thường 2000..2500
#define SERVO_PERIOD_US   20000    // chu kỳ ~20 ms
#define SERVO_PULSES      10       // lặp vài chu kỳ để giữ vị trí

// Tuỳ cơ khí của bạn: CLOSE/OPEN ứng với góc nào
#define ANGLE_CLOSE  90.0f
#define ANGLE_OPEN   0.0f

void TIM3_ConfigFrequency(uint32_t f_pwm_hz);
void PWM_Start(void);
void TIM3_SetDuty_CH1(uint8_t duty);
void TIM3_SetDuty_CH2(uint8_t duty);
void esp32_send(const char *fmt, ...);
size_t build_sensor_json(uint16_t gas_ppm,
                         uint16_t light,
                         uint8_t  hum,
                         uint8_t  temp,
                         char *out,
                         size_t out_size);
bool esp32handle();

extern uint32_t adc_mq2[64];
extern uint8_t lampGuestVal;
extern uint8_t lampSleepVal;
extern float BH1750_lux;
extern dht11_t dht;

extern char sdt_sos[20];
extern char countSendSms;

#define GAS_LIMIT 2500

#endif /* SRC_USER_LIB_APP_H_ */
