#include "Inf_Motor.h"

Motor_t motor = {
    .htim        = &htim1,
    .channel     = TIM_CHANNEL_1,
    .runDir      = FORWARD,
    .speed       = 0,
    .expectRPM   = 0,
    .realRPM     = 0,
    .id          = 1,
    .controlMode = REMOTE};

void Inf_Motor_Init(void)
{
    HAL_TIM_PWM_Start(motor.htim, motor.channel);      // 启动PWM输出
    HAL_TIMEx_PWMN_Start(motor.htim, motor.channel);   // 启动对应定时器的反相输出

    /* 测速用的定时器 */
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);   // 启动编码器
}

void Inf_Motor_Start(void)
{
    HAL_GPIO_WritePin(MOTOR_SHUTDOWN_GPIO_Port, MOTOR_SHUTDOWN_Pin, GPIO_PIN_RESET);   // 启动电机
}

void Inf_Motor_Stop(void)
{
    HAL_GPIO_WritePin(MOTOR_SHUTDOWN_GPIO_Port, MOTOR_SHUTDOWN_Pin, GPIO_PIN_SET);   // 停止电机
}

void Inf_Motor_SetSpeed(int16_t speed)
{
    motor.speed = LIMIT(speed, -498, 498);   // 限制速度范围 [0, 1000]

    __HAL_TIM_SetCompare(motor.htim, motor.channel, motor.speed + 500);   // 设置占空比
}

/**
 * @description: 获取电机转速
 *
 *  1. 一个脉冲计4次数
 *  2. 电机一圈11个脉冲
 * 3. 减速比: 6000/280=21.43
 * @return {*} RPM
 */
int16_t Inf_Motor_GetRPM(float dt)
{
    int16_t rpm = (int16_t)((int16_t)__HAL_TIM_GetCounter(&htim4) / 4.0 / 11 / 21.43 / dt * 60);
    __HAL_TIM_SetCounter(&htim4, 0);   // 清零计数器
    return rpm;
}
