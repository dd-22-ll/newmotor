/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"
#include "stdio.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);

/* ----------------------- Start implementation -----------------------------*/
void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
    if(xRxEnable == TRUE)
    {
        __HAL_UART_ENABLE_IT(&MB_UART, UART_IT_RXNE);
        // printf("r\n");
    }
    else
    {
        __HAL_UART_DISABLE_IT(&MB_UART, UART_IT_RXNE);
    }

    if(xTxEnable == TRUE)
    {
        __HAL_UART_ENABLE_IT(&MB_UART, UART_IT_TXE);
        // printf("t\n");
    }
    else
    {
        __HAL_UART_DISABLE_IT(&MB_UART, UART_IT_TXE);
    }
}

BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
    // 初始化串口: 省略
    // 返回值必须改为true
    return TRUE;
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    // HAL_UART_Transmit(&MB_UART, (uint8_t *)&ucByte, 1, 1000);
    //  为了提高发送的效率, 直接操作寄存器
    MB_UART.Instance->DR = ucByte;
    return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR *pucByte)
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    // 为了提高发送的效率, 直接操作寄存器
    *pucByte = (CHAR)(MB_UART.Instance->DR & 0xFF);
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR(void)
{
    pxMBFrameCBTransmitterEmpty();
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR(void)
{
    pxMBFrameCBByteReceived();
}

// 实现串口2的中断服务函数
void USART2_IRQHandler(void)
{
    if(__HAL_UART_GET_FLAG(&MB_UART, UART_FLAG_ORE))  //过载错误
    {
        __HAL_UART_CLEAR_OREFLAG(&MB_UART);
        // 丢弃本次数据，直接返回
        MB_UART.Instance->DR;
        // printf("ORE\r\n");
        return;
    }
    if(__HAL_UART_GET_FLAG(&MB_UART, UART_FLAG_FE))  // 帧错误
    {
        __HAL_UART_CLEAR_FEFLAG(&MB_UART);
        MB_UART.Instance->DR;
        // printf("FE\r\n");
        return;
    }
    if(__HAL_UART_GET_FLAG(&MB_UART, UART_FLAG_PE))  //校验错误
    {
        __HAL_UART_CLEAR_PEFLAG(&MB_UART);
        MB_UART.Instance->DR;
        // printf("PE\r\n");
        return;
    }

    if(__HAL_UART_GET_FLAG(&MB_UART, UART_FLAG_NE))  // 噪声错误
    {
        __HAL_UART_CLEAR_NEFLAG(&MB_UART);
        MB_UART.Instance->DR;
        //printf("NE\r\n");
        return;
    }

    // printf("1\r\n");
    //  表示接收非空, 表示可以接收数据
    if(__HAL_UART_GET_FLAG(&MB_UART, UART_FLAG_RXNE))
    {
        // 通知框架: 有数据可以接收
        prvvUARTRxISR();
        __HAL_UART_CLEAR_FLAG(&MB_UART, UART_FLAG_RXNE);   //
    }
    else if(__HAL_UART_GET_FLAG(&MB_UART, UART_FLAG_TXE))
    {
        // 通知框架: 发送缓冲区为空,可以发送数据
        prvvUARTTxReadyISR();
        __HAL_UART_CLEAR_FLAG(&MB_UART, UART_FLAG_TXE);   //
    }
}
