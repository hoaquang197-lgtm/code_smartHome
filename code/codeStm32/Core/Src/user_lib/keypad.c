#include "keypad.h"

unsigned char read_address_key()
{
  unsigned char value=0;
  out1(C1_GPIO_Port, C1_Pin);
  out1(C2_GPIO_Port, C2_Pin);
  out1(C3_GPIO_Port, C3_Pin);
  out1(C4_GPIO_Port, C4_Pin);


  delay_ms(delay_read);
  //cot 1
  out0(C1_GPIO_Port, C1_Pin); delay_ms(5);
  if(!r1) {while(!r1);  return value=1;} //hang 1
  if(!r2) {while(!r2);  return value=5;} //hang 2
  if(!r3) {while(!r3);  return value=9;} //hang 3
  if(!r4) {while(!r4);  return value=13;} //hang 4
  out1(C1_GPIO_Port, C1_Pin);

  delay_ms(delay_read);

  //cot 2
  out0(C2_GPIO_Port, C2_Pin); HAL_Delay(5);
  if(!r1) {while(!r1);  return value=2;} //hang 1
  if(!r2) {while(!r2);  return value=6;} //hang 2
  if(!r3) {while(!r3);  return value=10;} //hang 3
  if(!r4) {while(!r4);  return value=14;} //hang 4
  out1(C2_GPIO_Port, C2_Pin);


  delay_ms(delay_read);
  //cot 3
  out0(C3_GPIO_Port, C3_Pin); delay_ms(5);
  if(!r1) {while(!r1);  return value=3;} //hang 1
  if(!r2) {while(!r2);  return value=7;} //hang 2
  if(!r3) {while(!r3);  return value=11;} //hang 3
  if(!r4) {while(!r4);  return value=15;} //hang 4
  out1(C3_GPIO_Port, C3_Pin);


  delay_ms(delay_read);
  //cot 4
  out0(C4_GPIO_Port, C4_Pin); delay_ms(5);
  if(!r1) {while(!r1);  return value=4;} //hang 1
  if(!r2) {while(!r2);  return value=8;} //hang 2
  if(!r3) {while(!r3);  return value=12;} //hang 3
  if(!r4) {while(!r4);  return value=16;} //hang 4
  out1(C4_GPIO_Port, C4_Pin);

  delay_ms(delay_read);
  return 0;
}


/*
*  ham ma hoa toa do phim ra ki tu va gui qua uart
*/
char read_key()
{
  char phim=0;
  char dat=read_address_key();
  switch(dat)
  {
     case 1: phim='1';  break;
     case 2: phim='2';  break;
     case 3: phim='3';  break;
     case 4: phim='A';  break;

     case 5: phim='4';  break;
     case 6: phim='5';  break;
     case 7: phim='6';  break;
     case 8: phim='B';  break;

     case 9:  phim='7';  break;
     case 10: phim='8';  break;
     case 11: phim='9';  break;
     case 12: phim='C';  break;

     case 13: phim='*';  break;
     case 14: phim='0';  break;
     case 15: phim='#';  break;
     case 16: phim='D';  break;

     default: break;
  }
  return phim;
}





