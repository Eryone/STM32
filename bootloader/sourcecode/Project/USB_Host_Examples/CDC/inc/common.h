/*-------------------------------------------------------------------------

                            接口头文件
                            
-------------------------------------------------------------------------*/



#ifndef _COMMON_H_
#define _COMMON_H_

#include "stm32f10x_type.h"


 

/* Constants used by Serial Command Line Mode */
#define CMD_STRING_SIZE         128

#define ApplicationAddress      0x8004000       //APP程序首地址
#define ApplicationSize         256*1024          //程序预留空间
//#define STM32F10X_HD                            //所选择芯片为大容量芯片
//#define ApplicationSize			((*(vu16 *)(0x1FFFF7E0)) * 1000) //芯片存储空间


#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 #define PAGE_SIZE                         (0x400)    /* 1 Kbyte */ fdf
 #define FLASH_SIZE                        (0x20000)  /* 128 KBytes */
#elif defined STM32F10X_CL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */wee
 #define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
#elif defined STM32F10X_HD

 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */ 
 #define FLASH_SIZE                        (0x80000)  /* 512 KBytes */
#elif defined STM32F10X_XL
dd
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x100000) /* 1 MByte */
#else 
 #error "Please select first the STM32 device"    
#endif


#define ADU_LENGTH 0x400




/* Exported types 
------------------------------------------------------------*/
typedef enum {
    eCOMChoose,
    eCOMDisplay,    
    eCOMInput,
    eCOMReceive,
    eCOMFlashAddr,
    eCOMFromFlash,
}eCOM_STATUS;

typedef  void (*FunVoidType)(void);
typedef u32 (*FunWriteType)(u8*, u32, u16);
typedef void (*FunProcessType)(u8 *, u32 *, volatile eCOM_STATUS *);

typedef struct
{
    u32 UpgrageOrNot;			 // 1为有升级   0为没有要升级
    u32 OldOrNew;
    struct FileInfo
    {
        u32 BinLength;			 //文件长度
        u32 CRCValue;			 //CRC校验	
        u32 Version;			 //更新时判断的版本
        u32 BinIndex;
//        u8 Firmware_Version[32]; //固件版本
    }BinInfo[2];
}Update_Set_TypeDef;

/* Exported constants --------------------------------------------------------*/









void CommonInit(void);
void JumpToApp(void);
void CommonExec(void);



#endif
/*********************************** END **************************************/

