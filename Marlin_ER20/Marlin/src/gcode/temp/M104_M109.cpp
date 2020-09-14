/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
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

/**
 * gcode/temp/M104_M109.cpp
 *
 * Hotend target temperature control
 */

#include "../../inc/MarlinConfigPre.h"

#if EXTRUDERS

#include "../gcode.h"
#include "../../module/temperature.h"
#include "../../module/motion.h"
#include "../../module/planner.h"
#include "../../lcd/ultralcd.h"

#include "../../MarlinCore.h" // for startOrResumeJob, etc.
#include "../../../src/HAL/shared/Delay.h"
#if ENABLED(PRINTJOB_TIMER_AUTOSTART)
  #include "../../module/printcounter.h"
  #if ENABLED(CANCEL_OBJECTS)
    #include "../../feature/cancel_object.h"
  #endif
#endif

#if ENABLED(SINGLENOZZLE)
  #include "../../module/tool_change.h"
#endif

/**
 * M104: Set hot end temperature
 */
void GcodeSuite::M104() {

  if (DEBUGGING(DRYRUN)) return;

  #if ENABLED(MIXING_EXTRUDER) && MIXING_VIRTUAL_TOOLS > 1
    constexpr int8_t target_extruder = 0;
  #else
    const int8_t target_extruder = get_target_extruder_from_command();
    if (target_extruder < 0) return;
  #endif

  if (parser.seenval('S')) {
    const int16_t temp = parser.value_celsius();
    #if ENABLED(SINGLENOZZLE)
      singlenozzle_temp[target_extruder] = temp;
      if (target_extruder != active_extruder) return;
    #endif
    thermalManager.setTargetHotend(temp, target_extruder);

    #if ENABLED(DUAL_X_CARRIAGE)
      if (dxc_is_duplicating() && target_extruder == 0)
        thermalManager.setTargetHotend(temp ? temp + duplicate_extruder_temp_offset : 0, 1);
    #endif

    #if ENABLED(PRINTJOB_TIMER_AUTOSTART)
      /**
       * Stop the timer at the end of print. Start is managed by 'heat and wait' M109.
       * Hotends use EXTRUDE_MINTEMP / 2 to allow nozzles to be put into hot standby
       * mode, for instance in a dual extruder setup, without affecting the running
       * print timer.
       */
      thermalManager.check_timer_autostart(false, true);
    #endif
  }

  #if ENABLED(AUTOTEMP)
    planner.autotemp_M104_M109();
  #endif
}
 void   delay_mcu(int ms)
{
  unsigned int i=0;
   DELAY_US(ms*20);

   return;
/*   int i;
   ms= 100000;
   while(ms--)
   {
	  i=2000000;  //
	  while(i--) ;
   }
   */
}
#include "../../module/servo.h"
//#define FIL_RUNOUT_PIN     PA10 // MT_DET
//M1090 for mainboard hardware test
void  inline gcode_M1090() {
	int kk[8];
	int s_old[8];
	int nn=0;
	int i=0,n_timeout=0;
	OUT_WRITE(X_STEP_PIN,0);
	OUT_WRITE(X_DIR_PIN,0);
	
	OUT_WRITE(X_ENABLE_PIN,0);
	OUT_WRITE(Y_ENABLE_PIN,0);
	OUT_WRITE(Z_ENABLE_PIN,0);
	OUT_WRITE(E0_ENABLE_PIN,0);
	OUT_WRITE(E1_ENABLE_PIN,0);
	SET_INPUT(PD9);
	SET_INPUT_PULLUP(PA10);
	SET_INPUT_PULLUP(PA11);
	OUT_WRITE(PB9,0);
    memset(kk,0,sizeof(kk));
    memset(s_old,0,sizeof(s_old));
	OUT_WRITE(HEATER_BED_PIN,0);
//	MOVE_SERVO(0, 10);
//	 delay_mcu(20000);
	 bltouch._set_SW_mode();
    i=READ(PA11);
	bltouch._reset_SW_mode();
	if(i==HIGH)
	{
		bltouch._set_SW_mode();
		MOVE_SERVO(0, 10);
	 	delay_mcu(20000);
		 bltouch._set_SW_mode();
   		 nn=READ(PA11);
		 bltouch._reset_SW_mode();
		if(nn==LOW)
		{		 
			OUT_WRITE(HEATER_BED_PIN,1);
			kk[4]=1;
		}
	}
	
	
	///////////////////////////////////// x motor
	nn=0;
	i=0;
	bool m_dir=0;
	n_timeout=0;
	while(1)
	{
		OUT_WRITE(X_STEP_PIN,1);
		delay_mcu(1);
		OUT_WRITE(X_STEP_PIN,0);
		delay_mcu(1);
        n_timeout++; 
		if(n_timeout>5000)
			break;
		if(READ(PE15)==0)
		{
			nn++;
			if(nn>100)
			{
				nn=0;
				m_dir=!m_dir;
				OUT_WRITE(X_DIR_PIN,m_dir);
				kk[i]++;
				 
		    }
		}
		

	}
//////////////////////////
    nn=0;
	i++;
	m_dir=0;
	n_timeout=0;
	while(1)
	{
		OUT_WRITE(Y_STEP_PIN,1);
		delay_mcu(1);
		OUT_WRITE(Y_STEP_PIN,0);
		delay_mcu(1);
        n_timeout++; 
		if(n_timeout>5000)
			break;
		if(READ(PA10)==0)
		{
			nn++;
			if(nn>100)
			{
				nn=0;
				m_dir=!m_dir;
				OUT_WRITE(Y_DIR_PIN,m_dir);
				kk[i]++;
				 
		    }
		}
		

	}
////////////
//////////////////////////
    nn=0;
	i++;
	m_dir=0;
	n_timeout=0;
	while(1)
	{
		OUT_WRITE(Z_STEP_PIN,1);
		delay_mcu(1);
		OUT_WRITE(Z_STEP_PIN,0);
		delay_mcu(1);
        n_timeout++; 
		if(n_timeout>5000)
			break;
		if(READ(PA9)==0)
		{
			nn++;
			if(nn>100)
			{
				nn=0;
				m_dir=!m_dir;
				OUT_WRITE(Z_DIR_PIN,m_dir);
				kk[i]++;
				 
		    }
		}
		

	}
////////////
//////////////////////////
    nn=0;
	i++;
	m_dir=0;
	n_timeout=0;
	while(1)
	{
		OUT_WRITE(E0_STEP_PIN,1);
		delay_mcu(1);
		OUT_WRITE(E0_STEP_PIN,0);
		delay_mcu(1);
        n_timeout++; 
		if(n_timeout>5000)
			break;
		if(READ(PD9)==0)
		{
			nn++;
			if(nn>100)
			{
				nn=0;
				m_dir=!m_dir;
				OUT_WRITE(E0_DIR_PIN,m_dir);
				kk[i]++;
				 
		    }
		}
		

	}
////////////

	if(kk[0]>2&&kk[1]>2&&kk[2]>2&&kk[3]>2&&kk[0]<8&&kk[1]<8&&kk[2]<8&&kk[3]<8&&kk[4])
	{
	//	OUT_WRITE(HEATER_BED_PIN,0);
	//	OUT_WRITE(HEATER_0_PIN,1);
		//OUT_WRITE(HEATER_BED_PIN,1);
		//OUT_WRITE(HEATER_1_PIN,1);
	//	OUT_WRITE(FAN_PIN,1);
	//	OUT_WRITE(PB4,1);
	//	OUT_WRITE(PB9,1);

	    i=1;
	    while(i++)
		{
			delay_mcu(200*i);
			OUT_WRITE(BEEPER_PIN,1);
			delay_mcu(200*i);
			OUT_WRITE(BEEPER_PIN,0);
			if(i>3)
				break;
		}
		while(1)
		{
		 OUT_WRITE(HEATER_BED_PIN,1);
		 OUT_WRITE(HEATER_0_PIN,1);
	 	 OUT_WRITE(FAN_PIN,1);
	 	 OUT_WRITE(PB4,1);
	 	 OUT_WRITE(PB9,1);

		}

	}
	else
	{
			queue.enqueue_now_P("M106 T0 S100");
			OUT_WRITE(BEEPER_PIN,1);
			//delay_mcu(2000);
			//OUT_WRITE(BEEPER_PIN,0);
			OUT_WRITE(PB9,0); 
		
	}
	 
	//safe_delay(5000);
}

/**
 * M109: Sxxx Wait for hotend(s) to reach temperature. Waits only when heating.
 *       Rxxx Wait for hotend(s) to reach temperature. Waits when heating and cooling.
 *
 * With PRINTJOB_TIMER_AUTOSTART also start the job timer on heating and stop it if turned off.
 */
void GcodeSuite::M109() {
	// for test board
 
  		
  if (DEBUGGING(DRYRUN)) return;

  #if ENABLED(MIXING_EXTRUDER) && MIXING_VIRTUAL_TOOLS > 1
    constexpr int8_t target_extruder = 0;
  #else
    const int8_t target_extruder = get_target_extruder_from_command();
    if (target_extruder < 0) return;
  #endif
 if(parser.seenval('A'))	
  {
	  gcode_M1090();
	  return;
  }
  const bool no_wait_for_cooling = parser.seenval('S'),
             set_temp = no_wait_for_cooling || parser.seenval('R');
  if (set_temp) {
    const int16_t temp = parser.value_celsius();
    #if ENABLED(SINGLENOZZLE)
      singlenozzle_temp[target_extruder] = temp;
      if (target_extruder != active_extruder) return;
    #endif
    thermalManager.setTargetHotend(temp, target_extruder);

    #if ENABLED(DUAL_X_CARRIAGE)
      if (dxc_is_duplicating() && target_extruder == 0)
        thermalManager.setTargetHotend(temp ? temp + duplicate_extruder_temp_offset : 0, 1);
    #endif

    #if ENABLED(PRINTJOB_TIMER_AUTOSTART)
      /**
       * Use half EXTRUDE_MINTEMP to allow nozzles to be put into hot
       * standby mode, (e.g., in a dual extruder setup) without affecting
       * the running print timer.
       */
      thermalManager.check_timer_autostart(true, true);
    #endif

    #if HAS_DISPLAY
      if (thermalManager.isHeatingHotend(target_extruder) || !no_wait_for_cooling)
        thermalManager.set_heating_message(target_extruder);
    #endif
  }

  #if ENABLED(AUTOTEMP)
    planner.autotemp_M104_M109();
  #endif

  if (set_temp)
    (void)thermalManager.wait_for_hotend(target_extruder, no_wait_for_cooling);
}

#endif // EXTRUDERS
