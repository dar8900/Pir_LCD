#ifndef EEPROM_ARD_H
#define EEPROM_ARD_H

#include <Arduino.h>

#define MAX_EEPROM_DIM	      EEPROM.length()
#define MAX_CELL_EEPROM	                  254

#define START_DELAY_ADDR                    0
#define SWITCH_PIR_ADDR                     5	
#define HOUR_BAND_1_ADDR  	               10
#define HOUR_BAND_2_ADDR  	               11
#define MINUTE_BAND_1_ADDR	               12
#define MINUTE_BAND_2_ADDR	               13
#define DAY_BAND_1_ADDR                    14
#define DAY_BAND_2_ADDR                    15
#define MONTH_BAND_1_ADDR                  16
#define MONTH_BAND_2_ADDR                  17 
                                           
#define FLAG_VALUE_ADDR                    50
#define NUM_REG_ADDR                       51


int WriteMemory(short address, short value);
bool ReadMemory(short address, byte numReg, short *value);

#endif
