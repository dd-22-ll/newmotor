#include "Inf_CAN.h"
#include "Inf_Motor.h"

void Inf_CAN_Init(void)
{
    /* 启动CAN外设 */
    HAL_CAN_Start(&hcan);

    /* 配置过滤器 */
    CAN_FilterTypeDef config;
    config.FilterBank       = 0;                       // 过滤器组0
    config.FilterMode       = CAN_FILTERMODE_IDLIST;   // ID列表模式
    config.FilterScale      = CAN_FILTERSCALE_32BIT;   // 32位过滤器
    config.FilterIdHigh     = motor.id << 5;           // 高16位ID
    config.FilterIdLow      = 0x0000;                  // 低16位ID
    config.FilterMaskIdHigh = 0x0000;                  // 高16位掩码
    config.FilterMaskIdLow  = 0x0000;                  // 低16位掩码

    /* config.FilterBank       = 0;                       // 过滤器组0
    config.FilterMode       = CAN_FILTERMODE_IDMASK;   // 使用掩码模式
    config.FilterScale      = CAN_FILTERSCALE_32BIT;   // 32位过滤器
    config.FilterIdHigh     = 0x0000;                  // 高16位ID
    config.FilterIdLow      = 0x0000;                  // 低16位ID
    config.FilterMaskIdHigh = 0x0000;                  // 高16位掩码
    config.FilterMaskIdLow  = 0x0000;                  // 低16位掩码 */

    config.FilterFIFOAssignment = CAN_FILTER_FIFO0;   // 过滤器分配到FIFO0
    config.FilterActivation     = ENABLE;             // 启用过滤器
    HAL_CAN_ConfigFilter(&hcan, &config);
}

void Inf_CAN_SendData(CanMsg_t *msg)
{
    uint32_t            mailBox;
    CAN_TxHeaderTypeDef header;
    header.IDE   = CAN_ID_STD;     // 标准帧
    header.RTR   = CAN_RTR_DATA;   // 数据帧or遥控帧
    header.StdId = msg->ID;        // 具体id
    header.DLC   = msg->dataLen;   // 数据长度
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
