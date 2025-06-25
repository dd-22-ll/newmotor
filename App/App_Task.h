#ifndef __APP_TASK_H
#define __APP_TASK_H
#include "Common_Debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "App_MotorControl.h"
#include "App_Display.h"
#include "App_Commu.h"
#include "Inf_CAN.h"

void App_Task_StartFreeRTOS(void);
#endif
