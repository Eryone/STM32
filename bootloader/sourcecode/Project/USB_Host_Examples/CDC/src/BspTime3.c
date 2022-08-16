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

#include "BspTime3.h"


static void (*IRQHandler)(void) = NULL;

/*******************************************************************************
* Function Name :void BspTim3Init(void)
* Description   :ͨ�ö�ʱ�� ��ʼ����
* Input         :
* Output        :
* Other         :
* Date          :2013.01.27
*******************************************************************************/
void BspTim3Init(void)
{
    NVIC_InitTypeDef NvicInitdef;
    TIM_TimeBaseInitTypeDef timbase;	
    u16 cnt = 170 * 72;     //170us ����

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	

    TIM_DeInit(TIM3); 
    timbase.TIM_CounterMode = TIM_CounterMode_Up;
    timbase.TIM_ClockDivision = TIM_CKD_DIV1;
    timbase.TIM_Period = cnt - 1;
    timbase.TIM_Prescaler = 0; 	        // 1��Ƶ
    TIM_TimeBaseInit(TIM3, &timbase);

    NvicInitdef.NVIC_IRQChannel = TIM3_IRQn;
    NvicInitdef.NVIC_IRQChannelPreemptionPriority = 6;
    NvicInitdef.NVIC_IRQChannelSubPriority = 0;
    NvicInitdef.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NvicInitdef);

    TIM_SetCounter(TIM3, 0);
    TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
    TIM_ClearITPendingBit( TIM3, TIM_IT_Update);

    /* TIMX enable counter */
    TIM_Cmd(TIM3, DISABLE);
}

/*******************************************************************************
* Function Name :void BspTim3Open()
* Description   :ʹ���ܶ�ʱ��
* Input         :
* Output        :
* Other         :
* Date          :2013.02.19
*******************************************************************************/
void BspTim3Open(void)
{
    TIM_SetCounter(TIM3, 0);
    TIM_Cmd(TIM3, ENABLE);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}

/*******************************************************************************
* Function Name :void BspTim3Close()
* Description   :�رն�ʱ��
* Input         :
* Output        :
* Other         :
* Date          :2013.02.19
*******************************************************************************/
void BspTim3Close(int close_flag)
{
		if(close_flag==1)
		{
			NVIC_InitTypeDef NvicInitdef;
			NvicInitdef.NVIC_IRQChannel = TIM3_IRQn;
			NvicInitdef.NVIC_IRQChannelPreemptionPriority = 6;
			NvicInitdef.NVIC_IRQChannelSubPriority = 0;
			NvicInitdef.NVIC_IRQChannelCmd = DISABLE;
			NVIC_Init(&NvicInitdef);
		}
    TIM_Cmd(TIM3, DISABLE);
    TIM_SetCounter(TIM3, 0);
    TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
}


/*******************************************************************************
* Function Name :void BspTim3SetCounter(u16 count)
* Description   :���ü���ֵ
* Input         :
* Output        :
* Other         :
* Date          :2013.02.19
*******************************************************************************/
void BspTim3SetCounter(u16 count)
{
    TIM_SetCounter(TIM3, count);
}

/*******************************************************************************
* Function Name :void BspTim3SetIRQFun(void *fun)
* Description   :�����жϺ���
* Input         :
* Output        :
* Other         :
* Date          :2013.02.19
*******************************************************************************/
void BspTim3SetIRQCallBack(void *fun)
{
    IRQHandler = (void (*)(void))fun;
}

/*******************************************************************************
* Function Name :void TIM3_IRQHandler(void)
* Description   :timx�жϺ���
* Input         :
* Output        :
* Other         :
* Date          :2013.01.27
*******************************************************************************/
void TIM3_IRQHandler(void)
{
    if( SET == TIM_GetITStatus(TIM3,TIM_IT_Update) )
    {
        TIM_ClearITPendingBit( TIM3, TIM_IT_Update);

        if (IRQHandler != NULL)
            (*IRQHandler)();			//�жϺ���
    }
}


/********************** END ***************************************************/

