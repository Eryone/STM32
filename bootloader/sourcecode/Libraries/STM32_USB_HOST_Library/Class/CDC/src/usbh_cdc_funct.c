/**
  ******************************************************************************
  * @file    usbh_cdc_funct.c
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    09-November-2015
    * @brief   This file is the CDC Layer Handlers for USB Host CDC class.
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
#include "usbh_cdc_funct.h"
#include "string.h"
#include "data_handle.h"
/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup CDC_CLASS
* @{
*/

/** @defgroup CDC_CORE 
  * @brief    This file includes HID Layer Handlers for USB Host HID class.
* @{
*/ 

/** @defgroup CDC_CORE_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup CDC_CORE_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup CDC_CORE_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup CDC_CORE_Private_Variables
* @{
*/
CDC_InterfaceDesc_Typedef         CDC_Desc;
CDC_LineCodingTypeDef             CDC_GetLineCode;
CDC_LineCodingTypeDef             CDC_SetLineCode;
extern setting_t Setting;

extern CDC_Requests               CDC_ReqState;
extern PRINT_INFO_D print_info;
/**
* @}
*/ 


/** @defgroup CDC_CORE_Private_FunctionPrototypes
* @{
*/ 

/**
* @}
*/ 

extern char driver_string[64];

int  SetBaudRate(int baud) 
{
      short baud_value;//, baud_index = 0;
      int divisor3;

      divisor3 = 48000000 / 2 / baud; // divisor shifted 3 bits to the left

       {
              static const uint8_t divfrac [8] = {0, 3, 2, 0, 1, 1, 2, 3};
              //static const uint8_t divindex[8] = {0, 0, 0, 1, 0, 1, 1, 1};

              baud_value = divisor3 >> 3;
              baud_value |= divfrac [divisor3 & 0x7] << 14;
              //baud_index = divindex[divisor3 & 0x7];

              /* Deal with special cases for highest baud rates. */
            /*  if(baud_value == 1)
                baud_value = 0;
              else // 1.0
                if(baud_value == 0x4001) 
                  baud_value = 1; // 1.5*/
               
      }
     return     baud_value;
}

/** @defgroup CDC_CORE_Private_Functions
* @{
*/ 
/**
  * @brief  This request allows the host to find out the currently 
  *         configured line coding.
  * @param  pdev: Selected device
  * @retval USBH_Status : USB ctl xfer status
  */
USBH_Status CDC_GETLineCoding(USB_OTG_CORE_HANDLE *pdev , USBH_HOST *phost)
{
    ////for  PL2303	   
		phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | \
                              USB_REQ_RECIPIENT_INTERFACE;
  
		phost->Control.setup.b.bRequest = CDC_GET_LINE_CODING;
		phost->Control.setup.b.wValue.w = 0;
		phost->Control.setup.b.wIndex.w = 0; /*At to be checked*/
		phost->Control.setup.b.wLength.w = LINE_CODING_STRUCTURE_SIZE;           
  
  	return USBH_CtlReq(pdev, phost, CDC_GetLineCode.Array, LINE_CODING_STRUCTURE_SIZE);
}
USBH_Status CDC_PL2303_clear(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x00;
  	phost->Control.setup.b.bRequest = 0x01;
  	phost->Control.setup.b.wValue.w = 0x01;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x00;    
  return USBH_CtlReq(pdev, phost, 0 , 0 );  
}
USBH_Status CDC_PL2303_SetControl(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x21;
  	phost->Control.setup.b.bRequest = 0x22;
  	phost->Control.setup.b.wValue.w = 0x01;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x00;    
  return USBH_CtlReq(pdev, phost, 0 , 0 );  
}
USBH_Status CDC_PL2303_GETLine(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{

  	phost->Control.setup.b.bmRequestType = 0xA1;
  	phost->Control.setup.b.bRequest = 0x21;
  	phost->Control.setup.b.wValue.w = 0x00;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x07;
	
	return USBH_CtlReq(pdev, phost, CDC_GetLineCode.Array, 0x07);
}

USBH_Status CDC_PL2303_SetLine(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
	 if(print_info.baud_rate==250000)
  		CDC_SetLineCode.b.dwDTERate =250000;
 	 else if(print_info.baud_rate==256000)
  		CDC_SetLineCode.b.dwDTERate =256000;
 	 else //115200
  		CDC_SetLineCode.b.dwDTERate =115200;
	 
  		CDC_SetLineCode.b.bDataBits =8;
  		CDC_SetLineCode.b.bParityType =0 ;  
  		CDC_SetLineCode.b.bCharFormat =0; 
  	phost->Control.setup.b.bmRequestType = 0x21;
  	phost->Control.setup.b.bRequest = 0x20;
  	phost->Control.setup.b.wValue.w = 0;
  	phost->Control.setup.b.wIndex.w = 0;
  	phost->Control.setup.b.wLength.w = 0x07;   
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 7 );  
}
USBH_Status CDC_PL2303_0(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
phost->Control.setup.b.bmRequestType = 0x40;
    phost->Control.setup.b.bRequest = 0x01;
    phost->Control.setup.b.wValue.w = 0x0505;//0909;//0x0b0b;
    phost->Control.setup.b.wIndex.w = 0x1311;//0x02;
    phost->Control.setup.b.wLength.w = 0;
	
    return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8);
}
USBH_Status CDC_SETCP2102_1(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x41;
  	phost->Control.setup.b.bRequest = 0x00;
  	phost->Control.setup.b.wValue.w = 0x01;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x00;    
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8 );   
}
USBH_Status CDC_SETCP2102_2(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x41;
  	phost->Control.setup.b.bRequest = 0x03;
  	phost->Control.setup.b.wValue.w = 0x0800;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x00;    
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8 );  
}
USBH_Status CDC_SETCP2102_3(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x41;
  	phost->Control.setup.b.bRequest = 0x13;
  	phost->Control.setup.b.wValue.w = 0x00;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x10;    
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8);  
}
USBH_Status CDC_SETCP2102_4(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x41;
  	phost->Control.setup.b.bRequest = 0x19;
  	phost->Control.setup.b.wValue.w = 0x00;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x06;    
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8);  
}
USBH_Status CDC_SETCP2102_5(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x41;
  	phost->Control.setup.b.bRequest = 0x1e;
  	phost->Control.setup.b.wValue.w = 0x00;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x04;    
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8);  
}
USBH_Status CDC_SETCP2102_6(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x41;
  	phost->Control.setup.b.bRequest = 0x1e;
  	phost->Control.setup.b.wValue.w = 0x00;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x04;    
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8);  
}
USBH_Status CDC_SETCP2102_7(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x41;
  	phost->Control.setup.b.bRequest = 0x07;
  	phost->Control.setup.b.wValue.w = 0x0200;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x00;    
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8);  
}
USBH_Status CDC_SETCP2102_8(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x41;
  	phost->Control.setup.b.bRequest = 0x07;
  	phost->Control.setup.b.wValue.w = 0x0101;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x00;    
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8);  
}
USBH_Status CDC_SETCP2102_9(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x41;
  	phost->Control.setup.b.bRequest = 0x03;
  	phost->Control.setup.b.wValue.w = 0x0800;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x00;    
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8);  
}
USBH_Status CDC_SETCP2102_10(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x41;
  	phost->Control.setup.b.bRequest = 0x19;
  	phost->Control.setup.b.wValue.w = 0x00;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x06;    
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8);  
}
USBH_Status CDC_SETCP2102_11(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  	phost->Control.setup.b.bmRequestType = 0x41;
  	phost->Control.setup.b.bRequest = 0x07;
  	phost->Control.setup.b.wValue.w = 0x0202;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x00;    
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8);  
}
 uint8_t SETLINE_REQUEST[13][8]=
{
	{0x00,0x01,0x01,0x00,  0x00,0x00,0x00,0x00},//0
	{0x21,0x20,0x00,0x00,  0x00,0x00,0x07,0x00},// 1
		{0xb0,0x04,0x00,0x00,  0x00,0x02,0x07,   0x00},// 2
	{0x21,0x22,0x01,0x00,  0x00,0x00,0x00,0x00},// 3
	{0x21,0x22,0x03,0x00,  0x00,0x00,0x00,0x00},// 4
	{0x21,0x20,0x00,0x00,  0x00,0x00,0x07,0x00},// 5
		{0xb0,0x04,0x00,0x00,  0x00,0x02,0x07,   0x00},// 6
	{0x21,0x20,0x00,0x00,  0x00,0x00,0x07,0x00}, // 7
		{0x00,0xc2,0x01,0x00,  0x00,0x02,0x07,   0x00}, // 8
	{0x21,0x22,0x01,0x00,  0x00,0x00,0x00,0x00}, //9
	{0x21,0x20,0x00,0x00,  0x00,0x00,0x07,0x00}, //10 
		{0x00,0xc2,0x01,0x00,  0x00,0x00,0x08,0x00},// 11
	{0x40,0x01,0x05,0x05,  0x11,0x13,0x00,0x00} //12
};

USBH_Status CDC_PL2303(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost,uint8_t x[8],uint8_t y[8])
{  
  phost->Control.setup.b.bmRequestType = x[0];
  phost->Control.setup.b.bRequest = x[1];
  phost->Control.setup.b.wValue.w =(u16) ((u16)(x[3]<<8)|x[2]);
  phost->Control.setup.b.wIndex.w = (u16) ((u16)(x[5]<<8)|x[4]);
  phost->Control.setup.b.wLength.w = (u16) ((u16)(x[7]<<8)|x[6]);          
		CDC_SetLineCode.Array[0]=y[0];
		CDC_SetLineCode.Array[1]=y[1];
		CDC_SetLineCode.Array[2]=y[2];
		CDC_SetLineCode.Array[3]=y[3];
		CDC_SetLineCode.Array[4]=y[4];
		CDC_SetLineCode.Array[5]=y[5];
		CDC_SetLineCode.Array[6]=y[6];
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , phost->Control.setup.b.wLength.w);  
}


USBH_Status control_PL2303(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{
static u8 state_flag=1;
      USBH_Status ret;
  switch(state_flag)
  {
  	case 1:
  		ret=CDC_PL2303(pdev, phost,SETLINE_REQUEST[0],0);
		if(ret==USBH_OK)
			state_flag=2;
		return USBH_BUSY;
    		break;
	case 2:
  		ret=CDC_PL2303(pdev, phost,SETLINE_REQUEST[1],SETLINE_REQUEST[2]);
		if(ret==USBH_OK)
			state_flag=3;
		return USBH_BUSY;
		break;
	case 3:
  		ret=CDC_PL2303(pdev, phost,SETLINE_REQUEST[3],0);
		if(ret==USBH_OK)
			state_flag=4;
		return USBH_BUSY;
		break;
	case 4:
  		ret=CDC_PL2303(pdev, phost,SETLINE_REQUEST[4],0);
		if(ret==USBH_OK)
			state_flag=5;
		return USBH_BUSY;
		break;
	case 5:
  		ret=CDC_PL2303(pdev, phost,SETLINE_REQUEST[5],SETLINE_REQUEST[6]);
		if(ret==USBH_OK)
			state_flag=6;
		return USBH_BUSY;
		break;
	case 6:
  		ret=CDC_PL2303(pdev, phost,SETLINE_REQUEST[7],SETLINE_REQUEST[8]);
		if(ret==USBH_OK)
			state_flag=7;
		return USBH_BUSY;
		break;
	case 7:
  		ret=CDC_PL2303(pdev, phost,SETLINE_REQUEST[9],0);
		if(ret==USBH_OK)
			state_flag=8;
		return USBH_BUSY;
		break;
	case 8:
  		ret=CDC_PL2303(pdev, phost,SETLINE_REQUEST[10],SETLINE_REQUEST[11]);
		if(ret==USBH_OK)
			state_flag=9;
		return USBH_BUSY;
		break;
	case 9:
  		ret=CDC_PL2303(pdev, phost,SETLINE_REQUEST[12],0);
		if(ret==USBH_OK)
			state_flag=10;
		return USBH_BUSY;
		break;
	default:
			state_flag=0;
			return USBH_OK;
		break;
	}

}

USBH_Status FTDISetBaudRate(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost, unsigned long ulBaudRate)
{
    unsigned long baud_value;
    unsigned long baud_index = 0;
    unsigned long divisor3;

    //
    // divisor shifted 3 bits to the left
    //
    divisor3 = 48000000 / 2 / ulBaudRate; 

    static const unsigned char divfrac [8] = { 0, 3, 2, 0, 1, 1, 2, 3 };
    static const unsigned char divindex[8] = { 0, 0, 0, 1, 0, 1, 1, 1 };

    baud_value	= divisor3 >> 3;
    baud_value |= divfrac [divisor3 & 0x7] << 14;
    baud_index	= divindex[divisor3 & 0x7];

    //
    // Deal with special cases for highest baud rates.
    //
    if (baud_value == 1)
        baud_value = 0; else	// 1.0
          
    if (baud_value == 0x4001)
        baud_value = 1;		// 1.5
        
    //
    // This is a Class specific interface OUT request.
    //
    phost->Control.setup.b.bmRequestType = 0x40;
//#define bmREQ_FTDI_OUT  0x40
//#define bmREQ_FTDI_IN   0xc0
    //
    // Request a set baud rate
    //#define FTDI_SIO_SET_BAUD_RATE		3 /* Set baud rate */
    phost->Control.setup.b.bRequest = 3;//FTDI_SIO_SET_BAUD_RATE;
    phost->Control.setup.b.wValue.w = baud_value;

    //
    // Indicate the index
    //
    phost->Control.setup.b.wIndex.w = baud_index;

    //
    // Zero bytes request
    //
    phost->Control.setup.b.wLength.w = 0;

    //
    // Put the setup packet in the buffer and send the command.
  		CDC_SetLineCode.b.dwDTERate = CDC_GetLineCode.b.dwDTERate=115200;
	 
  		CDC_SetLineCode.b.bDataBits = CDC_GetLineCode.b.bDataBits=8;
  		CDC_SetLineCode.b.bParityType = CDC_GetLineCode.b.bParityType=0 ;  
  		CDC_SetLineCode.b.bCharFormat = CDC_GetLineCode.b.bCharFormat=0; 
    return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , phost->Control.setup.b.wLength.w);  
}
/**
  * @brief  This request allows the host to specify typical asynchronous 
  *         line-character formatting properties 
  *         This request applies to asynchronous byte stream data class interfaces 
  *         and endpoints
  * @param  pdev: Selected device
  * @retval USBH_Status : USB ctl xfer status
  */
USBH_Status CDC_SETLineCoding(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{

#if 0
  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | \
                              USB_REQ_RECIPIENT_INTERFACE;
  
  phost->Control.setup.b.bRequest = CDC_SET_LINE_CODING;
  phost->Control.setup.b.wValue.w = 0;

  phost->Control.setup.b.wIndex.w = 0;

  phost->Control.setup.b.wLength.w = LINE_CODING_STRUCTURE_SIZE;       
		CDC_SetLineCode.Array[0]=CDC_GetLineCode.Array[0]=0x00;
		CDC_SetLineCode.Array[1]=CDC_GetLineCode.Array[0]=0xc2;
		CDC_SetLineCode.Array[2]=CDC_GetLineCode.Array[0]=0x01;
		CDC_SetLineCode.Array[3]=CDC_GetLineCode.Array[0]=0x00;
		CDC_SetLineCode.Array[4]=CDC_GetLineCode.Array[0]=0x00;
		CDC_SetLineCode.Array[5]=CDC_GetLineCode.Array[0]=0x00;
		CDC_SetLineCode.Array[6]=CDC_GetLineCode.Array[0]=0x08;
  return USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array, LINE_CODING_STRUCTURE_SIZE);
#else
    USBH_Status ret;
	static u8 i=0;

    if(Setting.Setline.b.Open_flag==1)
  {
  	 if(print_info.baud_rate==250000)
  		CDC_SetLineCode.b.dwDTERate =250000;
 	 else if(print_info.baud_rate==256000)
  		CDC_SetLineCode.b.dwDTERate =256000;
 	 else //115200
  		CDC_SetLineCode.b.dwDTERate =115200;
	 
  		CDC_SetLineCode.b.bDataBits =8;
  		CDC_SetLineCode.b.bParityType =0 ;  
  		CDC_SetLineCode.b.bCharFormat =0; 
       phost->Control.setup.b.bmRequestType = Setting.Setline.b.bmRequestType;
	phost->Control.setup.b.bRequest = Setting.Setline.b.bRequest;
	phost->Control.setup.b.wValue.w = Setting.Setline.b.wValue;
	phost->Control.setup.b.wIndex.w = Setting.Setline.b.wIndex;
	phost->Control.setup.b.wLength.w = Setting.Setline.b.wLength;
  	ret=USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8 );  
   }
  else if((strncmp(driver_string,"USB2.0-Serial",strlen("USB2.0-Serial"))==0)||\
          (phost->device_prop.Dev_Desc.idVendor==0x1A86&&phost->device_prop.Dev_Desc.idProduct==0x5523)||\
          (phost->device_prop.Dev_Desc.idVendor==0x1A86&&phost->device_prop.Dev_Desc.idProduct==0x7523)) ////for  CH340    CH341
  {
 	 if(print_info.baud_rate==250000)// 
  	{
    		phost->Control.setup.b.bmRequestType = 0x40;
    		phost->Control.setup.b.bRequest = 0x9a;
    		phost->Control.setup.b.wValue.w = 0x1312;
    		phost->Control.setup.b.wIndex.w = 0xe883;
    		phost->Control.setup.b.wLength.w = 0x00;
  	}
 	 else if(print_info.baud_rate==256000)
  	{
            phost->Control.setup.b.bmRequestType = 0x40;
            phost->Control.setup.b.bRequest = 0x9a;
            phost->Control.setup.b.wValue.w = 0x1312;
            phost->Control.setup.b.wIndex.w = 0xe983;
            phost->Control.setup.b.wLength.w = 0;
  	}
 	 else //115200
  	{
            phost->Control.setup.b.bmRequestType = 0x40;
            phost->Control.setup.b.bRequest = 0x9a;
            phost->Control.setup.b.wValue.w = 0x1312;
            phost->Control.setup.b.wIndex.w = 0xcc83;
            phost->Control.setup.b.wLength.w = 0;
  	}
	 
  ret=USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8 );  
  
  }
 else if(strncmp(driver_string,"CP2102 USB",strlen("CP2102 USB"))==0)  ////for  CP2102 
  {
  //41 1e 00 00  00 00 04 00 带参数:90 d0 03 00=250000; 00 e8 03 00=256000; 00 c2 01 00=115200
	 if(print_info.baud_rate==250000)//CP2102这个配置是必须的
  		CDC_SetLineCode.b.dwDTERate =250000;
 	 else if(print_info.baud_rate==256000)
  		CDC_SetLineCode.b.dwDTERate =256000;
 	 else //115200
  		CDC_SetLineCode.b.dwDTERate =115200;
	 
  		CDC_SetLineCode.b.bDataBits =8;
  		CDC_SetLineCode.b.bParityType =0 ;  
  		CDC_SetLineCode.b.bCharFormat =0; 
	
		if(i==0)
  		{
			ret=CDC_SETCP2102_1(pdev, phost);
			if(ret==USBH_OK)
				i=1;
			return USBH_BUSY;
		}
  		if(i==1)
  		{
			ret=CDC_SETCP2102_2(pdev, phost);
			if(ret==USBH_OK)
				i=2;
			return USBH_BUSY;
  		}
  		if(i==2)
  		{
			ret=CDC_SETCP2102_3(pdev, phost);
			if(ret==USBH_OK)
				i=3;
			return USBH_BUSY;
  		}
  		if(i==3)
  		{
			ret=CDC_SETCP2102_4(pdev, phost);
			if(ret==USBH_OK)
				i=4;
			return USBH_BUSY;
  		}
  		if(i==4)
  		{
			ret=CDC_SETCP2102_5(pdev, phost);
			if(ret==USBH_OK)
				i=5;
			return USBH_BUSY;
  		}
  		if(i==5)
  		{
			ret=CDC_SETCP2102_6(pdev, phost);//是这个函数触发了设置
			if(ret==USBH_OK)
				i=6;
			return USBH_BUSY;
  		}
  		if(i==6)
  		{
			ret=CDC_SETCP2102_7(pdev, phost);
			if(ret==USBH_OK)
				i=7;
			return USBH_BUSY;
  		}
  		if(i==7)
  		{
			ret=CDC_SETCP2102_8(pdev, phost);
			if(ret==USBH_OK)
				i=8;
			return USBH_BUSY;
  		}
  		if(i==8)
  		{
			ret=CDC_SETCP2102_9(pdev, phost);
			if(ret==USBH_OK)
				i=9;
			return USBH_BUSY;
  		}
  		if(i==9)
  		{
			ret=CDC_SETCP2102_10(pdev, phost);
			if(ret==USBH_OK)
				i=10;
			return USBH_BUSY;
  		}
  		if(i==10)
  		{
			ret=CDC_SETCP2102_11(pdev, phost);
			if(ret==USBH_OK)
				i=0;
  		}
		else
			i=0;
  }
  ////for  FT232
  else if(strncmp(driver_string,"FT232",strlen("FT232"))==0)
  {
    int baud=0;
	if(print_info.baud_rate==115200)//0x1A
		baud=SetBaudRate(115200); 
	else if(print_info.baud_rate==250000)
		  baud=0x000c; //baud=0x800b; //未验证，随意设的
	else
      baud=0x800b; //256000:0x800b 
	 // baud=0xc409;//115200
    phost->Control.setup.b.bmRequestType = 0x40;
    phost->Control.setup.b.bRequest = 0x03;
    phost->Control.setup.b.wValue.w = baud;
    phost->Control.setup.b.wIndex.w = 0x01;
    phost->Control.setup.b.wLength.w = 0;
	
    ret=USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8 );  
	
  }
    ////for  PL2303
  else //if(strncmp(driver_string,"USB-Serial Controller",strlen("USB-Serial Controller"))==0)
  {
  	if(i==0)
  	{
		ret=CDC_PL2303_clear(pdev, phost);
		if(ret==USBH_OK)
			i=1;
		return USBH_BUSY;
	}
  	if(i==1)
  	{
		ret=CDC_PL2303_SetLine(pdev, phost);
		if(ret==USBH_OK)
		{
			if((strncmp(driver_string,"Arduino Mega 2560",strlen("Arduino Mega 2560"))==0)
                           ||(strncmp(driver_string,"RUMBA",strlen("RUMBA"))==0))
				i=7;//Iduino MEGA R3的情况，直接跳过这里，因为会卡住
			else
				i=2;
			
		}
		return USBH_BUSY;

	}
	else if(i==2)
	{
		ret=CDC_PL2303_0(pdev, phost);
		if(ret==USBH_OK)
			i=3;
		return USBH_BUSY;
	}
	else if(i==3)
	{
  	phost->Control.setup.b.bmRequestType = 0x40;
  	phost->Control.setup.b.bRequest = 0x01;
  	phost->Control.setup.b.wValue.w = 0x0b0b;
  	phost->Control.setup.b.wIndex.w = 0x02;
  	phost->Control.setup.b.wLength.w = 0;    
	ret=USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8);
		if(ret==USBH_OK)
			i=4;
		return USBH_BUSY;
	}
	else if(i==4)
	{
  	phost->Control.setup.b.bmRequestType = 0xc0;
  	phost->Control.setup.b.bRequest = 0x01;
  	phost->Control.setup.b.wValue.w = 0x80;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0x02;    
	ret=USBH_CtlReq(pdev, phost, CDC_GetLineCode.Array , 8);
		if(ret==USBH_OK)
			i=5;
		return USBH_BUSY;
	}
	else if(i==5)
	{
  	phost->Control.setup.b.bmRequestType = 0x40;
  	phost->Control.setup.b.bRequest = 0x01;
  	phost->Control.setup.b.wValue.w = 0x0404;
  	phost->Control.setup.b.wIndex.w = 0x00;
  	phost->Control.setup.b.wLength.w = 0;    
	ret=USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 8);
		if(ret==USBH_OK)
			i=6;
		return USBH_BUSY;
	}
	else if(i==6)
	{
  	phost->Control.setup.b.bmRequestType = 0x40;
  	phost->Control.setup.b.bRequest = 0x01;
  	phost->Control.setup.b.wValue.w = 0x02;
  	phost->Control.setup.b.wIndex.w = 0x44;
  	phost->Control.setup.b.wLength.w = 0X02;    
	ret=USBH_CtlReq(pdev, phost, CDC_SetLineCode.Array , 2);
		if(ret==USBH_OK)
			i=7;
		return USBH_BUSY;
	}
	else if(i==7)
	{
		ret=CDC_PL2303_SetControl(pdev, phost);
		if(ret==USBH_OK)
			i=8;
		return USBH_BUSY;
	}
	else if(i==8)
	{
	  	phost->Control.setup.b.bmRequestType = 0x21;
  		phost->Control.setup.b.bRequest = 0x22;
  		phost->Control.setup.b.wValue.w = 0x03;
  		phost->Control.setup.b.wIndex.w = 0x00;
  		phost->Control.setup.b.wLength.w = 0x00;
	
		ret=USBH_CtlReq(pdev, phost, 0, 0);
		if(ret==USBH_OK)
			i=0;
	}
	else i=0;
	#endif
   }
  return ret;
}

/**
  * @brief  This request generates RS-232/V.24 style control signals.
  * @param  pdev: Selected device
  * @retval USBH_Status : USB ctl xfer status
  */
USBH_Status CDC_SETControlLineState(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{  
  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | \
                              USB_REQ_RECIPIENT_INTERFACE;
  
  phost->Control.setup.b.bRequest = CDC_SET_CONTROL_LINE_STATE;
  /*Control Signal Bitmap Values for SetControlLineState*/
  phost->Control.setup.b.wValue.w = CDC_DEACTIVATE_CARRIER_SIGNAL_RTS | \
                          CDC_DEACTIVATE_SIGNAL_DTR;

  phost->Control.setup.b.wIndex.w = 0;

  /*Length feild is zero*/
  phost->Control.setup.b.wLength.w = 0;           
  
  return USBH_CtlReq(pdev, phost, 0 , 0 );  
}

/**
  * @brief  This function prepares the state before issuing the class specific commands
  * @param  None
  * @retval None
  */
void CDC_ChangeStateToIssueSetConfig(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{
  phost->gStateBkp = phost->gState  ;
  phost->gState = HOST_CLASS_REQUEST;
  CDC_ReqState = CDC_SET_LINE_CODING_RQUEST;
}

/**
  * @brief  This function prepares the state before issuing the class specific commands
  * @param  None
  * @retval None
  */
void CDC_IssueGetConfig(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost)
{
  phost->gStateBkp =  phost->gState ;
  phost->gState = HOST_CLASS_REQUEST;
  CDC_ReqState = CDC_GET_LINE_CODING_RQUEST;
}

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/


/**
* @}
*/


/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
