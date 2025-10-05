/*
 * app.c
 *
 *  Created on: Sep 27, 2025
 *      Author: maiva
 */

#include "app.h"
#include "board_drive.h"
#include "my_debug.h"
#include "password_handle.h"
#include "sim4g.h"

#include "usart.h"
#include "tim.h"

char sdt_sos[20]="0899564156";
char countSendSms=0;

uint32_t adc_mq2[64]={0};
uint8_t lampGuestVal = 0;
uint8_t lampSleepVal = 0;
float BH1750_lux=0;
uint32_t timeReadBh1750=0;
uint32_t timeReadDht11=0;
dht11_t dht;


void delay_us (uint16_t us)
{
	HAL_TIM_Base_Start(&htim2);
    __HAL_TIM_SET_COUNTER(&htim2,0);  // set the counter value a 0
    while (__HAL_TIM_GET_COUNTER(&htim2) < us);  // wait for the counter to reach the us input in the parameter
}

// --- Helpers ---
uint16_t clamp_u16_int(int x, int lo, int hi)
{
    if (x < lo) return (uint16_t)lo;
    if (x > hi) return (uint16_t)hi;
    return (uint16_t)x;
}

// Ánh xạ góc → độ rộng xung (µs)
uint16_t servo_angle_to_us(float angle_deg)
{
    if (angle_deg < SERVO_MIN_ANGLE) angle_deg = SERVO_MIN_ANGLE;
    if (angle_deg > SERVO_MAX_ANGLE) angle_deg = SERVO_MAX_ANGLE;

    float k = (float)(SERVO_MAX_US - SERVO_MIN_US) / (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE);
    int us = (int)(SERVO_MIN_US + k * (angle_deg - SERVO_MIN_ANGLE) + 0.5f);
    return clamp_u16_int(us, SERVO_MIN_US, SERVO_MAX_US);
}

// Phát 1 chu kỳ xung servo (high = pulse_us, low = phần còn lại để đủ 20 ms)
void servo_pulse_us(uint16_t pulse_us)
{
    HAL_GPIO_WritePin(SERVO_GPIO_Port, SERVO_Pin, GPIO_PIN_SET);
    delay_us(pulse_us);
    HAL_GPIO_WritePin(SERVO_GPIO_Port, SERVO_Pin, GPIO_PIN_RESET);

    uint32_t rest_us = (pulse_us < SERVO_PERIOD_US) ? (SERVO_PERIOD_US - pulse_us) : 0;
    if (rest_us >= 1000U) HAL_Delay(rest_us / 1000U);
    rest_us %= 1000U;
    if (rest_us) delay_us(rest_us);
}

// Ghi góc cho servo (lặp vài chu kỳ để servo kịp tới vị trí)
void servo_write_angle(float angle_deg)
{
    uint16_t us = servo_angle_to_us(angle_deg);
    for (int i = 0; i < SERVO_PULSES; ++i) {
        servo_pulse_us(us);
    }
}

// Hàm theo yêu cầu của bạn, chuyển sang điều khiển theo góc
void door(char vi_tri)
{
	/*
    if (vi_tri == CLOSE) {
        servo_write_angle(ANGLE_CLOSE);
    } else if (vi_tri == OPEN) {
        servo_write_angle(ANGLE_OPEN);
    }
    */
}


void TIM3_ConfigFrequency(uint32_t f_pwm_hz)
{
  // Lấy clock timer (TIM3 nằm APB1; nếu APB1 prescaler != 1 thì timer_clk = 2*PCLK1)
  uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
  uint32_t tim_clk = (RCC->CFGR & RCC_CFGR_PPRE1_DIV1) ? pclk1 : (pclk1 * 2U);

  // Chọn PSC sao cho ARR <= 65535
  uint32_t psc = 0;
  uint32_t arr = (tim_clk / f_pwm_hz) - 1;
  if (arr > 65535) {
    // tăng PSC để ARR lọt 16-bit
    psc = (arr / 65536) + 1;
    arr = (tim_clk / ((psc + 1) * f_pwm_hz)) - 1;
  }

  __HAL_TIM_DISABLE(&htim3);
  __HAL_TIM_SET_PRESCALER(&htim3, psc);
  __HAL_TIM_SET_AUTORELOAD(&htim3, arr);
  __HAL_TIM_SET_COUNTER(&htim3, 0);
  __HAL_TIM_ENABLE(&htim3);
}

void PWM_Start(void)
{
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

// duty: 0..100 (%)
void TIM3_SetDuty_CH1(uint8_t duty)
{
  if (duty > 100) duty = 100;
  uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim3);
  uint32_t ccr = (uint32_t)((uint64_t)(arr + 1) * duty / 100);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, ccr);
}

void TIM3_SetDuty_CH2(uint8_t duty)
{
  if (duty > 100) duty = 100;
  uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim3);
  uint32_t ccr = (uint32_t)((uint64_t)(arr + 1) * duty / 100);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, ccr);
}

void esp32_send(const char *fmt, ...)
{
	#ifdef DEBUG_EN
		static char buffer1[256];
		unsigned int len=0;
		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer1, sizeof(buffer1), fmt, args);
		va_end(args);
		len=strlen(buffer1);
		HAL_UART_Transmit(&huart2,"@",1,100);
		HAL_UART_Transmit(&huart2,buffer1,len,100);
		HAL_UART_Transmit(&huart2,"#",1,100);
	#endif
}

/**
 * @brief  Đóng gói dữ liệu cảm biến thành JSON.
 * @param  gas_ppm    Giá trị cảm biến khí gas (uint16_t)
 * @param  light      Giá trị cảm biến ánh sáng (uint16_t)
 * @param  hum        Độ ẩm (uint8_t)
 * @param  temp       Nhiệt độ (uint8_t)
 * @param  out        Buffer đầu ra (mảng char do người gọi cấp)
 * @param  out_size   Kích thước buffer out (khuyến nghị >= 64)
 * @return Số byte đã ghi (không tính ký tự kết thúc '\0');
 *         trả về 0 nếu lỗi (buffer quá nhỏ hoặc lỗi format).
 */
size_t build_sensor_json(uint16_t gas_ppm,
                         uint16_t light,
                         uint8_t  hum,
                         uint8_t  temp,
                         char *out,
                         size_t out_size)
{
    if (out == NULL || out_size == 0) return 0;

    // JSON dạng: {"gas":123,"light":456,"hum":78,"temp":25}
    // Với giá trị lớn nhất: {"gas":65535,"light":65535,"hum":255,"temp":255}
    // Độ dài tối đa ~ 44 ký tự + '\0' → nên cấp ít nhất 64 byte cho an toàn.
    int n = snprintf(out, out_size,
                     "{\"gas\":%u,\"light\":%u,\"hum\":%u,\"temp\":%u}",
                     (unsigned)gas_ppm,
                     (unsigned)light,
                     (unsigned)hum,
                     (unsigned)temp);

    if (n < 0 || (size_t)n >= out_size) {
        // Quá dung lượng hoặc lỗi format
        if (out_size > 0) out[0] = '\0';
        return 0;
    }
    return (size_t)n; // số byte hữu ích (không gồm '\0')
}

bool esp32handle()
{
	if(esp32_uart.data_done_frame)
	{
		debug_print("esp32 send: %s\n",esp32_uart.arr_data_rec);

		//lockOn
		if(strstr(esp32_uart.arr_data_rec,"lockOn"))
		{
			rfid_valid=THE_DUNG;
		}
		else if(strstr(esp32_uart.arr_data_rec,"lockOff"))
		{
			rfid_valid=THE_DUNG;
		}
		//fanSleepOn
		if(strstr(esp32_uart.arr_data_rec,"fanSleepOn"))
		{
			rl1(on);
		}
		else if(strstr(esp32_uart.arr_data_rec,"fanSleepOff"))
		{
			rl1(off);
		}
		//fanCookOn
		if(strstr(esp32_uart.arr_data_rec,"fanCookOn"))
		{
			rl2(on);
		}
		else if(strstr(esp32_uart.arr_data_rec,"fanCookOff"))
		{
			rl2(off);
		}
		//fanWcOn
		if(strstr(esp32_uart.arr_data_rec,"fanWcOn"))
		{
			rl3(on);
		}
		else if(strstr(esp32_uart.arr_data_rec,"fanWcOff"))
		{
			rl3(off);
		}

		if(strstr(esp32_uart.arr_data_rec,"lampWcOn"))
		{
			lamp1(on);
		}
		else if(strstr(esp32_uart.arr_data_rec,"lampWcOff"))
		{
			lamp1(off);
		}

		if(strstr(esp32_uart.arr_data_rec,"lampCookOn"))
		{
			lamp2(on);
		}
		else if(strstr(esp32_uart.arr_data_rec,"lampCookOff"))
		{
			lamp2(off);
		}

		if (strstr(esp32_uart.arr_data_rec, "lampGuest"))
		{
		    // Tìm dấu '='
		    char *eq = strchr(esp32_uart.arr_data_rec, '=');
		    if (eq)
		    {
		        // Lấy phần sau dấu '='
		        int val = atoi(eq + 1);
		        if (val >= 0 && val <= 255)
		        {
		        	lampGuestVal = (uint8_t)val;
		        	TIM3_SetDuty_CH1(lampGuestVal);
		            debug_print("set lampGuestVal=%d\n",lampGuestVal);
		        }
		    }
		}
		else if (strstr(esp32_uart.arr_data_rec, "lampSleep"))
		{
		    char *eq = strchr(esp32_uart.arr_data_rec, '=');
		    if (eq)
		    {
		        int val = atoi(eq + 1);
		        if (val >= 0 && val <= 255)
		        {
		        	lampSleepVal = (uint8_t)val;
		        	TIM3_SetDuty_CH2(lampSleepVal);
		        	debug_print("set lampSleep=%d\n",lampSleepVal);
		        }
		    }
		}


		uartIntClear(&esp32_uart);
	}

	//700ms đọc cảm biến ánh sáng 1 lần
	if((HAL_GetTick()-timeReadBh1750)>700)
	{
		timeReadBh1750=HAL_GetTick();
		BH1750_ReadLight(&BH1750_lux);
		char json_buf[200];
		build_sensor_json(adc_mq2[0], (uint16_t)BH1750_lux, dht.humidty, dht.temperature, json_buf, sizeof(json_buf));
		debug_print("@%s#\n\r",json_buf);
		esp32_send(json_buf);
	}

	//1500ms đọc dht11/lần
	if((HAL_GetTick()-timeReadDht11)>1700)
	{
		timeReadDht11=HAL_GetTick();
		readDHT11(&dht);
		//debug_print("temp=%d\n",dht.temperature);
		//debug_print("hum=%d\n",dht.humidty);
	}

	if(adc_mq2[0]>GAS_LIMIT)
	{
		if(countSendSms<1)
		{
			countSendSms=1;
			sim4g_send_sms(sdt_sos, "CANH BAO KHI GAS !");
			for(int i=0;i<10;i++)
			{
				buzz(1);
				HAL_Delay(100);
				buzz(0);
				HAL_Delay(100);
			}
		}
	}

	if(adc_mq2[0]<1000)
	{
		countSendSms=0;
	}

	return 0;
}














