/**
  ******************************************************************************
  * @file    stm32fxxx_it.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   This file includes the interrupt handlers for the application
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usb_bsp.h"
#include "usbh_usr.h"
#include "usb_hcd_int.h"
#include "usbh_core.h"
#include "stm32fxxx_it.h"
//#include "data_handle.h"

//#include "LED.H"  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern USB_OTG_CORE_HANDLE           USB_OTG_Core;
extern USBH_HOST                    USB_Host;
 u32  Time3_Count;
/* Private function prototypes -----------------------------------------------*/
extern void USB_OTG_BSP_TimerIRQ (void);
extern   u16 Delay_Times; 
extern   u16  Delay_Count;

static u16 Key_Times=0;
u16 Delay_Count=0;
 u16 Delay_Times=0; 
 

extern unsigned int baud_rate_m;
 void st_Delay(uint32_t nCount)
{ 
  while(nCount > 0)
  { 
  	  nCount --;   
  }

}

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M Processor Exceptions Handlers                         */
/******************************************************************************/
/**
  * @brief  NMI_Handler
  *         This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  EXTI1_IRQHandler
  *         This function handles External line 1 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
      USB_Host.usr_cb->OverCurrentDetected();
      EXTI_ClearITPendingBit(EXTI_Line1);
  }
}

float fabs_c(float x,float y)
{
  if(x>y)
    return (x-y);
  else
    return (y-x);
    

}

void save_latest_pose(char paused)
{
   print_info.print_pos.file_pos=print_status.file_pos;
   print_info.print_pos.Z_pos=print_status.Z;
   print_info.print_pos.E_pos=print_status.E;
   print_info.print_paused=paused;
   STMFLASH_Write(PRINT_PARAMETERS_ADDR, (u16*)&print_info,  sizeof(print_info)/2);
  // STMFLASH_Write(PRINT_PARAMETERS_ADDR, (u16*)&print_info,  sizeof(print_info)/2);
   Debug_Information_ToUart4 ("saved success!\r\n");
   if(paused==1)
    GPIO_ResetBits(GPIOC , GPIO_Pin_14);// ok red led on
}

///PA2  power off
void EXTI2_IRQHandler(void)
{
    if ((EXTI_GetITStatus(EXTI_Line2) != RESET))
    {
     
      st_Delay(500);
      if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0)
        Debug_Information_ToUart4 ("PA2 down\r\n");
      
      if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0)&&(print_status.SD_printing==1)&&(print_status.file_pos>0)
         &&(print_status.usb_power!=2))     
      {
        if(fabs_c(print_status.Z,print_info.print_pos.Z_pos)>=0.1)
        {
           // CDC_SendData ("M25 \r\n", strlen("M25 \r\n"));
           print_status.SD_printing=0;
           save_latest_pose(1);
           
           Uart4_Send_Data("power off\r\n",strlen("power off\r\n"));
        }
      }
       EXTI_ClearFlag(EXTI_Line2);
    }

}

///PA3  power off micro usb
void EXTI3_IRQHandler(void)
{
    if ((EXTI_GetITStatus(EXTI_Line3) != RESET))
    {
     
      st_Delay(500);
      
      if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0)&&(print_status.SD_printing==1)&&(print_status.file_pos>0)
         &&(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0))     
      {
        if(fabs_c(print_status.Z,print_info.print_pos.Z_pos)>=0.1)
        {
         //  CDC_SendData("M25 \r\n", strlen("M25 \r\n"));
           print_status.SD_printing=0;
           save_latest_pose(1);
           
           Uart4_Send_Data("power off\r\n",strlen("micro usb power off\r\n"));
        }
      }
      if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0)
        Debug_Information_ToUart4 ("PA3 down\r\n");
      EXTI_ClearFlag(EXTI_Line3);
    }

}


void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line10) != RESET)
    {
      st_Delay(1000);
      // key down 
      if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10)==1)
      {
        TIM_Cmd(TIM3, DISABLE); 
        
        if(( print_status.SD_printing!=1)&&(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)==1)&&(print_info.print_paused==1))
        {
          // G92 Z60.50   
          if(print_status.key==0)
                   print_status.key=3;
          //G91 \r\n  G1  Z-0.2 \r\n G90
        
        }
      }
      else
      {
          Time3_Count=0;
          TIM_Cmd(TIM3, ENABLE); 
      }
      
      EXTI_ClearITPendingBit(EXTI_Line10); //清除标志
         
    }
}

void EXTI9_5_IRQHandler(void)
{
  
 
  if(EXTI_GetITStatus(EXTI_Line8) != RESET)
  {
       st_Delay(1000);
        // OK button
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)==0)
	{
                if((print_info.print_paused==1)&&(print_status.SD_printing!=1)) 
                {
                    if(print_status.key==0)
                      print_status.key=1;
                }
	}
	
      EXTI_ClearITPendingBit(EXTI_Line8);
  }
   if(EXTI_GetITStatus(EXTI_Line9) != RESET)
   {
     st_Delay(1000);
     // key up 
     if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9)==0)
     {
      TIM_Cmd(TIM3, DISABLE); 
      if(( print_status.SD_printing!=1)&&(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)==1)&&(print_info.print_paused==1))
      {
        // G92 Z60.50   
        
        
          if(print_status.key==0)
               print_status.key=2;
        
        //G91 \r\n  G1  Z0.2 \r\n G90
      
      }
     }
     else
     {
         Time3_Count=0;
         TIM_Cmd(TIM3, ENABLE); 
     }
	
     EXTI_ClearITPendingBit(EXTI_Line9);
   }
}
/**
  * @brief  HardFault_Handler
  *         This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
   while (1)
  {
  }
}

/**
  * @brief  MemManage_Handler
  *         This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  BusFault_Handler
  *         This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  UsageFault_Handler
  *         This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  SVC_Handler
  *         This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  DebugMon_Handler
  *         This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}


/**
  * @brief  PendSV_Handler
  *         This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}
extern u16 USB_Timers;
void TIM3_IRQHandler(void)
{
      if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
       {
	    Time3_Count++;
		
           //  LED_Control_Function();
              //清除TIM2的中断待处理位
              TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);
	 }
  
}


/**
  * @brief  TIM2_IRQHandler
  *         This function handles Timer2 Handler.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
  USB_OTG_BSP_TimerIRQ();
}


/**
  * @brief  OTG_FS_IRQHandler
  *          This function handles USB-On-The-Go FS global interrupt request.
  *          requests.
  * @param  None
  * @retval None
  */
void OTG_FS_IRQHandler(void)
{
  USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

void Key_Delayt(uint32_t nCount)
{ 
  while(nCount > 0)
  { 
  	  nCount --;   
          
  }

}

extern Receive_C  Receive_CMD[1];




#if 1
void UART4_IRQHandler(void)  
{
        char ch;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//接收到了数据
	{	
		ch = UART4->DR; 
                receing_data(ch);
		//USART_SendData( UART4, (unsigned char)ch);
	        while(!(UART4->SR & USART_FLAG_TXE));
                
                
 		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
                
	}
        if (USART_GetFlagStatus(UART4, USART_FLAG_ORE) != RESET)
        {  
    	    USART_ClearFlag(UART4,USART_FLAG_ORE); //读SR 
            USART_ReceiveData(UART4);  //读DR 
        } 
        
	if (USART_GetITStatus(UART4, USART_IT_TXE) != RESET) 
	{
            USART_ClearITPendingBit(UART4, USART_IT_TXE);           /* Clear the USART transmit interrupt                  */
        }
       
 	
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
