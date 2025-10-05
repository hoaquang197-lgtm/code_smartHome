/*
 * sim4g.c
 *
 *  Created on: Sep 30, 2022
 *      Author: Mai Ba
 */


#include "sim4g.h"

bool SimIsBusy=false;

void sim4g_print(const char *fmt, ...)
{
  static char buffer1[256];
  unsigned int len=0;
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer1, sizeof(buffer1), fmt, args);
  va_end(args);
  len=strlen(buffer1);
  HAL_UART_Transmit(&huart1,buffer1,len,100);
}

void sim4g_write(char c)
{
	uint8_t buff[1]={c};
	HAL_UART_Transmit(&huart1,buff,1,100);
}

bool ec200_mqtt_publish(ec20_mqtt *client)
{

//    if(ec20_at_cmd("+QMTOPEN","AT+QMTOPEN=0,\"%s\",%s\r\n",client->brocker,client->port))
//        esp8266_print("Access brocker ok\n");
//    else esp8266_print("Access brocker false\n");

	sim4g_print("AT+QMTOPEN=0,\"%s\",%s\r\n",client->brocker,client->port);
    //rs485_print("AT+QMTOPEN=0,\"%s\",%s\r\n",client->brocker,client->port);
    osDelay(400);

//    if(ec20_at_cmd("+QMTCONN","AT+QMTCONN=0,\"Mai_ba\",\"%s\",\"%s\"\r\n",client->user,client->pass))
//        esp8266_print("Subcribe Id ok\n");
//    else esp8266_print("Subcribe Id false\n");

    sim4g_print("AT+QMTCONN=0,\"Mai_ba\",\"%s\",\"%s\"\r\n",client->user,client->pass);
    //rs485_print("AT+QMTCONN=0,\"Mai_ba\",\"%s\",\"%s\"\r\n",client->user,client->pass);
    osDelay(200);

    //send topic
    sim4g_print("AT+QMTPUBEX=0,1,1,0,\"%s\",%d\r\n",client->topic,strlen(client->message));
    //rs485_print("AT+QMTPUBEX=0,1,1,0,\"%s\",%d\r\n",client->topic,strlen(client->message));
    osDelay(50);
    //send message
    sim4g_print("%s\r\n",client->message);
    //rs485_print("%s\r\n",client->message);

    sim4g_print("AT+QMTDISC=0\r\n");
    //rs485_print("AT+QMTDISC=0\r\n");
    //if(ec20_at_cmd("+QMTPUBEX",client->message)) esp8266_print("Send message ok\n");
    //else esp8266_print("Send message error\n");
}

void GSM_SendString(char *s)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)s, strlen(s), 1000);
}

bool sim4g_send_sms(char* sdt, char* noi_dung)
{
    char cmd[64];


    GSM_SendString("AT+CMGF=1\r");
    HAL_Delay(500);

    // AT+CSCS="GSM"
    GSM_SendString("AT+CSCS=\"GSM\"\r");
    HAL_Delay(500);

    // AT+CMGS="s?"
    sprintf(cmd, "AT+CMGS=\"%s\"\r", sdt);
    GSM_SendString(cmd);
    HAL_Delay(500);

    GSM_SendString(noi_dung);


    uint8_t ctrlz = 0x1A;
    HAL_UART_Transmit(&huart1, &ctrlz, 1, 1000);

   return false;
}

/*
bool sim4g_waiting_feedback(char feedback[100],uint16_t timeOut)
{
	while(timeOut>0)
	{
		if(strstr(sim_feedback,feedback))
		{
			memset(sim_feedback,0,sizeof(sim_feedback));
			return true;
		}
		osDelay(10);
		timeOut--;
	}
	return false;
}
*/

bool sim4g_call(char *sdt)
{
	if(CheckSimBusy(500)==BUSY) return false;
	SimIsBusy=true;
	osDelay(300);
	sim4g_print("ATD%s;\r\n",sdt);
	if (sim4g_waiting_feedback("OK",3000))
	{
		SimIsBusy=false;
		return true;
	}
	SimIsBusy=false;
	return false;
}

/*
uint8_t sim4g_read_signalStrength(uint16_t *signalValue)
{
	if(CheckSimBusy(500)==BUSY) return *(signalValue);
	sim4g_print("AT+CSQ\r\n");
	uint16_t timeOut=500;
	while(timeOut>0)
	{
		if(strstr(sim_feedback,"+CSQ"))
		{
			char signalStrength[10];
			char Strength=0;
			memset(signalStrength,0,sizeof(signalStrength));
			trim(sim_feedback, signalStrength, "+CSQ:", "OK");
			Strength=atof(signalStrength);
			rs485_2_print("Strength=%d\n",Strength);
			//rs485_2_print("\n");
			//rs485_2_print("trim=%s\n",signalStrength);
			//rs485_2_print("signalStrength=%d\n",Strength);

			if(Strength<=9) *(signalValue)=1;
			else if(Strength>=10 && Strength<15) *(signalValue)=2;
			else if(Strength>=15 && Strength<20) *(signalValue)=3;
			else if(Strength>=20 && Strength<=30) *(signalValue)=4;
			else *(signalValue)=0;

			memset(sim_feedback,0,sizeof(sim_feedback));
			SimIsBusy=false;
			return *(signalValue);
		}
		osDelay(10);
		timeOut--;
	}
	SimIsBusy=false;
	return *(signalValue);
}
*/


bool CheckSimBusy(uint16_t timeOut)
{
	while(timeOut>0)
	{
		if(SimIsBusy==false) return IDLE;
		timeOut--;
		HAL_Delay(1);
	}
	if(timeOut==0) return BUSY;
}











