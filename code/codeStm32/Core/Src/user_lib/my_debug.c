/*
 * my_debug.c
 *
 *  Created on: Jan 25, 2024
 *      Author: maiva
 */

#include "my_debug.h"
#include "gpio.h"
#include "usart.h"

Uart_debug my_debug=
{
	.char_data_rec=0,
	.arr_data_rec={0},
	.data_done_frame=false,
	.arr_data_index=0,
	.en_get_char_data=false
};

Uart_debug esp32_uart=
{
	.char_data_rec=0,
	.arr_data_rec={0},
	.data_done_frame=false,
	.arr_data_index=0,
	.en_get_char_data=false
};

void uartIntHandle(Uart_debug *myUart)
{
	if(myUart->char_data_rec==APP_FRAME_STOP)
	{
		myUart->en_get_char_data=false;
		myUart->data_done_frame=true;
	}
	if(myUart->en_get_char_data)
	{
		myUart->arr_data_rec[myUart->arr_data_index]=myUart->char_data_rec;
		myUart->arr_data_index++;
	}
	if(myUart->char_data_rec==APP_FRAME_START) myUart->en_get_char_data=true;
}

void uartIntClear(Uart_debug *myUart)
{
	myUart->char_data_rec=0;
	myUart->data_done_frame=false;
	myUart->en_get_char_data=false;
	memset(myUart->arr_data_rec,0,sizeof(myUart->arr_data_rec));
	myUart->arr_data_index=0;
}

void debug_print(const char *fmt, ...)
{
	#ifdef DEBUG_EN
		static char buffer1[256];
		unsigned int len=0;
		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer1, sizeof(buffer1), fmt, args);
		va_end(args);
		len=strlen(buffer1);
		//HAL_UART_Transmit(&huart3,"@",1,100);
		HAL_UART_Transmit(&huart3,buffer1,len,100);
		//HAL_UART_Transmit(&huart3,"#",1,100);
	#endif
}
//---------------------------------------------------------------------

bool trim(char *source,char *des,char key_start[30],char key_stop[30])
{
    unsigned int start_id=0,stop_id=0;
    if(strstr(source,key_start)==0) return false;
    if(strstr(source,key_stop)==0) return false;
    memset(des,0,strlen(des));
    start_id=strstr(source,key_start)+strlen(key_start)-source;
    stop_id=strstr(source+start_id,key_stop)-source;
    //printf("start_id: %d\n",start_id);
    //printf("stop_id: %d\n",stop_id);
    if(start_id>stop_id) return false;
    for(int i=start_id;i<stop_id;i++) des[i-start_id]=source[i];
    return true;
}

bool trim_with_length(char *source,char *des,char key_start[30],unsigned int length)
{
    unsigned int start_id=0,stop_id=0;
    if(strstr(source,key_start)==0) return false;
    //if(strstr(source,key_stop)==0) return false;
    memset(des,0,strlen(des));
    start_id=strstr(source,key_start)+strlen(key_start)-source;
    stop_id=start_id+length;
    //printf("start_id: %d\n",start_id);
    //printf("stop_id: %d\n",stop_id);
    if(start_id>stop_id) return false;
    for(int i=start_id;i<stop_id;i++) des[i-start_id]=source[i];
    return true;
}

uint8_t delay_user(uint32_t t)
{
	while(t>0)
	{
		t--;
		HAL_Delay(1);
	}

	return 1;
}









