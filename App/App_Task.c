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

/* 启动freertos */
void App_Task_StartFreeRTOS(void)
{

    debug_printfln("启动freertos...");
    /* 1. 创建一个任务: 启动任务, 在这个任务内部去创建其他的和业务相关的任务 */
    BaseType_t r = xTaskCreate(startTask,
                               "startTask",
                               128,
                               NULL,
                               10,
                               &startTaskHandle); /* 结构体指针 */
    if(r)
    {
        debug_printfln("创建启动任务成功");
    }
    else
    {
        debug_printfln("创建启动任务失败");
    }

    /* 2. 启动调度器 */
    vTaskStartScheduler();

    /* 后面的代码不会再执行 */
    debug_printfln("不会执行....");
}

/* 启动任务的函数 */
void startTask(void *args)
{
    debug_printfln("启动任务开始执行...");

    /* motor控制 模块 启动 */
    App_MotorControl_Start();

    /* 启动显示模块 */
    App_Display_Start();

    /* 通讯模块启动 */
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

    /* 删除启动任务: 自杀 */
    vTaskDelete(NULL); /* 传递NULL, 则会删除自己 */
}

/* 通讯任务 */
void commu(void *args)
{
    vTaskDelay(100);
    debug_printfln("通讯任务开始执行...");
    TickType_t lastWakeTime = xTaskGetTickCount();

    CanMsg_t msg[3];
    uint8_t  msgNum = 0;
    while(1)
    {
        App_Commu_RecvModbusData();

        /* can收数据 */
        Inf_CAN_RecvData(msg, &msgNum);
        if(msgNum > 0)
        {
            for(size_t i = 0; i < msgNum; i++)
            {
                if(msg[i].data[0] == 0)
                {
                    // 设置转速
                    int16_t speed = (int16_t)((msg[i].data[1] << 8) | msg[i].data[2]);
                    debug_printfln("can设置的转速: %d", speed);
                    REG_HOLD_BUF[0] = (uint16_t)speed;
                }
                else if(msg[i].data[0] == 1)
                {
                    // 获取转速
                    debug_printfln("can获取转速");
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
    debug_printfln("motorControl开始执行...");
    TickType_t lastWakeTime = xTaskGetTickCount();
    uint8_t    dt           = 50;
    while(1)
    {
        /* 1. 获取电机的实际转速 */
        int16_t realRPM = App_MotorControl_GetRealRPM(dt / 1000.0);

        /* 2. pid计算 */
        App_MotorControl_PIDCacl(realRPM, dt);

        App_MotorControl_MotorRun();

        vTaskDelayUntil(&lastWakeTime, dt);
    }
}

/* 按键扫描任务 */
void keyScan(void *args)
{
    debug_printfln("keyScan开始执行...");
    TickType_t lastWakeTime = xTaskGetTickCount();
    while(1)
    {
        App_MotorControl_KeyScan();
        vTaskDelayUntil(&lastWakeTime, 50);
    }
}

void display(void *args)
{
    debug_printfln("display开始执行...");
    TickType_t lastWakeTime = xTaskGetTickCount();

    while(1)
    {
        App_Display_Show();
        vTaskDelayUntil(&lastWakeTime, 20);
    }
}
