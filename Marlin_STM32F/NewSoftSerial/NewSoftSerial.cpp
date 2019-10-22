/*
NewSoftSerial.cpp 
Multi-instance software serial library for Arduino/Wiring
-- Interrupt-driven receive and other improvements by ladyada
   (http://ladyada.net)
-- Tuning, circular buffer, derivation from class Print/Stream,
   multi-instance support, porting to 8MHz processors,
   various optimizations, PROGMEM delay tables, inverse logic and 
   direct port writing by Mikal Hart (http://www.arduiniana.org)
-- Pin change interrupt macros by Paul Stoffregen (http://www.pjrc.com)
-- 20MHz processor support by Garrett Mace (http://www.macetech.com)
-- ATmega1280/2560 support by Brett Hagman (http://www.roguerobotics.com/)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

The latest version of this library can always be found at
http://arduiniana.org.
*/

// When set, _DEBUG co-opts pins 11 and 13 for debugging with an
// oscilloscope or logic analyzer.  Beware: it also slightly modifies
// the bit times, so don't rely on it too much at high baud rates
#define _DEBUG 0
#define _DEBUG_PIN1 11
#define _DEBUG_PIN2 13
// 
// Includes
// 
//#include <avr/interrupt.h>
//#include <avr/pgmspace.h>
//#include "WConstants.h"
//#include "pins_arduino.h"
#include "NewSoftSerial.h"
#include "icrmacros.h"
#include "../Macros.h"
#include "../Marlin.h"

//
// Lookup table
//
typedef struct _DELAY_TABLE
{
  long baud;
  unsigned short rx_delay_centering;
  unsigned short rx_delay_intrabit;
  unsigned short rx_delay_stopbit;
  unsigned short tx_delay;
} DELAY_TABLE;
#if 1//F_CPU == 16000000

static const DELAY_TABLE PROGMEM table[] =
{
  //  baud    rxcenter   rxintra    rxstop    tx
  { 115200,   1,         17,        17,       12,    },
  { 57600,    10,        37,        37,       33,    },
  { 38400,    25,        57,        57,       54,    },
  { 31250,    31,        70,        70,       68,    },
  { 28800,    34,        77,        77,       74,    },
  { 19200,    54,        117,       117,      114,   },
  { 14400,    74,        156,       156,      153,   },
  { 9600,     114,       236,       236,      233,   },
  { 4800,     233,       474,       474,      471,   },
  { 2400,     471,       950,       950,      947,   },
  { 1200,     947,       1902,      1902,     1899,  },
  { 300,      3804,      7617,      7617,     7614,  },
};
#elif F_CPU == 16000000

static const DELAY_TABLE PROGMEM table[] = 
{
  //  baud    rxcenter   rxintra    rxstop    tx
  { 115200,   1,         17,        17,       12,    },
  { 57600,    10,        37,        37,       33,    },
  { 38400,    25,        57,        57,       54,    },
  { 31250,    31,        70,        70,       68,    },
  { 28800,    34,        77,        77,       74,    },
  { 19200,    54,        117,       117,      114,   },
  { 14400,    74,        156,       156,      153,   },
  { 9600,     114,       236,       236,      233,   },
  { 4800,     233,       474,       474,      471,   },
  { 2400,     471,       950,       950,      947,   },
  { 1200,     947,       1902,      1902,     1899,  },
  { 300,      3804,      7617,      7617,     7614,  },
};

const int XMIT_START_ADJUSTMENT = 5;

#elif F_CPU == 8000000

static const DELAY_TABLE table[] PROGMEM = 
{
  //  baud    rxcenter    rxintra    rxstop  tx
  { 115200,   1,          5,         5,      3,      },
  { 57600,    1,          15,        15,     13,     },
  { 38400,    2,          25,        26,     23,     },
  { 31250,    7,          32,        33,     29,     },
  { 28800,    11,         35,        35,     32,     },
  { 19200,    20,         55,        55,     52,     },
  { 14400,    30,         75,        75,     72,     },
  { 9600,     50,         114,       114,    112,    },
  { 4800,     110,        233,       233,    230,    },
  { 2400,     229,        472,       472,    469,    },
  { 1200,     467,        948,       948,    945,    },
  { 300,      1895,       3805,      3805,   3802,   },
};

const int XMIT_START_ADJUSTMENT = 4;

#elif F_CPU == 20000000

// 20MHz support courtesy of the good people at macegr.com.
// Thanks, Garrett!

static const DELAY_TABLE PROGMEM table[] =
{
  //  baud    rxcenter    rxintra    rxstop  tx
  { 115200,   3,          21,        21,     18,     },
  { 57600,    20,         43,        43,     41,     },
  { 38400,    37,         73,        73,     70,     },
  { 31250,    45,         89,        89,     88,     },
  { 28800,    46,         98,        98,     95,     },
  { 19200,    71,         148,       148,    145,    },
  { 14400,    96,         197,       197,    194,    },
  { 9600,     146,        297,       297,    294,    },
  { 4800,     296,        595,       595,    592,    },
  { 2400,     592,        1189,      1189,   1186,   },
  { 1200,     1187,       2379,      2379,   2376,   },
  { 300,      4759,       9523,      9523,   9520,   },
};

const int XMIT_START_ADJUSTMENT = 6;

#else

#error This version of NewSoftSerial supports only 20, 16 and 8MHz processors

#endif

//
// Statics
//
NewSoftSerial *NewSoftSerial::active_object = 0;
char NewSoftSerial::_receive_buffer[_SS_MAX_RX_BUFF]; 
volatile uint8_t NewSoftSerial::_receive_buffer_tail = 0;
volatile uint8_t NewSoftSerial::_receive_buffer_head = 0;

//
// Debugging
//
// This function generates a brief pulse
// for debugging or measuring on an oscilloscope.
inline void DebugPulse(uint8_t pin, uint8_t count)
{
#if _DEBUG
  volatile uint8_t *pport = portOutputRegister(digitalPinToPort(pin));

  uint8_t val = *pport;
  while (count--)
  {
    *pport = val | digitalPinToBitMask(pin);
    *pport = val;
  }
#endif
}

//
// Private methods
//

/* static */ 
inline void NewSoftSerial::tunedDelay(uint16_t delay) { 
  uint8_t tmp=0;

}

// This function sets the current object as the "listening"
// one and returns true if it replaces another 
bool NewSoftSerial::listen()
{
  if (active_object != this)
  {
    _buffer_overflow = false;
    uint8_t oldSREG = SREG;
   // cli();
    _receive_buffer_head = _receive_buffer_tail = 0;
    active_object = this;
    SREG = oldSREG;
    return true;
  }

  return false;
}

//
// The receive routine called by the interrupt handler
//
void NewSoftSerial::recv()
{

#if GCC_VERSION < 40302
// Work-around for avr-gcc 4.3.0 OSX version bug
// Preserve the registers that the compiler misses
// (courtesy of Arduino forum user *etracer*)
  asm volatile(
    "push r18 \n\t"
    "push r19 \n\t"
    "push r20 \n\t"
    "push r21 \n\t"
    "push r22 \n\t"
    "push r23 \n\t"
    "push r26 \n\t"
    "push r27 \n\t"
    ::);
#endif  

  uint8_t d = 0;

  // If RX line is high, then we don't see any start bit
  // so interrupt is probably not for us
  if (_inverse_logic ? rx_pin_read() : !rx_pin_read())
  {
    // Wait approximately 1/2 of a bit width to "center" the sample
    tunedDelay(_rx_delay_centering);
    DebugPulse(_DEBUG_PIN2, 1);

    // Read each of the 8 bits
    for (uint8_t i=0x1; i; i <<= 1)
    {
      tunedDelay(_rx_delay_intrabit);
      DebugPulse(_DEBUG_PIN2, 1);
      uint8_t noti = ~i;
      if (rx_pin_read())
        d |= i;
      else // else clause added to ensure function timing is ~balanced
        d &= noti;
    }

    // skip the stop bit
    tunedDelay(_rx_delay_stopbit);
    DebugPulse(_DEBUG_PIN2, 1);

    if (_inverse_logic)
      d = ~d;

    // if buffer full, set the overflow flag and return
    if ((_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF != _receive_buffer_head) 
    {
      // save new data in buffer: tail points to where byte goes
      _receive_buffer[_receive_buffer_tail] = d; // save new byte
      _receive_buffer_tail = (_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF;
    } 
    else 
    {
#if _DEBUG // for scope: pulse pin as overflow indictator
      DebugPulse(_DEBUG_PIN1, 1);
#endif
      _buffer_overflow = true;
    }
  }

#if GCC_VERSION < 40302
// Work-around for avr-gcc 4.3.0 OSX version bug
// Restore the registers that the compiler misses
  asm volatile(
    "pop r27 \n\t"
    "pop r26 \n\t"
    "pop r23 \n\t"
    "pop r22 \n\t"
    "pop r21 \n\t"
    "pop r20 \n\t"
    "pop r19 \n\t"
    "pop r18 \n\t"
    ::);
#endif
}

void NewSoftSerial::tx_pin_write(uint8_t pin_state)
{
  if (pin_state == LOW)
    *_transmitPortRegister &= ~_transmitBitMask;
  else
    *_transmitPortRegister |= _transmitBitMask;
}

uint8_t NewSoftSerial::rx_pin_read()
{
  return *_receivePortRegister & _receiveBitMask;
}

//
// Interrupt handling
//

/* static */
inline void NewSoftSerial::handle_interrupt()
{
  if (active_object)
  {
    active_object->recv();
  }
}

#if defined(PCINT0_vect)
ISR(PCINT0_vect)
{
  NewSoftSerial::handle_interrupt();
}
#endif

#if defined(PCINT1_vect)
ISR(PCINT1_vect)
{
  NewSoftSerial::handle_interrupt();
}
#endif

#if defined(PCINT2_vect)
ISR(PCINT2_vect)
{
  NewSoftSerial::handle_interrupt();
}
#endif

#if defined(PCINT3_vect)
ISR(PCINT3_vect)
{
  NewSoftSerial::handle_interrupt();
}
#endif

#define BuadRate_9600	 100
#define BuadRate_115200	56//100
//#define BuadRate_9600	100
static u8  fac_us=0;							//usÑÓÊ±±¶³ËÊý			   
static u16 fac_ms=0;							//msÑÓÊ±±¶³ËÊý,ÔÚucosÏÂ,´ú±íÃ¿¸ö½ÚÅÄµÄmsÊý
		   

void delay_init()
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//Ñ¡ÔñÍâ²¿Ê±ÖÓ  HCLK/8
	fac_us=SystemCoreClock/8000000;				//ÎªÏµÍ³Ê±ÖÓµÄ1/8  
	fac_ms=(u16)fac_us*1000;					//·ÇOSÏÂ,´ú±íÃ¿¸ömsÐèÒªµÄsystickÊ±ÖÓÊý   
}	

void delay_us3(u16 time)
{    
   u32 i=0;

     while(time--){
      i=1; while(i--);
     }
}

void delay_us(u32 nus)
{		
	u32 temp;
	if(nus==0)
		return;
	SysTick->LOAD=nus*fac_us/10; 					//Ê±¼ä¼ÓÔØ	  		 
	SysTick->VAL=0x00;        					//Çå¿Õ¼ÆÊýÆ÷
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//¿ªÊ¼µ¹Êý	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//µÈ´ýÊ±¼äµ½´ï   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//¹Ø±Õ¼ÆÊýÆ÷
	SysTick->VAL =0X00;      					 //Çå¿Õ¼ÆÊýÆ÷	 
}

//
// Constructor
//
NewSoftSerial::NewSoftSerial(int16_t receivePin, int16_t transmitPin, bool inverse_logic /* = false */) : 
  _rx_delay_centering(0),
  _rx_delay_intrabit(0),
  _rx_delay_stopbit(0),
  _tx_delay(0),
  _buffer_overflow(false),
  _inverse_logic(inverse_logic)
{
  setTX(transmitPin);
  setRX(receivePin);
  TX_pin=transmitPin;
  RX_pin=receivePin;
}

//
// Destructor
//
NewSoftSerial::~NewSoftSerial()
{
  end();
}

void NewSoftSerial::setTX(uint8_t tx)
{

  pinMode(tx, OUTPUT);
  digitalWrite(tx, HIGH);
 // _transmitBitMask = digitalPinToBitMask(tx);
 // uint8_t port = digitalPinToPort(tx);
 // _transmitPortRegister = portOutputRegister(port);
}

void NewSoftSerial::setRX(uint8_t rx)
{
  pinMode(rx, INPUT);
  if (!_inverse_logic)
    digitalWrite(rx, HIGH);  // pullup for normal logic!
 // _receivePin = rx;
  //_receiveBitMask = digitalPinToBitMask(rx);
  //uint8_t port = digitalPinToPort(rx);
  //_receivePortRegister = portInputRegister(port);
}

//
// Public methods
//
	enum{
	COM_START_BIT,
	COM_D0_BIT,
	COM_D1_BIT,
	COM_D2_BIT,
	COM_D3_BIT,
	COM_D4_BIT,
	COM_D5_BIT,
	COM_D6_BIT,
	COM_D7_BIT,
	COM_STOP_BIT,
};




	
void NewSoftSerial::USART_Send(uint8_t *buf, uint8_t len)
{
	uint8_t t;
	
	for(t = 0; t < len; t++)
	{
		write(buf[t]);
	}
	
}

void NewSoftSerial::begin(long speed)
{
	setTX(TX_pin);
	switch(speed)
	{
		case 115200:
			baud_raterd=BuadRate_115200;//times;
		case 9600:
			baud_raterd=BuadRate_9600;//times;
		default:
			baud_raterd=BuadRate_115200;//times;
	}
	
	delay_init();

	//GPIO_InitTypeDef  GPIO_InitStructure;
			NVIC_InitTypeDef NVIC_InitStructure;
		EXTI_InitTypeDef EXTI_InitStruct;
/*	//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //Ê¹ÄÜPB,PC¶Ë¿ÚÊ±ÖÓ 
		 
	 //SoftWare Serial TXD
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 //ÍÆÍìÊä³ö
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 	 //IO¿ÚËÙ¶ÈÎª50MHz	 
	  GPIO_Init(GPIOC, &GPIO_InitStructure);					
	  GPIO_SetBits(GPIOC,GPIO_Pin_13);						
		 
		 
		//SoftWare Serial RXD
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 	
		GPIO_Init(GPIOB, &GPIO_InitStructure);	 	
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
		EXTI_InitStruct.EXTI_Line = EXTI_Line14;
		EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling; //ÏÂ½µÑØ´¥·¢ÖÐ¶Ï
		EXTI_InitStruct.EXTI_LineCmd=ENABLE;
		EXTI_Init(&EXTI_InitStruct);	
		NVIC_InitStructure.NVIC_IRQChannel= EXTI15_10_IRQn ; 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;  
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  
		NVIC_Init(&NVIC_InitStructure);  
*/

}

void NewSoftSerial::end()
{
 // if (digitalPinToPCMSK(_receivePin))
 //   *digitalPinToPCMSK(_receivePin) &= ~_BV(digitalPinToPCMSKbit(_receivePin));
}


// Read data from buffer
int NewSoftSerial::read()
{
  if (!is_listening())
    return -1;

  // Empty buffer?
  if (_receive_buffer_head == _receive_buffer_tail)
    return -1;

  // Read from "head"
  uint8_t d = _receive_buffer[_receive_buffer_head]; // grab next byte
  _receive_buffer_head = (_receive_buffer_head + 1) % _SS_MAX_RX_BUFF;
  return d;
}

int NewSoftSerial::available()
{
  if (!is_listening())
    return 0;

  return (_receive_buffer_tail + _SS_MAX_RX_BUFF - _receive_buffer_head) % _SS_MAX_RX_BUFF;
}




int NewSoftSerial::write(uint8_t Data)
{
	u8 i = 0;
	asm("cpsid i");
	WRITE_IO(TX_pin, 0);
	delay_us(baud_raterd);
//	for(i = 0; i < 8; i++)
	{
		WRITE_IO(TX_pin, Data&0x01);		
		delay_us(baud_raterd);
		Data = Data>>1;
		WRITE_IO(TX_pin, Data&0x01);		
		delay_us(baud_raterd);
		Data = Data>>1;
		WRITE_IO(TX_pin, Data&0x01);		
		delay_us(baud_raterd);
		Data = Data>>1;
		WRITE_IO(TX_pin, Data&0x01);		
		delay_us(baud_raterd);
		Data = Data>>1;
		WRITE_IO(TX_pin, Data&0x01);		
		delay_us(baud_raterd);
		Data = Data>>1;
		WRITE_IO(TX_pin, Data&0x01);		
		delay_us(baud_raterd);
		Data = Data>>1;
		WRITE_IO(TX_pin, Data&0x01);		
		delay_us(baud_raterd);
		Data = Data>>1;
		WRITE_IO(TX_pin, Data&0x01);		
		delay_us(baud_raterd);
		//Data = Data>>1;
	}
	WRITE_IO(TX_pin, 1);
	delay_us(baud_raterd);
	asm("cpsie i");
	return 0;
}

#if 0//!defined(cbi)
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void NewSoftSerial::enable_timer0(bool enable) 
{
	/*
  if (enable)
#if defined(__AVR_ATmega8__)
    sbi(TIMSK, TOIE0);
#else
    sbi(TIMSK0, TOIE0);
#endif
  else 
#if defined(__AVR_ATmega8__)
    cbi(TIMSK, TOIE0);
#else
    cbi(TIMSK0, TOIE0);
#endif
*/
}

void NewSoftSerial::flush()
{
  if (!is_listening())
    return;

  //uint8_t oldSREG = SREG;
 // cli();
  _receive_buffer_head = _receive_buffer_tail = 0;
 // SREG = oldSREG;
}

int NewSoftSerial::peek()
{
  if (!is_listening())
    return -1;

  // Empty buffer?
  if (_receive_buffer_head == _receive_buffer_tail)
    return -1;

  // Read from "head"
  return _receive_buffer[_receive_buffer_head];
}
