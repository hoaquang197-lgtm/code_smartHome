/*
 * i2c_lcd.h
 *
 *  Created on: May 18, 2022
 *      Author: maiva
 */

#ifndef LCD_I2C_H_
#define LCD_I2C_H_
/**
Edit by modify: Ngoc Hang
**/

#include "stm32f1xx_hal.h"
#include "i2c.h"

//#define lcd_char lcd_send_data
//#define lcd_string lcd_send_string
#define lcd_xy lcd_goto_XY



void lcd_init (void);   // initialize lcd
void lcd_send_cmd (char cmd);  // send command to the lcd
void lcd_send_data (char data);  // send data to the lcd
void lcd_send_string (char *str);  // send string to the lcd
void lcd_clear (void);	//clear display lcd
void lcd_goto_XY (int row, int col); //set proper location on screen

void lcd_string(char hang,char cot,char *str);
void lcd_char(char hang, char cot,char dat);


#endif /* SRC_I2C_LCD_H_ */
