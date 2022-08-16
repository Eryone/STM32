/*
********************************************************************************
*                               串口驱动程序设计
*                              ARM Cortex-M3 Port
*
* File          : DrvUsart1.C
* Version       : V1.0
* By            : 王宏强
*
* For           : Stm32f10x
* Mode          : Thumb2
* Toolchain     : 
*                   RealView Microcontroller Development Kit (MDK)
*                   Keil uVision
* Description   : 串口1驱动函数	

* Date          : 2013.1.27
*******************************************************************************/

#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
//#include "stm32f10x_nvic.h"


#include <string.h>
#include "BspUsart1.h"
#include "bsp.h"


void (*receChar)(u8 ch) = NULL;


static SerialBuffType m_SendBuff = SerialBuffDefault();
//static SerialBuffType m_ReceBuff = SerialBuffDefault();

static SerialBuffType *sb = &m_SendBuff;
//static SerialBuffType *rb = &m_ReceBuff;

static bool volatile eTXIdle = TRUE;

/*******************************************************************************
* Function Name :static s32 BspUsartxInit(void)
* Description   :串口及引脚初始化
* Input         :
* Output        :
* Other         :
* Date          :2013.01.27
*******************************************************************************/
void BspUsart1Init(void)
{
    GPIO_InitTypeDef GpioInitdef;
    USART_InitTypeDef UsartInitdef;
    NVIC_InitTypeDef NvicInitdef;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);  

    USART_DeInit(USART1);

    USART_StructInit( &UsartInitdef );
    UsartInitdef.USART_BaudRate = 115200;
    UsartInitdef.USART_StopBits = USART_StopBits_1;
    UsartInitdef.USART_WordLength = USART_WordLength_8b;
    UsartInitdef.USART_Parity = USART_Parity_No;

    UsartInitdef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &UsartInitdef);

    GpioInitdef.GPIO_Pin = GPIO_Pin_9; //
    GpioInitdef.GPIO_Speed = GPIO_Speed_10MHz;
    GpioInitdef.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GpioInitdef);

    GpioInitdef.GPIO_Pin = GPIO_Pin_10; //
    GpioInitdef.GPIO_Speed = GPIO_Speed_10MHz;
    GpioInitdef.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GpioInitdef);

    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NvicInitdef.NVIC_IRQChannel = USART1_IRQn;
    NvicInitdef.NVIC_IRQChannelPreemptionPriority = 1;
    NvicInitdef.NVIC_IRQChannelSubPriority = 0;
    NvicInitdef.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NvicInitdef );

    USART_Cmd(USART1, ENABLE);
}

void BspUsart1Close(void)
{
    while (eTXIdle != TRUE);
    USART_Cmd(USART1, DISABLE);

    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);

    USART_ClearITPendingBit(USART1, USART_IT_TXE);
    USART_ClearITPendingBit(USART1, USART_IT_TC);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
}


/*******************************************************************************
* Function Name :s32 BspUsartSend(u8* buf, u32 len)
* Description   :发送一组字符串
* Input         :*buf:字符串指针。len:长度
* Output        :
* Other         :
* Date          :2012.05.11  11:45:38
*******************************************************************************/
u16 BspUsart1Send(u8 *buf, u16 len)
{
    if( (0 == sb->len) && (len > 0) && (len <= USART1_BUFF_LANGTH) && (eTXIdle == TRUE))
    {
        USART_SendData(USART1, *buf++);
        eTXIdle = FALSE;
        sb->len = len-1;
        sb->ind = 0;
        memcpy(sb->buf, buf, len - 1);
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
* Function Name :u16 BspUsart1Receive(u8 *buf)
* Description   :读取接收的字符串
* Input         :buf: 缓冲变量
* Output        :
* Other         :
* Date          :2013.02.20
*******************************************************************************/
//u16 BspUsart1Receive(u8 *buf)
//{
//    u16 len = rb->ind;
//    
//    memcpy(buf, rb->buf, len );
//    rb->ind = 0;
//
//    return len;
//}

/*******************************************************************************
* Function Name :u8 BspUsart1IRQCallBack(void *fun)
* Description   :设置串口回调函数
* Input         :
* Output        :
* Other         :
* Date          :2013.02.19
*******************************************************************************/
void BspUsart1IRQCallBack(void *fun)
{
    receChar = (void (*)(u8))fun;
}

/*******************************************************************************
* Function Name :void USART1_IRQHandler(void)
* Description   :USART1 中断程序
* Input         :
* Output        :
* Other         :
* Date          :2011.11.16  16:57:39
*******************************************************************************/
void USART1_IRQHandler(void)
{
    if(SET == USART_GetITStatus(USART1, USART_IT_TXE))
    {
        USART_ClearITPendingBit(USART1, USART_IT_TXE);
        if (sb->len > 0 )
        {
            USART_SendData(USART1, sb->buf[sb->ind++]);
            sb->len--;
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
            USART_ITConfig(USART1, USART_IT_TC, ENABLE);
        }
    }
    else if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_TC);
        USART_ITConfig(USART1, USART_IT_TC, DISABLE);
        sb->len = 0;
        eTXIdle = TRUE;
    }
    else if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        u8 ch;
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
        ch = USART_ReceiveData(USART1);

//        if (rb->ind <USART1_BUFF_LANGTH)
//            rb->buf[rb->ind++] = ch;
        
        if (receChar != NULL)
            (*receChar)(ch);
    }	
}



/********************** END ***************************************************/


