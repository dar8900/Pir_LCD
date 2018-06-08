#ifndef EEPROM_ARD_H
#define EEPROM_ARD_H

#include <Arduino.h>

#undef  NEW_EEPROM

#ifndef NEW_EEPROM
#define MAX_EEPROM_DIM	      EEPROM.length()
#else
#define MAX_EEPROM_DIM				    32766
#endif

#define MAX_CELL_EEPROM	                  254

#define START_DELAY_ADDR                    0
#define SWITCH_PIR_ADDR                     5
#define MANUAL_STATE_ADDR	                6

#define BAND_INITHOUR	                   10
#define BAND_INITMINUTE	                   11
#define BAND_INITDAY	                   12
#define BAND_ENDHOUR	                   13
#define BAND_ENDMINUTE                     14
#define BAND_ENDDAY                        15

#define FLAG_VALUE_ADDR                    50
#define NUM_REG_ADDR                       51

#define FIRST_START_CHECK_ADDR			  100

int WriteMemory(short address, short value);
bool ReadMemory(short address, short numReg, short *value);
bool ClearMemory();
bool IsMemoryEmpty();

#ifdef NEW_EEPROM
void EepromUpdate(short address, short value)
#define 	EEPROM.update(address, value)  EepromUpdate(address, value)
#endif

#endif
