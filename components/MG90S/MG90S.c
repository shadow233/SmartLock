#include <stdio.h>
#include "MG90S.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO (8) // Define the output GPIO
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
// #define LEDC_DUTY (205)                 // Set duty to 2.5%. ((2 ** 13) - 1) * 2.5% = 204.775
#define LEDC_FREQUENCY (50) // Frequency in Hertz. Set frequency at 50 Hz
// #define SERVO_MAX_DEGREE (100)          // Maximum angle in degree upto which servo can rotate

#ifdef MG90S_360_EN
#define MG90S_360_DUTY_Forward (205)  // 2.5%
#define MG90S_360_DUTY_Stop (614)     // 7.5%
#define MG90S_360_DUTY_Reverse (1024) // 12.5%
#endif

#ifdef MG90S_180_EN
#define MG90S_180_DUTY_DEGREE_0 (205)    // 2.5%
#define MG90S_180_DUTY_DEGREE_45 (410)   // 5%
#define MG90S_180_DUTY_DEGREE_90 (614)   // 7.5%
#define MG90S_180_DUTY_DEGREE_135 (819)  // 10%
#define MG90S_180_DUTY_DEGREE_180 (1024) // 12.5%
#endif

void MG90S_Init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = LEDC_OUTPUT_IO,
#ifdef MG90S_360_EN
        .duty = MG90S_360_DUTY_Stop,
#endif
#ifdef MG90S_180_EN
        .duty = MG90S_180_DUTY_DEGREE_90,
#endif
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

/*
    舵机棕色线 --> GND
    舵机红色线 --> 5V
    舵机橙色线 --> GPIO_Pin_6
*/

#ifdef MG90S_360_EN
/************************************************************************************/
/*以下函数为360度舵机的正向转动、停止、反向转动的测试函数
    *	PWM 信号与360舵机转速的关系：
    *	0.5ms ---------------- 正向最大转速
    *	1.5ms ---------------- 速度为0
    *	2.5ms ---------------- 反向最大转速

    *	舵机频率与占空比的计算：
    *	设舵机的频率为50HZ，则PWM周期为20ms，0度对应的占空比为2.5%，即0.05ms的高电平输出。
 */

// 360度舵机测试函数
void MG90S_360_Forward(void)
{
    /* 0.5ms --> 正向最大转速 */
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, MG90S_360_DUTY_Forward));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    vTaskDelay(pdMS_TO_TICKS(5000));

    /* 1.5ms --> 速度为0 */
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, MG90S_360_DUTY_Stop));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    vTaskDelay(pdMS_TO_TICKS(5000));

    /* 2.5ms --> 反向最大转速 */
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, MG90S_360_DUTY_Reverse));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    vTaskDelay(pdMS_TO_TICKS(5000));
}
/*****************************************END****************************************/
#endif

#ifdef MG90S_180_EN
/************************************************************************************/
/*以下函数为0-180度舵机角度控制测试函数
    *	PWM 信号与0-180舵机的关系：
    *	0.5ms ---------------- 0度
    *	1ms   ---------------- 45度
    *	1.5ms ---------------- 90度
    *	2ms   ---------------- 135度
    *	2.5ms ---------------- 180度

    *	舵机频率与占空比的计算：
    *	设舵机的频率为50HZ，则PWM周期为20ms，0度对应的占空比为2.5%，即0.05ms的高电平输出。
 */

// 0-180度舵机 --> 0-90-180度控制函数
void MG90S_180_Angle(void)
{
    /* 0度 */
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, MG90S_180_DUTY_DEGREE_0));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    vTaskDelay(pdMS_TO_TICKS(1000));

    /* 90度 */
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, MG90S_180_DUTY_DEGREE_90));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    vTaskDelay(pdMS_TO_TICKS(1000));

    /* 180度 */
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, MG90S_180_DUTY_DEGREE_180));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    vTaskDelay(pdMS_TO_TICKS(1000));
}
/*****************************************END****************************************/
#endif