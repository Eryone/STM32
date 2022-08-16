/*-------------------------------------------------------------------------

                            �ӿڲ���
                            
                            
-------------------------------------------------------------------------*/

#include <string.h>

#include "stm32f10x_flash.h"
#include "YModem.h"
#include "common.h"
#include "Download.h"
#include "bsp.h"






/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static FunVoidType JumpToApplication;
static FunVoidType FunReceEnter = NULL;
static FunVoidType FunReceExit = NULL;
static FunWriteType FunWrite = NULL;
static FunProcessType FunCurrentProcess = NULL;

static u32 m_JumpAddress;
static u32 m_ProgramAddr = ApplicationAddress;
static volatile SerialBuffType m_ReceData = SerialBuffDefault();

static volatile eCOM_STATUS m_Mode = eCOMChoose;
static vu32 m_FlashAddress = 0;
static vu32 m_ExtFlashCounter = 0;       //�ⲿFLASH������������

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name :static void Print(u8 *str)
* Description   :��ӡ��Ϣ  ���ڷ���
* Input         :
* Output        :
* Other         :
* Date          :2013.03.01
*******************************************************************************/
static void Print(u8 *str)
{
    u16 len = 0;

    len = strlen((const char *)str);

    while (BspUsart1Send(str, len) != TRUE);
}


/*******************************************************************************
* Function Name :void ReceOneChar(u8 ReceCharacter)
* Description   :���յ�һ���ַ�
* Input         :
* Output        :
* Other         :
* Date          :2013.02.19
*******************************************************************************/
static void ReceOneChar(u8 ReceCharacter)
{
    if (m_ReceData.ind >= USART1_BUFF_LANGTH)
        return;
        
    if (m_ReceData.len > 0)
        return;
        
    m_ReceData.buf[m_ReceData.ind++] = ReceCharacter;
    BspTim3Open();      //��ʱ�����¼���
}

/*******************************************************************************
* Function Name :static void TimEndHandle(void)
* Description   :�����ַ���ʱ�ص�����
* Input         :
* Output        :
* Other         :
* Date          :2013.02.19
*******************************************************************************/
static void TimEndHandle(void)
{
    BspTim3Close(0);

    m_ReceData.len = m_ReceData.ind;
    m_ReceData.ind = 0;
}



/*******************************************************************************
* Function Name :void JumpToApp(void)
* Description   :��ת��Ӧ�ó�����
* Input         :None
* Output        :None
* Other         :None
* Date          :2013.02.19
*******************************************************************************/
static void JumpToApp(void)
{
	Print("ARW bootloader to app\r\n");
unsigned int tmp=(*(vu32*)ApplicationAddress);
    if ((tmp & 0x2FFE0000 ) == 0x20000000)
    { 
     
			
        BspClose();
			 __disable_irq();  
/*				 USART_DeInit(USART1);
					TIM_DeInit(TIM2);
					TIM_DeInit(TIM3);
*/
		     __asm("CPSID  I");   //???NVIC_SETFAULTMASK(); //�����ж�
		 //Print("JumpToApp2\r\n");
        /* Jump to user application */
        m_JumpAddress = *(vu32*) (ApplicationAddress + 4);
        JumpToApplication = (FunVoidType) m_JumpAddress;
		//Print("JumpToApp3\r\n");

        /* Initialize user application's Stack Pointer */
        __set_MSP(*(vu32*) ApplicationAddress);
        JumpToApplication();
    }
}


/*******************************************************************************
* Function Name :static void AppChoose(u8 *pData, u32 *pLen, volatile eCOM_STATUS *peStat)
* Description   :����ѡ��   300ms���� �յ� �ַ�C ����bootload��
* Input         :
* Output        :
* Other         :
* Date          :2013.02.26
*******************************************************************************/
static void AppChoose(u8 *pData, u32 *pLen, volatile eCOM_STATUS *peStat)
{
    static u8 flg = 0;
    
    if (*pLen > 0)
    {
        if ((*pData == 'C') || (*pData == 'c'))
        {
            if (flg == 0)
                flg++;

            if (flg && IS_TIMEOUT_1MS(eTim1, 200))  //����ȷ��
                *peStat = eCOMDisplay;
        }
        *pLen = 0;
    }
    
    if (IS_TIMEOUT_1MS(eTim2, 1000))
    {
        JumpToApp(); 
        Print("\n\r jump app fail!");
        while (1);
     }
}

/*******************************************************************************
* Function Name :static void DisplayMessage(u8 *pData, u32 *pLen, volatile eCOM_STATUS *peStat)
* Description   :��ʾ��ʾ��Ϣ
* Input         :
* Output        :
* Other         :
* Date          :2013.02.26
*******************************************************************************/
static void DisplayMessage(u8 *pData, u32 *pLen, volatile eCOM_STATUS *peStat)
{
    *pLen = 1;
    strcpy((char *)pData, "\r***************** **********\r\n");
    strcat((char *)pData, "1.Update   program.\r\n");  
    strcat((char *)pData, "2.Run application.\r\n");
    strcat((char *)pData, "********************************** ***********\r\n");
    strcat((char *)pData, "please choose:");
    *peStat = eCOMInput;
    
    Print(pData);
    *pLen = 0;
}


/*******************************************************************************
* Function Name :static void InputSelect(u8 *pData, u32 *pLen, volatile eCOM_STATUS *peStat)
* Description   :����ѡ��
* Input         :
* Output        :
* Other         :
* Date          :2013.02.26
*******************************************************************************/
static void InputSelect(u8 *pData, u32 *pLen, volatile eCOM_STATUS *peStat)
{
    if (*pLen > 0)
    {
        switch (*pData)
        {
        case '1': 
            m_ProgramAddr = ApplicationAddress; 
            FunReceEnter = FLASH_ProgramStart;
            FunWrite = FLASH_WriteBank;
            FunReceExit = FLASH_ProgramDone;
            *peStat = eCOMReceive;
            Print("1\r\nPlease select to send the file");
            break;    
            
        case '2': 
            *peStat = eCOMChoose; 
            Print("2\r\nRun the program...");
            break;    
            
        default :break;
        }
        *pLen = 0;
    }
}



/*******************************************************************************
* Function Name :static void ReceiveData(u8 *pData, u32 *pLen, volatile eCOM_STATUS *peStat)
* Description   :YModemЭ��������ݣ����洢����
* Input         :
* Output        :
* Other         :
* Date          :2013.02.26
*******************************************************************************/
static void ReceiveData(u8 *pData, u32 *pLen, volatile eCOM_STATUS *peStat)
{
    u8 pArray[1028] = {0,};
    int len = 0;
    
    switch (YmodemReceive((char *)(pData), (int *)pLen, (char *)pArray, (int *)&len))
    {
    case YM_FILE_INFO: 
        if (FunReceEnter) (*FunReceEnter)();    //��ʼ����
        break;
        
    case YM_FILE_DATA: 
        if (FunWrite) (*FunWrite)(pArray, m_ProgramAddr, len);  //�������ݺ���
        m_ProgramAddr += len;
        break;
        
    case YM_EXIT: 
        if (FunReceExit) (*FunReceExit)();      //������Ϻ���
        
        FunReceEnter = NULL;
        FunWrite = NULL;
        FunReceExit = NULL;
        *peStat = eCOMChoose;
        Print("\r\nrun the program...");
        break;
    }

}

/*******************************************************************************
* Function Name :void CommonInit(void)
* Description   :�ӿڳ�ʼ��
* Input         :
* Output        :
* Other         :
* Date          :2013.02.19
*******************************************************************************/
void CommonInit(void)
{
   BspTim3SetIRQCallBack((void *)TimEndHandle);
    BspUsart1IRQCallBack((void *)ReceOneChar);
}


/*******************************************************************************
* Function Name :void CommonExec(void)
* Description   :�ӿں���
* Input         :
* Output        :
* Other         :
* Date          :2013.02.20
*******************************************************************************/
void CommonExec(void)
{
    switch (m_Mode)
    {
    case eCOMChoose:    //�жϽ��� IAP���� ����APP����
        FunCurrentProcess = AppChoose;
        break;
        
    case eCOMDisplay:   //IAP������ʾ
        FunCurrentProcess = DisplayMessage;
        break;
        
    case eCOMInput:     //IAP����ѡ��
        FunCurrentProcess = InputSelect;
        break;

    case eCOMReceive:   //YMODEM ��������
        FunCurrentProcess = ReceiveData;
        break;
        
    default:
        m_Mode = eCOMChoose;
        break;
    }
    (*FunCurrentProcess)((u8 *)(m_ReceData.buf), (u32 *)&(m_ReceData.len), &m_Mode);
}




/*********************************** END **************************************/

