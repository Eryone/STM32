/*-------------------------------------------------------------------------

                            接口部分
                            
                            
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
static vu32 m_ExtFlashCounter = 0;       //外部FLASH擦除的扇区号

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name :static void Print(u8 *str)
* Description   :打印消息  串口发送
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
* Description   :接收到一个字符
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
    BspTim3Open();      //定时器重新计数
}

/*******************************************************************************
* Function Name :static void TimEndHandle(void)
* Description   :接收字符超时回调函数
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
* Description   :跳转到应用程序区
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
		     __asm("CPSID  I");   //???NVIC_SETFAULTMASK(); //关总中断
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
* Description   :操作选择   300ms内有 收到 字符C 进入bootload区
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

            if (flg && IS_TIMEOUT_1MS(eTim1, 200))  //二次确认
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
* Description   :显示提示消息
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
* Description   :操作选择
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
* Description   :YModem协议接收数据，并存储数据
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
        if (FunReceEnter) (*FunReceEnter)();    //开始函数
        break;
        
    case YM_FILE_DATA: 
        if (FunWrite) (*FunWrite)(pArray, m_ProgramAddr, len);  //接收数据函数
        m_ProgramAddr += len;
        break;
        
    case YM_EXIT: 
        if (FunReceExit) (*FunReceExit)();      //接收完毕函数
        
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
* Description   :接口初始化
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
* Description   :接口函数
* Input         :
* Output        :
* Other         :
* Date          :2013.02.20
*******************************************************************************/
void CommonExec(void)
{
    switch (m_Mode)
    {
    case eCOMChoose:    //判断进入 IAP程序 还是APP程序
        FunCurrentProcess = AppChoose;
        break;
        
    case eCOMDisplay:   //IAP操作显示
        FunCurrentProcess = DisplayMessage;
        break;
        
    case eCOMInput:     //IAP操作选择
        FunCurrentProcess = InputSelect;
        break;

    case eCOMReceive:   //YMODEM 接收数据
        FunCurrentProcess = ReceiveData;
        break;
        
    default:
        m_Mode = eCOMChoose;
        break;
    }
    (*FunCurrentProcess)((u8 *)(m_ReceData.buf), (u32 *)&(m_ReceData.len), &m_Mode);
}




/*********************************** END **************************************/

