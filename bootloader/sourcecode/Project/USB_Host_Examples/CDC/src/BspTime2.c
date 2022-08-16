/*
********************************************************************************
*                               ��ʱ�������������
*                              ARM Cortex-M3 Port
*
* File          : DrvTime3.C
* Version       : V1.0
* By            : ����ǿ
*
* For           : Stm32f10x
* Mode          : Thumb2
* Toolchain     : 
*                   RealView Microcontroller Development Kit (MDK)
*                   Keil uVision
* Description   : ��ʱ������  ռ�ö�ʱ������ʱ��pwm 4·ͨ��(������Ҫѡ��)
* Date          : 2012.05.22
*******************************************************************************/


#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
//#include "stm32f10x_nvic.h"

#include "BspTime2.h"





volatile int g_Tim2Array[(u16)eTimMax] = {0,};



/*******************************************************************************
* Function Name :void BspTim2Init(void)
* Description   :ͨ�ö�ʱ�� ��ʼ����
* Input         :
* Output        :
* Other         :
* Date          :2013.01.27
*******************************************************************************/
void BspTim2Init(void)
{
    NVIC_InitTypeDef NvicInitdef;
    TIM_TimeBaseInitTypeDef timbase;	
    uc16 cnt = 1000;     // 1ms ����

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	

    TIM_DeInit(TIM2); 
    timbase.TIM_CounterMode = TIM_CounterMode_Up;
    timbase.TIM_ClockDivision = TIM_CKD_DIV1;
    timbase.TIM_Period = cnt - 1;
    timbase.TIM_Prescaler = 120-1;//72 - 1; 	// 72��Ƶ
    TIM_TimeBaseInit(TIM2, &timbase);

    NvicInitdef.NVIC_IRQChannel = TIM2_IRQn;
    NvicInitdef.NVIC_IRQChannelPreemptionPriority = 10;    //������ȼ�
    NvicInitdef.NVIC_IRQChannelSubPriority = 0;
    NvicInitdef.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NvicInitdef);

    TIM_SetCounter(TIM2, 0);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void BspTim2Close(void)
{
    TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM2, DISABLE);
}

/*******************************************************************************
* Function Name :void TIM2_IRQHandler(void)
* Description   :tim2�жϺ���
* Input         :
* Output        :
* Other         :
* Date          :2013.01.27
*******************************************************************************/
void TIM2_IRQHandler(void)
{
    u16 i = 0;

    if( SET == TIM_GetITStatus(TIM2,TIM_IT_Update) )
    {
        TIM_ClearITPendingBit( TIM2, TIM_IT_Update);

        for (i = 0; i < (u16)eTimMax; i++)
        {
            g_Tim2Array[i]++;
        }
    }
}


/********************** END ***************************************************/

