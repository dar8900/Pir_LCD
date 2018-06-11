#ifndef EEPROM_ARD_H
#define EEPROM_ARD_H

#include <Arduino.h>

#define MAX_EEPROM_DIM	      EEPROM.length()


#define MAX_CELL_EEPROM	                  254

#define START_DELAY_ADDR                    0
#define SWITCH_PIR_ADDR                     5
#define MANUAL_STATE_ADDR	                6
#define LIGHT_STATE_ADDR                    7

#define BAND_INIT_HOUR_ADDR	               10
#define BAND_INIT_MINUTE_ADDR	           11
#define BAND_INIT_DAY_ADDR	               12
#define BAND_END_HOUR_ADDR	               13
#define BAND_END_MINUTE_ADDR               14
#define BAND_END_DAY_ADDR                  15
#define BAND_INVALIDATION_VALUE_ADDR       16

#define FLAG_VALUE_ADDR                    50
#define NUM_REG_ADDR                       51

#define FIRST_START_CHECK_ADDR			  100

int WriteMemory(short address, short value);
bool ReadMemory(short address, short numReg, short *value);
bool ClearMemory();
bool IsMemoryEmpty();

void EepromUpdate(short address, short value);


#endif
