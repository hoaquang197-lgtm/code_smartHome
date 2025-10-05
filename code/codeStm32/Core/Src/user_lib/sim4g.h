/*
 * sim4g.h
 *
 *  Created on: Sep 30, 2022
 *      Author: Mai Ba
 */

#ifndef __SIM4G_H_
#define __SIM4G_H_

#include "stm32f1xx_hal.h"
#include "usart.h"
#include "gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "stdbool.h"

typedef struct
{
    char brocker[100];
    char port[10];
    char qos[1];
    char user[100];
    char pass[100];
    char topic[200];
    char id[100];
    char message[300];
}ec20_mqtt;

extern bool SimIsBusy;

#define BUSY 0
#define IDLE 1

void sim4g_print(const char *fmt, ...);
bool ec200_mqtt_publish(ec20_mqtt *client);
void sim4g_write(char c);
bool sim4g_send_sms(char* sdt, char* noi_dung);
bool sim4g_waiting_feedback(char feedback[100],uint16_t timeOut);
bool sim4g_call(char *sdt);
uint8_t sim4g_read_signalStrength(uint16_t *signalValue);
bool CheckSimBusy(uint16_t timeOut);
void GSM_SendString(char *s);


#endif
