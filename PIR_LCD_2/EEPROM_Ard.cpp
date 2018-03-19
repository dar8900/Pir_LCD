#include "EEPROM_Ard.h"
#include <Arduino.h>
#include <EEPROM.h>

/*
es. value = 6793
numReg = 27
resto = 163

*/

int FlagValueBig = 0;
int numReg;

int WriteMemory(int address, int value)
{
  int numReg;
  int resto = value % 255;

  if(address < 1025)
  {
    if((value / 255) == 0)
    {
      numReg = 1;
      FlagValueBig = 0;
      EEPROM.write(FLAG_VALUE_ADDR, FlagValueBig);
    }
    else if((value / 255) == 1)
    {
      if(value > 255)
      {
        numReg = (value / 255) + 1;
        FlagValueBig = 1;
        EEPROM.write(FLAG_VALUE_ADDR, FlagValueBig);
      }
      else
      {
        numReg = 1;
        FlagValueBig = 0;
        EEPROM.write(FLAG_VALUE_ADDR, FlagValueBig);
      }
    }
    else
    {
      numReg = (value / 255) + 1;
      FlagValueBig = 1;
      EEPROM.write(FLAG_VALUE_ADDR, FlagValueBig);
      if(address + (numReg - 1) > 1025)
      {
        numReg = 0;
      }
    }
    
    for(int idx = 0; idx < numReg; idx++)
    {
      if(address + (numReg - 1) > 1025)
      {
        break;
      }
      else
      {
        if(numReg > 1)
        {
          if(idx < numReg - 1)
          {
            EEPROM.write(address + idx, 255);
          }
          else
          {
            EEPROM.write(address + idx, resto);
          }
        }
        else
        {
          EEPROM.write(address, value);     
        }       
      }
    }   
  }
  else
  {
    numReg = 0;
  }

  if(address == START_DELAY_ADDR)
  {
    EEPROM.write(NUM_REG_ADDR, numReg);
  }
  
  return  numReg;
}

int ReadMemory(int address, int numReg, int *value)
{
  int ValueRead;
  byte ReadOk = 0;
  FlagValueBig = EEPROM.read(FLAG_VALUE_ADDR);
  
  if(!FlagValueBig)
  {
    ValueRead = EEPROM.read(address);
    ReadOk = 1;
  }
  else
  {
    for(int idx = 0; idx < numReg; idx++)
    {
      ValueRead += EEPROM.read(address + idx);      
    }
    ReadOk = 1;
  }
  if(ReadOk)
  {
    *value = ValueRead;   
  }
  return ReadOk;
}
