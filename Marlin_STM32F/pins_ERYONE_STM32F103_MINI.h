/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */



//#if !defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)
//  #error "Oops!  Make sure you have 'Arduino Mega' selected from the 'Tools -> Boards' menu."
//#endif

//#ifndef BOARD_NAME
  #define BOARD_NAME "ERYONE,STM32F103MINI"
//#endif
//
// Servos
//

#define SERVO0_PIN       0xB9// 0xBE
//
// Limit Switches
//
#define X_MIN_PIN           0xD7//0xE5
#ifndef X_MAX_PIN
  #define X_MAX_PIN        // 0xBE//0xE4
#endif
#define Y_MIN_PIN          0xB7//0xE3
#define Y_MAX_PIN          //0xBD//0xE2

#define Z_MAX_PIN          // 0xB8//0xE0

#if ENABLED(BLTOUCH)
#define Z_MIN_PIN           0xE4

#else
#define Z_MIN_PIN            0xB8//0xE1

#endif

//
// Z Probe (when not Z_MIN_PIN)
//
#ifndef Z_MIN_PROBE_PIN
  #define Z_MIN_PROBE_PIN  32
#endif
//
// Steppers
//
#define X_STEP_PIN         0xBF//0xE3//0xC6
#define X_DIR_PIN          0xBE//0xDD
#define X_ENABLE_PIN       0xDB//0xA8//38
#define X_CS_PIN           53

#define Y_STEP_PIN        0xC6// 0xAC
#define Y_DIR_PIN         0xDF// 0xAB
#define Y_ENABLE_PIN      0xC7//0XAF
#define Y_CS_PIN           49

#define Z_STEP_PIN         0xAB// 0xD6
#define Z_DIR_PIN          0xAA//0xD3
#define Z_ENABLE_PIN       0xAF//0xA3
#define Z_CS_PIN           40


#define E0_STEP_PIN        0xDD//0xB2
#define E0_DIR_PIN         0xDC//0xBB
#define E0_ENABLE_PIN      0xDE//0xC4
#ifndef E0_CS_PIN
#define E0_CS_PIN          42
#endif

#define E1_STEP_PIN        0xA8
#define E1_DIR_PIN         0xC8
#define E1_ENABLE_PIN      0xA9
#ifndef E1_CS_PIN
#define E1_CS_PIN          44
#endif

/**
 * Default pins for TMC software SPI
 */
#if ENABLED(TMC_USE_SW_SPI)
  #ifndef TMC_SW_MOSI
    #define TMC_SW_MOSI    66
  #endif
  #ifndef TMC_SW_MISO
    #define TMC_SW_MISO    44
  #endif
  #ifndef TMC_SW_SCK
    #define TMC_SW_SCK     64
  #endif
#endif

#if HAS_DRIVER(TMC2208)
  /**
   * TMC2208 stepper drivers
   *
   * Hardware serial communication ports.
   * If undefined software serial is used according to the pins below
   */
  //#define X_HARDWARE_SERIAL  Serial1
  //#define X2_HARDWARE_SERIAL Serial1
  //#define Y_HARDWARE_SERIAL  Serial1
  //#define Y2_HARDWARE_SERIAL Serial1
  //#define Z_HARDWARE_SERIAL  Serial1
  //#define Z2_HARDWARE_SERIAL Serial1
  //#define E0_HARDWARE_SERIAL Serial1
  //#define E1_HARDWARE_SERIAL Serial1
  //#define E2_HARDWARE_SERIAL Serial1
  //#define E3_HARDWARE_SERIAL Serial1
  //#define E4_HARDWARE_SERIAL Serial1

  /**
   * Software serial
   */

  #define X_SERIAL_TX_PIN    40
  #define X_SERIAL_RX_PIN    63
  #define X2_SERIAL_TX_PIN   -1
  #define X2_SERIAL_RX_PIN   -1

  #define Y_SERIAL_TX_PIN    59
  #define Y_SERIAL_RX_PIN    64
  #define Y2_SERIAL_TX_PIN   -1
  #define Y2_SERIAL_RX_PIN   -1

  #define Z_SERIAL_TX_PIN    42
  #define Z_SERIAL_RX_PIN    65
  #define Z2_SERIAL_TX_PIN   -1
  #define Z2_SERIAL_RX_PIN   -1

  #define E0_SERIAL_TX_PIN   44
  #define E0_SERIAL_RX_PIN   66
  #define E1_SERIAL_TX_PIN   -1
  #define E1_SERIAL_RX_PIN   -1
  #define E2_SERIAL_TX_PIN   -1
  #define E2_SERIAL_RX_PIN   -1
  #define E3_SERIAL_TX_PIN   -1
  #define E3_SERIAL_RX_PIN   -1
  #define E4_SERIAL_TX_PIN   -1
  #define E4_SERIAL_RX_PIN   -1
#endif

//
// Temperature Sensors
//
#define TEMP_0_PIN        0xC2//0xB1// 13   // Analog Input
#define TEMP_1_PIN        0xC1//0xB0   // Analog Input
//#define TEMP_2_PIN        0xC0//0xB0   // Analog Input
#define TEMP_BED_PIN      0xC3//0xC5   // Analog Input
#define TEMP_BOARD_PIN        0xC0//0xB0   // Analog Input

// SPI for Max6675 or Max31855 Thermocouple
#if DISABLED(SDSUPPORT)
  #define MAX6675_SS       66   // Do not use pin 53 if there is even the remote possibility of using Display/SD card
#else
  #define MAX6675_SS       66   // Do not use pin 49 as this is tied to the switch inside the SD card socket to detect if there is an SD card present
#endif

//
// Augmentation for auto-assigning RAMPS plugs
//
#if DISABLED(IS_RAMPS_EEB) && DISABLED(IS_RAMPS_EEF) && DISABLED(IS_RAMPS_EFB) && DISABLED(IS_RAMPS_EFF) && DISABLED(IS_RAMPS_SF) && !PIN_EXISTS(MOSFET_D)
  #if HOTENDS > 1
    #if TEMP_SENSOR_BED
      #define IS_RAMPS_EEB
    #else
      #define IS_RAMPS_EEF
    #endif
  #elif TEMP_SENSOR_BED
    #define IS_RAMPS_EFB
  #else
    #define IS_RAMPS_EFF
  #endif
#endif

#define HEATER_0_PIN     0xD3

#if EXTRUDERS > 1
#define HEATER_1_PIN     0xD1
#endif
#define FAN_PIN         0xA2
#define FAN1_PIN        0xA3
#define FAN2_PIN        0xA1
#define FAN3_PIN        0xA0
#define FAN4_PIN        0xDA

#define HEATER_BED_PIN 0xD0
#define FAN_SOFT_PWM
#if ENABLED(IS_RAMPS_EFB)                      // Hotend, Fan, Bed
#elif ENABLED(IS_RAMPS_EEF)                    // Hotend, Hotend, Fan

  #define HEATER_1_PIN   RAMPS_D9_PIN
  #define FAN_PIN        RAMPS_D8_PIN
#elif ENABLED(IS_RAMPS_EEB)                    // Hotend, Hotend, Bed
  //#define HEATER_1_PIN     RAMPS_D9_PIN
  //#define HEATER_BED_PIN   RAMPS_D8_PIN
#elif ENABLED(IS_RAMPS_EFF)                    // Hotend, Fan, Fan
  #define FAN1_PIN         RAMPS_D8_PIN
#elif DISABLED(IS_RAMPS_SF)                    // Not Spindle, Fan (i.e., "EFBF" or "EFBE")
  #define HEATER_BED_PIN   RAMPS_D8_PIN
  #if HOTENDS == 1
    #define FAN1_PIN       MOSFET_D_PIN
  #else
    #define HEATER_1_PIN   MOSFET_D_PIN
  #endif
#endif


//////////////////////////
// LCDs and Controllers //
//////////////////////////

#if ENABLED(ULTRA_LCD)
  //
  // LCD Display output pins
  //
  	#ifdef REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER
        #define LCD_PINS_RS     0xEE//0xC6// 0xDA//0xE6 //T_PEN
        #define LCD_PINS_ENABLE  0xEF//0xBA//0xEF //D11
        #define LCD_PINS_D4       0xBB //0xC8 //D19
    #else
		#define LCD_PINS_RS     0xEE //T_PEN
        #define LCD_PINS_ENABLE 0xBA //D11
        #define LCD_PINS_D4     0xBB //D13
        #define LCD_PINS_D5     0xEC//0xED //D14
        #define LCD_PINS_D6     0xBC //D15
		#define LCD_PINS_D7     0xEC//D12

	#endif		
       //
		 ///////////////////
   #define SD_DETECT_PIN   0xA4 // 0xC7//49
   #define SD_SPI_SCK	0xB3//0xA5//
   #define SD_SPI_MISO	 0xB4// 0xA6
   #define SD_SPI_MOSI	 0xB5//0xA7
   #define SD_SPI_CS   0xB6//0xA4
   #define SD_SPI_REMAP  1 
   #define SD_SPI_NUM	1
		/////////////////////////////  
   #define SDSS               SD_SPI_CS//53	 
/////////
    #if DISABLED(NEWPANEL)
        #define BEEPER_PIN      0xEC		 
    #endif
  // LCD Display input pins
  //
  #if ENABLED(NEWPANEL)
   #define BEEPER_PIN		  0xBD//0xEC
   #define BTN_ENC 		0xBC//0xEF//0xED
 
   #define BTN_EN1 		0xA5//0xE9
   #define BTN_EN2 		0xA6//0xE8

  #endif // NEWPANEL

#endif // ULTRA_LCD
