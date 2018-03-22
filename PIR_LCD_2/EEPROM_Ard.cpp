#include "EEPROM_Ard.h"
#include <Arduino.h>
#include <EEPROM.h>

/*
es. value = 6793
numReg = 27
resto = 163

*/


int WriteMemory(int address, int value)
{
  int FlagValueBig = 0;
  int numReg;
  int resto = value % MAX_CELL_EEPROM;

  if(address < MAX_EEPROM_DIM)
  {
    if((value / MAX_CELL_EEPROM) == 0)
    {
      numReg = 1;
      FlagValueBig = 0;
      EEPROM.update(FLAG_VALUE_ADDR, FlagValueBig);
    }
    else if((value / MAX_CELL_EEPROM) == 1)
    {
      if(value > MAX_CELL_EEPROM)
      {
        numReg = (value / MAX_CELL_EEPROM) + 1;
        FlagValueBig = 1;
        EEPROM.update(FLAG_VALUE_ADDR, FlagValueBig);
      }
      else
      {
        numReg = 1;
        FlagValueBig = 0;
        EEPROM.update(FLAG_VALUE_ADDR, FlagValueBig);
      }
    }
    else
    {
      numReg = (value / MAX_CELL_EEPROM) + 1;
      FlagValueBig = 1;
      EEPROM.update(FLAG_VALUE_ADDR, FlagValueBig);
      if(address + (numReg - 1) > MAX_EEPROM_DIM)
      {
        numReg = 0;
      }
    }
    
    for(int idx = 0; idx < numReg; idx++)
    {
      if(numReg == 0)
      {
        break;
      }
      else
      {
        if(numReg > 1)
        {
          if(idx < numReg - 1)
          {
            EEPROM.write(address + idx, MAX_CELL_EEPROM);
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
   EEPROM.update(NUM_REG_ADDR, numReg);
 }
  
  return  numReg;
}

bool ReadMemory(int address, int numReg, int *value)
{
  int ValueRead;
  int FlagValueBig = 0;
  bool ReadOk = false;
  if(address == START_DELAY_ADDR)
  {
	  FlagValueBig = EEPROM.read(FLAG_VALUE_ADDR);
	  
	  if(!FlagValueBig)
	  {
		 ValueRead = EEPROM.read(address);
		 ReadOk = true;      
	  }
	  else
	  {
		for(int idx = 0; idx < numReg; idx++)
		{
		  ValueRead += EEPROM.read(address + idx);      
		}
		ReadOk = true;
	  }	  
  }
  else
  {
	ValueRead = EEPROM.read(address);
	ReadOk = true;   	  
  }

  if(ReadOk)
  {
    *value = ValueRead;   
  }
  return ReadOk;
}

bool ClearMemory()
{
	// Tempo di cancellazione 3.3 s
	for(int i = 0; i < MAX_EEPROM_DIM; i++)
	{
		EEPROM.update(i, 0);
	}	
}