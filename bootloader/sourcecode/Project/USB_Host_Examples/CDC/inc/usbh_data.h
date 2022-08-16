/**
  ******************************************************************************
  * @file    usbh_data.h
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   This file is the header file for usb data file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBH_DATA__
#define __USBH_DATA__

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
#include "usb_conf.h"
#include <stdio.h>

struct fsdata_file {
  const struct fsdata_file *next;
  const uint8_t *name;
  const uint8_t *data;
  const uint16_t len;
};

struct fsdata_file_noconst {
  struct fsdata_file *next;
  uint8_t *name;
  uint8_t *data;
  unsigned long len;
};



extern const uint8_t Stored_File1[368];
extern const uint8_t Stored_File2[5603];
extern const uint8_t Stored_File3[800] ;

static const struct fsdata_file file_text3[] = {NULL, "file3.txt",Stored_File3, sizeof(Stored_File3)};
static const struct fsdata_file file_text2[] = {file_text3, "file2.txt",Stored_File2, sizeof(Stored_File2)};
static const struct fsdata_file file_text1[] = {file_text2, "file1.txt",Stored_File1, sizeof(Stored_File1)};

#define FS_ROOT   file_text1

#endif /* __USBH_DATA__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
