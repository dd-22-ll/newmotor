#ifndef __INF_MOTOR_H
#define __INF_MOTOR_H
#include "Common_Debug.h"
#include "tim.h"
#include "gpio.h"
#include "Com_Define.h"

typedef enum
{
    FORWARD  = 1,    // ��ת
    BACKWARD = -1,   // ��ת
} Dir_t;

typedef enum
{
    LOCAL  = 0,   // ���ؿ���
    REMOTE = 1,   // Զ�̿���
} Contorl_Mode_t;

typedef struct
{
    TIM_HandleTypeDef *htim;
    uint32_t           channel;

    Dir_t runDir; /* ת������: 1��ת -1��ת */

    int16_t speed; /* ����ռ�ձ�: [-500 +500] */

    int16_t expectRPM; /* ����ת��: [-280, 280] */
    int16_t realRPM;   /* ʵ��ת��: [-280, 280] */

    uint8_t id; /* ������  modbusͨѶ��ʱ�ĵ�ַ*/

    Contorl_Mode_t controlMode; /* ����ģʽ: 0���ؿ��� 1Զ�̿��� */

} Motor_t;

extern Motor_t motor;

void Inf_Motor_Init(void);

void Inf_Motor_Start(void);

void Inf_Motor_Stop(void);

void Inf_Motor_SetSpeed(int16_t speed);

int16_t Inf_Motor_GetRPM(float dt);

#endif
