/*
********************************************************************************
*                           
*                              YModem Э�鲿��
*
* File          : YModem.C
* Version       : V1.0
* By            : whq
*
* For           : Stm32f10x
* Mode          : Thumb2
* Toolchain     : 
*                   RealView Microcontroller Development Kit (MDK)
*                   Keil uVision
* Description   : 
*
*
* Date          : 2013.2.20
*******************************************************************************/



#include <string.h>

#include "YModem.h"
#include "common.h"
#include "bsp.h"

#define SendString(a, b)    BspUsart1Send(a, b)     //���ڷ��ͺ��� a:�ַ��� b:����
#define ReceiveString(a)    BspUsart1Receive(a)     //���ڽ��պ���


// static const unsigned short CRC16TAB[ 256 ] =
// {
//     0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
//     0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
//     0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
//     0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
//     0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
//     0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
//     0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
//     0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
//     0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
//     0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
//     0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
//     0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
//     0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
//     0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
//     0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
//     0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
//     0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
//     0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
//     0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
//     0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
//     0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
//     0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
//     0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
//     0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
//     0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
//     0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
//     0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
//     0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
//     0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
//     0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
//     0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
//     0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
// };

// /*******************************************************************************
// * Function Name :unsigned short YModemCrc(char c, unsigned short crc)
// * Description   :Ymodem crc���ֽ�Ч��
// * Input         :
// * Output        :
// * Other         :
// * Date          :2013.02.20
// *******************************************************************************/
// static unsigned short YModemCrc(char *pData, unsigned short sLen)
// {
//     unsigned short i;
//     unsigned short CRC = 0x00;
//     
//     for( i = 0; i < sLen; i++ )
//     {
//         CRC = ( CRC << 8 ) ^ CRC16TAB[( ( CRC >> 8 ) ^ *pData++ ) & 0xff ];
//     }

//     return CRC;
// }

/*******************************************************************************
* Function Name :unsigned short YModemCrc(char c, unsigned short crc)
* Description   :Ymodem crc���ֽ�Ч��
* Input         :
* Output        :
* Other         :
* Date          :2013.02.20
*******************************************************************************/
static unsigned short YModemCrc(char *pData, unsigned short sLen)
{  
  
      unsigned short CRC1 = 0;
     unsigned short i = 0;

   while (sLen--)  //len����Ҫ����ĳ���
   {
       CRC1 = CRC1^(int)(*pData++) << 8; //    
       for (i=8; i!=0; i--) 
       {
           if (CRC1 & 0x8000)   
               CRC1 = CRC1 << 1 ^ 0x1021;    
           else
               CRC1 = CRC1 << 1;
       }    
   }

   return CRC1;  
}

/*******************************************************************************
* Function Name :static char ReceivePacket (char *pRece, unsigned short sReceLen, char *pData, unsigned short *sLen, char cNum)
* Description   :�������ݰ�
* Input         :pData:���ݰ��� sLen:�ܳ��ȣ�cNum:��ǰ���
* Output        :
* Other         :
* Date          :2013.02.20
*******************************************************************************/
static int ReceivePacket (char *pRece, int sReceLen, char *pData, int *sLen, char cNum)
{
    int len = 0;            //���ݵĳ���
    int tmp = 0;

    if (pRece[0] == SOH)
    {
        len = PACKET_SIZE;
    } 
    else if (pRece[0] == STX)
    {
        len = PACKET_1K_SIZE;
    }
    else if (pRece[0] == EOT)   
    {
        return FALSE;           //���յ�������־
    }
    else if (pRece[0] == ETX)   
    {
        return FALSE;           //���յ���ֹ��־ ctrl + c
    }
    else if (pRece[0] == CA)    //���յ���ֹ��־
    {
        if (pRece[1] == CA)     //ȷ����ֹ��־
            return FALSE; 
        else
            return -6;          //���յ���ֹ�����־
    }
    else
    {
        return -1;              //����ͷ����
    }

    if (pRece[1] != cNum)
        return -2;              //���ݱ�Ŵ���
     
    if (pRece[2] != (char)(~cNum))
        return -3;              //���ݱ�Ų������

    if (sReceLen != (len + PACKET_OVERHEAD))
        return -4;              //�����ܳ��ȴ���

    // YModemֻЧ�����ݲ���
    tmp = YModemCrc(pRece + PACKET_HEADER, len);
    if (((pRece[sReceLen - 2] << 8) |pRece[sReceLen - 1]) != tmp)
        return -5;              //crcЧ�����

    memcpy(pData, &pRece[PACKET_HEADER], len);  //��������
    *sLen = len;

    return TRUE;
}


/*******************************************************************************
* Function Name :int YmodemSendChar(unsigned char ch, eYM_STAT *stat, int *errCount)
* Description   :����һ���ַ� ���ж� �����������
* Input         :
* Output        :
* Other         :
* Date          :2013.02.24
*******************************************************************************/
void YmodemSendChar(unsigned char ch, eYM_STAT *stat, int *errCount)
{
    while(TRUE != SendString(&ch, 1));
 
    if (ch == ACK)          //�������������
    {
        *errCount = 0;
    }
    else if (ch == NAK)     //����������ۼ�
    {
        (*errCount)++;
        if ((*errCount) > MAX_ERRORS)  //�������������
        {
            *stat = eYM_END;
        }
    }
}

/*******************************************************************************
* Function Name :char YmodemReceive(char *buff, unsigned short *len)
* Description   :YModem �������
* Input         :pRece:���յ�������֡   pData:֡�е����ݲ���
* Output        :res < 0: ���ս��� res>0 :���յ���Ч����
* Other         :
* Date          :2013.02.19
*******************************************************************************/
int YmodemReceive(char *pRece, int *sReceLen, char *pData, int *sResLen)
{
    int cRes = YM_VOIDER;
    static eYM_STAT stat= eYM_INIT;          //��ǰ״̬
    
    static char affirmCount = 0;                //ȷ�ϴ���    
    static int iNumber = 0;                     //���ݰ����
    static int sErrorCount = 0;                 //���������

    s8 cTmp = 0;
    
    switch (stat)
    {
    case eYM_INIT:
        iNumber = 0;
        sErrorCount = 0;   
        affirmCount = 0;
        stat = eYM_RECE_HEAD_PACKET;
        IS_TIMEOUT_1MS(eTimYModem, 0);          //�峬ʱ������
        break;

    case eYM_RECE_HEAD_PACKET:
        if (IS_TIMEOUT_1MS(eTimYModem, NAK_TIMEOUT))            //���ȴ���ʱ 
        {         
            YmodemSendChar(CRC16, &stat, &sErrorCount);         //���� 'C'
            stat = eYM_INIT;
            break;
        }
            
        if (*sReceLen == 0)
            break;
  
        cTmp = ReceivePacket(pRece, *sReceLen, pData, sResLen, iNumber & 0xFF);
        *sReceLen = 0;                  //�������ݺ��������������
        switch (cTmp)
        {
        case TRUE:                      //������ȷ
            if (pRece[PACKET_HEADER] == 0)                      //���ļ�����
                stat = eYM_END;
            else
            {
                stat = eYM_RECE_DATA_START;                     //���ļ�����
                cRes = YM_FILE_INFO;    //���ؽ���ͷ�ļ�
                iNumber++;
                affirmCount = 0;           
            }
            break;

        case 0:                         //���յ�������־
            if (affirmCount)
            {
                iNumber = 0;  
                YmodemSendChar(ACK, &stat, &sErrorCount);       //��ȷӦ��
            }
            else
            {       
                YmodemSendChar(NAK, &stat, &sErrorCount);       //����Ӧ�� ����ȷ��
            }
            affirmCount++;
            break;

        default:                        //������������  
            affirmCount = 0;
            YmodemSendChar(NAK, &stat, &sErrorCount);           //����Ӧ��
            break;
        }
        IS_TIMEOUT_1MS(eTimYModem, 0);                          //�峬ʱ������
        break;
        
    case eYM_RECE_DATA_START:
        stat = eYM_RECE_DATA;
        YmodemSendChar(ACK, &stat, &sErrorCount);               //��ȷӦ��
        YmodemSendChar(CRC16, &stat, &sErrorCount);             //���� 'C' 
        break;
                
    case eYM_RECE_DATA:
        if (IS_TIMEOUT_1MS(eTimYModem, NAK_TIMEOUT))            //���ȴ���ʱ 
        {
            stat = eYM_END;
            break;
        }
            
        if (*sReceLen == 0)
            break;
  
        cTmp = ReceivePacket(pRece, *sReceLen, pData, sResLen, iNumber & 0xFF);
        *sReceLen = 0;                  //�������ݺ��������������
        switch (cTmp)
        {
        case TRUE:                      //������ȷ
            cRes = YM_FILE_DATA;        //���ؽ���������ȷ
            iNumber++;                  //
            affirmCount = 0;
            YmodemSendChar(ACK, &stat, &sErrorCount);        //��ȷӦ��
            break;

        case 0:                         //���յ�������־
            if (affirmCount)
            {
                iNumber = 0;
                stat = eYM_RECE_HEAD_PACKET;
                YmodemSendChar(ACK, &stat, &sErrorCount);    //��ȷӦ��
            }
            else
            {
                YmodemSendChar(NAK, &stat, &sErrorCount);    //����Ӧ��
            }
            affirmCount++;
            break;

        default:                        //������������  
            affirmCount = 0;
            YmodemSendChar(NAK, &stat, &sErrorCount);       //����Ӧ��
            break;
        } 
        IS_TIMEOUT_1MS(eTimYModem, 0);                      //�峬ʱ������        
        break;
        
    case eYM_END:
        YmodemSendChar(ACK, &stat, &sErrorCount);           //��ֹ
        YmodemSendChar(CA, &stat, &sErrorCount);            //��ֹ
        YmodemSendChar(CA, &stat, &sErrorCount);            //��ֹ
        cRes = YM_EXIT;
        stat = eYM_INIT;
        break;
    }
    
    return cRes;
}


/********************** END ***************************************************/


