#include "servo.h"

SoftServo_t g_softservo;

static float clampf(float v, float a, float b) {
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

HAL_StatusTypeDef SoftServo_Init(SoftServo_t *ctx, TIM_HandleTypeDef *htim_base)
{
    if (!ctx || !htim_base) return HAL_ERROR;
    ctx->htim_base  = htim_base;
    ctx->n_channels = 0;
    ctx->frame_tick = 0;

    // Đảm bảo các chân output ở mức LOW ban đầu
    for (uint16_t i = 0; i < SOFTSERVO_MAX_CH; ++i) {
        ctx->ch[i].enabled = 0;
    }

    // Start base timer (update event mỗi 10us)
    if (HAL_TIM_Base_Start_IT(ctx->htim_base) != HAL_OK) return HAL_ERROR;
    return HAL_OK;
}

HAL_StatusTypeDef SoftServo_AddChannel(SoftServo_t *ctx,
                                       GPIO_TypeDef *port, uint16_t pin,
                                       float min_pulse_ms, float max_pulse_ms)
{
    if (!ctx || !port) return HAL_ERROR;
    if (ctx->n_channels >= SOFTSERVO_MAX_CH) return HAL_ERROR;

    uint16_t idx = ctx->n_channels++;

    ctx->ch[idx].port         = port;
    ctx->ch[idx].pin          = pin;
    ctx->ch[idx].min_pulse_ms = (min_pulse_ms > 0.1f) ? min_pulse_ms : 0.6f;
    ctx->ch[idx].max_pulse_ms = (max_pulse_ms > 0.1f) ? max_pulse_ms : 2.4f;
    ctx->ch[idx].pulse_ticks  = (uint16_t)((1.5f * 1000.0f) / SOFTSERVO_TICK_US); // mặc định ~1.5ms (giữa)
    ctx->ch[idx].enabled      = 1;

    // Config GPIO: Output push-pull, tốc độ cao (nên cấu hình trước trong MX_GPIO_Init)
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
    return HAL_OK;
}

HAL_StatusTypeDef SoftServo_SetPulseMs(SoftServo_t *ctx, uint16_t idx, float pulse_ms)
{
    if (!ctx || idx >= ctx->n_channels) return HAL_ERROR;
    if (!ctx->ch[idx].enabled) return HAL_ERROR;

    float p = clampf(pulse_ms, ctx->ch[idx].min_pulse_ms, ctx->ch[idx].max_pulse_ms);
    uint16_t ticks = (uint16_t)((p * 1000.0f) / (float)SOFTSERVO_TICK_US);
    if (ticks >= SOFTSERVO_FRAME_TICKS) ticks = SOFTSERVO_FRAME_TICKS - 1;
    ctx->ch[idx].pulse_ticks = ticks;
    return HAL_OK;
}

HAL_StatusTypeDef SoftServo_SetAngle(SoftServo_t *ctx, uint16_t idx, float deg)
{
    if (!ctx || idx >= ctx->n_channels) return HAL_ERROR;
    float d = clampf(deg, 0.0f, 180.0f);
    float pulse = ctx->ch[idx].min_pulse_ms +
                  (d / 180.0f) * (ctx->ch[idx].max_pulse_ms - ctx->ch[idx].min_pulse_ms);
    return SoftServo_SetPulseMs(ctx, idx, pulse);
}

void SoftServo_OnTimerISR(SoftServo_t *ctx)
{
    if (!ctx) return;

    uint16_t t = ctx->frame_tick;

    if (t == 0) {
        // Bắt đầu khung: kéo tất cả kênh lên HIGH
        for (uint16_t i = 0; i < ctx->n_channels; ++i) {
            if (ctx->ch[i].enabled)
                HAL_GPIO_WritePin(ctx->ch[i].port, ctx->ch[i].pin, GPIO_PIN_SET);
        }
    }

    // Với mỗi kênh, nếu đến thời điểm kết thúc xung thì kéo xuống LOW
    for (uint16_t i = 0; i < ctx->n_channels; ++i) {
        if (ctx->ch[i].enabled && (t == ctx->ch[i].pulse_ticks)) {
            HAL_GPIO_WritePin(ctx->ch[i].port, ctx->ch[i].pin, GPIO_PIN_RESET);
        }
    }

    // Tăng tick khung
    t++;
    if (t >= SOFTSERVO_FRAME_TICKS) t = 0;
    ctx->frame_tick = t;
}
