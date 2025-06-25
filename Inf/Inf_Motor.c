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
    HAL_TIM_PWM_Start(motor.htim, motor.channel);      // ����PWM���
    HAL_TIMEx_PWMN_Start(motor.htim, motor.channel);   // ������Ӧ��ʱ���ķ������

    /* �����õĶ�ʱ�� */
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);   // ����������
}

void Inf_Motor_Start(void)
{
    HAL_GPIO_WritePin(MOTOR_SHUTDOWN_GPIO_Port, MOTOR_SHUTDOWN_Pin, GPIO_PIN_RESET);   // �������
}

void Inf_Motor_Stop(void)
{
    HAL_GPIO_WritePin(MOTOR_SHUTDOWN_GPIO_Port, MOTOR_SHUTDOWN_Pin, GPIO_PIN_SET);   // ֹͣ���
}

void Inf_Motor_SetSpeed(int16_t speed)
{
    motor.speed = LIMIT(speed, -498, 498);   // �����ٶȷ�Χ [0, 1000]

    __HAL_TIM_SetCompare(motor.htim, motor.channel, motor.speed + 500);   // ����ռ�ձ�
}

/**
 * @description: ��ȡ���ת��
 *
 *  1. һ�������4����
 *  2. ���һȦ11������
 * 3. ���ٱ�: 6000/280=21.43
 * @return {*} RPM
 */
int16_t Inf_Motor_GetRPM(float dt)
{
    int16_t rpm = (int16_t)((int16_t)__HAL_TIM_GetCounter(&htim4) / 4.0 / 11 / 21.43 / dt * 60);
    __HAL_TIM_SetCounter(&htim4, 0);   // ���������
    return rpm;
}
