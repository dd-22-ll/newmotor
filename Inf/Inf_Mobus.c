#include "Inf_Mobus.h"


void Inf_Mobus_Init(void)
{
    // modbus��ʼ��
    eMBInit(MB_RTU, motor.id, 0, 115200, MB_PAR_NONE);

    // ʹ��modbus
    eMBEnable();
}

void Inf_Mobus_AutoRecvAndReply(void)
{   
    // �����Եĵ���
    eMBPoll();
}

