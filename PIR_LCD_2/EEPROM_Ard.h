#ifndef EEPROM_ARD_H
#define EEPROM_ARD_H

#define START_DELAY_ADDR  0
#define START_SWITCH_PIR_ADDR 5
#define FLAG_VALUE_ADDR 50
#define NUM_REG_ADDR    51

//extern int FlagValueBig;

int WriteMemory(int address, int value);
int ReadMemory(int address, int numReg, int *value);

#endif
