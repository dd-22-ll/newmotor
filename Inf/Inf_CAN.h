#ifndef __INF_CAN_H
#define __INF_CAN_H
#include "Common_Debug.h"
#include "can.h"

typedef struct {
    uint16_t ID;
    uint8_t data[9];  // 'abcdefghi'
    uint8_t dataLen;
}CanMsg_t;

void Inf_CAN_Init(void);

void Inf_CAN_SendData(CanMsg_t *msg);

void Inf_CAN_RecvData(CanMsg_t msgs[], uint8_t *msgNum);

#endif 
