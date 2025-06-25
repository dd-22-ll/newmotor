#include "App_Task.h"

void         startTask(void *);
TaskHandle_t startTaskHandle;

void         commu(void *);
TaskHandle_t commuTaskHandle;

void         keyScan(void *);
TaskHandle_t keyScanTaskHandle;

void         display(void *);
TaskHandle_t displayTaskHandle;

void         motorControl(void *);
TaskHandle_t motorControlTaskHandle;

/* ����freertos */
void App_Task_StartFreeRTOS(void)
{

    debug_printfln("����freertos...");
    /* 1. ����һ������: ��������, ����������ڲ�ȥ���������ĺ�ҵ����ص����� */
    BaseType_t r = xTaskCreate(startTask,
                               "startTask",
                               128,
                               NULL,
                               10,
                               &startTaskHandle); /* �ṹ��ָ�� */
    if(r)
    {
        debug_printfln("������������ɹ�");
    }
    else
    {
        debug_printfln("������������ʧ��");
    }

    /* 2. ���������� */
    vTaskStartScheduler();

    /* ����Ĵ��벻����ִ�� */
    debug_printfln("����ִ��....");
}

/* ��������ĺ��� */
void startTask(void *args)
{
    debug_printfln("��������ʼִ��...");

    /* motor���� ģ�� ���� */
    App_MotorControl_Start();

    /* ������ʾģ�� */
    App_Display_Start();

    /* ͨѶģ������ */
    App_Commu_Start();

    xTaskCreate(commu,
                "commuTask",
                256,
                NULL,
                8,
                &commuTaskHandle);

    xTaskCreate(keyScan,
                "keyScanTask",
                128,
                NULL,
                8,
                &keyScanTaskHandle);

    xTaskCreate(display,
                "displayTask",
                128,
                NULL,
                8,
                &displayTaskHandle);

    xTaskCreate(motorControl,
                "motorControl",
                128,
                NULL,
                8,
                &motorControlTaskHandle);

    /* ɾ����������: ��ɱ */
    vTaskDelete(NULL); /* ����NULL, ���ɾ���Լ� */
}

/* ͨѶ���� */
void commu(void *args)
{
    vTaskDelay(100);
    debug_printfln("ͨѶ����ʼִ��...");
    TickType_t lastWakeTime = xTaskGetTickCount();

    CanMsg_t msg[3];
    uint8_t  msgNum = 0;
    while(1)
    {
        App_Commu_RecvModbusData();

        /* can������ */
        Inf_CAN_RecvData(msg, &msgNum);
        if(msgNum > 0)
        {
            for(size_t i = 0; i < msgNum; i++)
            {
                if(msg[i].data[0] == 0)
                {
                    // ����ת��
                    int16_t speed = (int16_t)((msg[i].data[1] << 8) | msg[i].data[2]);
                    debug_printfln("can���õ�ת��: %d", speed);
                    REG_HOLD_BUF[0] = (uint16_t)speed;
                }
                else if(msg[i].data[0] == 1)
                {
                    // ��ȡת��
                    debug_printfln("can��ȡת��");
                    CanMsg_t msg;
                    msg.ID      = 0x255;
                    msg.data[0] = motor.id;
                    msg.data[1] = (uint8_t)(motor.realRPM >> 8);
                    msg.data[2] = (uint8_t)(motor.realRPM & 0xff);
                    msg.dataLen = 3;
                    Inf_CAN_SendData(&msg);
                }
            }
        }

        vTaskDelayUntil(&lastWakeTime, 30);
    }
}

void motorControl(void *args)
{
    debug_printfln("motorControl��ʼִ��...");
    TickType_t lastWakeTime = xTaskGetTickCount();
    uint8_t    dt           = 50;
    while(1)
    {
        /* 1. ��ȡ�����ʵ��ת�� */
        int16_t realRPM = App_MotorControl_GetRealRPM(dt / 1000.0);

        /* 2. pid���� */
        App_MotorControl_PIDCacl(realRPM, dt);

        App_MotorControl_MotorRun();

        vTaskDelayUntil(&lastWakeTime, dt);
    }
}

/* ����ɨ������ */
void keyScan(void *args)
{
    debug_printfln("keyScan��ʼִ��...");
    TickType_t lastWakeTime = xTaskGetTickCount();
    while(1)
    {
        App_MotorControl_KeyScan();
        vTaskDelayUntil(&lastWakeTime, 50);
    }
}

void display(void *args)
{
    debug_printfln("display��ʼִ��...");
    TickType_t lastWakeTime = xTaskGetTickCount();

    while(1)
    {
        App_Display_Show();
        vTaskDelayUntil(&lastWakeTime, 20);
    }
}
