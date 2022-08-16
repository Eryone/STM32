/****************************************Copyright (c)**************************************************
** Modified by:        王宏强
** Modified date:      2012-05-20 
** Version:            v3.0
** Descriptions:       修改用于STM32F10x   
**
**------------------------------------------------------------------------------------------------------
** Modified by: 		
** Modified date:	   
** Version: 		   
** Descriptions:		
**
********************************************************************************************************/

#ifndef _bsp_h_
#define _bsp_h_
#include "stm32f10x_type.h"


#include "BspTime2.h"
#include "BspTime3.h"
#include "BspUsart1.h"



#ifndef NULL
#define NULL        ((void *)0)
#endif

/********************************/
/*       系统配置函数           */
/********************************/
#define LED_0 0
#define LED_1 1
#define LED_2 2
#define LED_3 3
#define LED_4 4
#define LED_5 5


void Delay(vu32 nCount);
void IWDG_Init(void);
void SysTickInit(void);
void BspInit(void);
void LedInit(void);
void LedOn(u32 n);
void LedOff(u32 n);
void BspClose(void);




#endif

/*********************************** END **************************************/



