#ifndef _SERVO_H
#define _SERVO_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SOFTSERVO_MAX_CH     8       // số kênh tối đa
#define SOFTSERVO_TICK_US    10      // độ phân giải tick = 10 µs
#define SOFTSERVO_FRAME_MS   20      // chu kỳ servo 20 ms
#define SOFTSERVO_FRAME_TICKS ( (SOFTSERVO_FRAME_MS*1000) / SOFTSERVO_TICK_US )  // 2000

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
    float         min_pulse_ms;   // mặc định 0.6 ms
    float         max_pulse_ms;   // mặc định 2.4 ms
    uint16_t      pulse_ticks;    // số tick HIGH trong 1 khung
    uint8_t       enabled;
} SoftServo_Channel_t;

typedef struct {
    TIM_HandleTypeDef *htim_base; // ví dụ &htim2 (cấu hình Update mỗi 10 µs)
    uint16_t           n_channels;
    uint16_t           frame_tick; // tăng từ 0..1999
    SoftServo_Channel_t ch[SOFTSERVO_MAX_CH];
} SoftServo_t;


extern SoftServo_t g_softservo;


/* Khởi tạo bộ phát soft-PWM (gọi sau khi MX_TIMx_Init đã set period=10µs và đã Enable IRQ trong NVIC) */
HAL_StatusTypeDef SoftServo_Init(SoftServo_t *ctx, TIM_HandleTypeDef *htim_base);

/* Thêm 1 kênh servo (GPIO bất kỳ) */
HAL_StatusTypeDef SoftServo_AddChannel(SoftServo_t *ctx,
                                       GPIO_TypeDef *port, uint16_t pin,
                                       float min_pulse_ms, float max_pulse_ms);

/* Đặt góc 0..180° (tự clamp) cho kênh idx (0..n_channels-1) */
HAL_StatusTypeDef SoftServo_SetAngle(SoftServo_t *ctx, uint16_t idx, float deg);

/* Đặt trực tiếp độ rộng xung (ms) cho kênh idx */
HAL_StatusTypeDef SoftServo_SetPulseMs(SoftServo_t *ctx, uint16_t idx, float pulse_ms);

/* Hàm bạn gọi trong callback ngắt Update của timer (HAL_TIM_PeriodElapsedCallback) */
void SoftServo_OnTimerISR(SoftServo_t *ctx);

#ifdef __cplusplus
}
#endif
#endif /* SOFT_SERVO_H */
