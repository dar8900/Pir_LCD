#include "EEPROM_Ard.h"
#include <Arduino.h>

#include "RTC_EEPROMLib.h"


/*
es. value = 6793
numReg = 27
resto = 163

*/

static RTCLib eep_func;

int WriteMemory(short address, short value)
{
  short FlagValueBig = 0;
  short numReg;
  short resto = value % MAX_CELL_EEPROM;

  if(address < MAX_EEPROM_DIM)
  {
    if((value / MAX_CELL_EEPROM) == 0)
    {
      numReg = 1;
      FlagValueBig = 0;
      EepromUpdate(FLAG_VALUE_ADDR, FlagValueBig);
    }
    else if((value / MAX_CELL_EEPROM) == 1)
    {
      if(value > MAX_CELL_EEPROM)
      {
        numReg = (value / MAX_CELL_EEPROM) + 1;
        FlagValueBig = 1;
        EepromUpdate(FLAG_VALUE_ADDR, FlagValueBig);
      }
      else
      {
        numReg = 1;
        FlagValueBig = 0;
        EepromUpdate(FLAG_VALUE_ADDR, FlagValueBig);
      }
    }
    else
    {
      numReg = (value / MAX_CELL_EEPROM) + 1;
      FlagValueBig = 1;
      EepromUpdate(FLAG_VALUE_ADDR, FlagValueBig);
      if(address + (numReg - 1) > MAX_EEPROM_DIM)
      {
        numReg = 0;
      }
    }
    
    for(short idx = 0; idx < numReg; idx++)
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
            eep_func.eeprom_write(address + idx, MAX_CELL_EEPROM);
          }
          else
          {
            eep_func.eeprom_write(address + idx, resto);
          }
        }
        else
        {
          eep_func.eeprom_write(address, value);     
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
   EepromUpdate(NUM_REG_ADDR, numReg);
 }
  
  return  numReg;
}

bool ReadMemory(short address, short numReg, short *value)
{
  short ValueRead = 0;
  short FlagValueBig = 0;
  bool ReadOk = false;
  if(address == START_DELAY_ADDR)
  {
	  FlagValueBig = eep_func.eeprom_read(FLAG_VALUE_ADDR);
	  
	  if(!FlagValueBig)
	  {
		 ValueRead = eep_func.eeprom_read(address);
		 ReadOk = true;      
	  }
	  else
	  {
		for(short idx = 0; idx < numReg; idx++)
		{
		  ValueRead += eep_func.eeprom_read(address + idx);      
		}
		ReadOk = true;
	  }	  
  }
  else
  {
	ValueRead = eep_func.eeprom_read(address);
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
	// Tempo di cancellazione massimo 3.3 s
	for(short i = 0; i < MAX_EEPROM_DIM; i++)
	{
		EepromUpdate(i, 255);
	}
	return true;
}

bool IsMemoryEmpty()
{
	bool Empty = true;
	short Value;
	for(short i = 0; i < MAX_EEPROM_DIM; i++)
	{
		Value = eep_func.eeprom_read(i);
		if(Value != 255)
			Empty = false;		
		
	}
	return Empty;
}

void EepromUpdate(short address, short value)
{
	if(eep_func.eeprom_read(address) != value)
	{
		eep_func.eeprom_write(address, value);
	}
}