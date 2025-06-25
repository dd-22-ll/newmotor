#include "Inf_CAN.h"
#include "Inf_Motor.h"

void Inf_CAN_Init(void)
{
    /* ����CAN���� */
    HAL_CAN_Start(&hcan);

    /* ���ù����� */
    CAN_FilterTypeDef config;
    config.FilterBank       = 0;                       // ��������0
    config.FilterMode       = CAN_FILTERMODE_IDLIST;   // ID�б�ģʽ
    config.FilterScale      = CAN_FILTERSCALE_32BIT;   // 32λ������
    config.FilterIdHigh     = motor.id << 5;           // ��16λID
    config.FilterIdLow      = 0x0000;                  // ��16λID
    config.FilterMaskIdHigh = 0x0000;                  // ��16λ����
    config.FilterMaskIdLow  = 0x0000;                  // ��16λ����

    /* config.FilterBank       = 0;                       // ��������0
    config.FilterMode       = CAN_FILTERMODE_IDMASK;   // ʹ������ģʽ
    config.FilterScale      = CAN_FILTERSCALE_32BIT;   // 32λ������
    config.FilterIdHigh     = 0x0000;                  // ��16λID
    config.FilterIdLow      = 0x0000;                  // ��16λID
    config.FilterMaskIdHigh = 0x0000;                  // ��16λ����
    config.FilterMaskIdLow  = 0x0000;                  // ��16λ���� */

    config.FilterFIFOAssignment = CAN_FILTER_FIFO0;   // ���������䵽FIFO0
    config.FilterActivation     = ENABLE;             // ���ù�����
    HAL_CAN_ConfigFilter(&hcan, &config);
}

void Inf_CAN_SendData(CanMsg_t *msg)
{
    uint32_t            mailBox;
    CAN_TxHeaderTypeDef header;
    header.IDE   = CAN_ID_STD;     // ��׼֡
    header.RTR   = CAN_RTR_DATA;   // ����֡orң��֡
    header.StdId = msg->ID;        // ����id
    header.DLC   = msg->dataLen;   // ���ݳ���
    HAL_CAN_AddTxMessage(&hcan, &header, msg->data, &mailBox);
}

void Inf_CAN_RecvData(CanMsg_t msgs[], uint8_t *msgNum)
{
    *msgNum = HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_FILTER_FIFO0);

    if(*msgNum == 0) return;

    CAN_RxHeaderTypeDef header;
    for(size_t i = 0; i < *msgNum; i++)
    {
        memset(msgs[i].data, 0, 9);
        HAL_CAN_GetRxMessage(&hcan, CAN_FILTER_FIFO0, &header, msgs[i].data);
        msgs[i].ID      = header.StdId;
        msgs[i].dataLen = header.DLC;
    }
}
