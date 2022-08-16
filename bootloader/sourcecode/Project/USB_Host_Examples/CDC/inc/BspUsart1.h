#ifndef _DrvUsart1_h_
#define _DrvUsart1_h_

#include "stm32f10x_type.h"



#define USART1_BUFF_LANGTH     1048





typedef struct {
	u32 len;
	u16 ind;	
	u8  buf[USART1_BUFF_LANGTH];
}SerialBuffType;		//·¢ËÍÔÝ´æÇø
#define SerialBuffDefault() {\
	{0,},\
	0,\
	0,\
}


void BspUsart1Init(void);
void BspUsart1Close(void);

u16 BspUsart1Send(u8 *buf, u16 len);
u16 BspUsart1Receive(u8 *buf);

u8 Usart1ReceiveByte(void);
void BspUsart1IRQCallBack(void *fun);


#endif
/********************** END ***************************************************/


