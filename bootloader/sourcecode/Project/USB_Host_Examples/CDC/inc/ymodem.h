/*-------------------------------------------------------------------------
                        YModem 协议头文件
-------------------------------------------------------------------------*/



#ifndef _YMODEM_H_
#define _YMODEM_H_




/* Includes ------------------------------------------------------------------*/

#include "stm32f10x_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)
#define PACKET_TRAILER          (2)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define FILE_NAME_LENGTH        (100)
#define FILE_SIZE_LENGTH        (16)

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define ETX                     (0x03)  /* end of Text */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CA                      (0x18)  /* two of these in succession aborts transfer */
#define CRC16                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  (0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2                  (0x61)  /* 'a' == 0x61, abort by user */

#define NAK_TIMEOUT             (1000)   // 1s
#define MAX_ERRORS              (5)

#define YM_FILE_INFO            (2)
#define YM_FILE_DATA            (1)
#define YM_VOIDER               (0)
#define YM_EXIT                 (-1)


typedef enum {
    eYM_INIT,   
    eYM_RECE_HEAD_PACKET,
    eYM_RECE_DATA_START,
    eYM_RECE_DATA,
    eYM_END,   
}eYM_STAT;



int YmodemReceive(char *pRece, int *sReceLen, char *pData, int *sResLen);


#endif
/********************** END ***************************************************/

