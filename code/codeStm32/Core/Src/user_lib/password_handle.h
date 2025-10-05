#ifndef PASSWORD_HANDLE_H
#define PASSWORD_HANDLE_H

#include "keypad.h"
#include "lcd_i2c.h"


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdarg.h>

#include "servo.h"

#define lcd_str lcd_string
#define _clear lcd_clear()
#define lcd_chr lcd_char
#define delay delay_ms
extern char kp;
extern uint32_t id;
extern char tam[20];
extern char PASS_DEFAULT[16];
extern char phone_number[20];
extern bool simOpen;
extern char passSim[20];

extern char rfid_code1[20];
extern char rfid_code2[20];
extern char rfid_code3[20];

#define THE_DUNG 1
#define THE_SAI 2
#define RFID_IDE 0
extern char rfid_valid;

#define backspace 'A'
#define enter 'D'

void man_hinh_cho();
char set_key();
void doi_pass();
void dong_cua();
void mo_cua();
void nhap_pass();
void nhap_phim();
char check_id();
void old_pass();
void nhap_phim_cu();
void sim_manager();


#endif























