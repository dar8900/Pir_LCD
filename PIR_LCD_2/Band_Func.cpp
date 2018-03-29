#include "Band_Func.h"
#include "EEPROM_Ard.h"
#include "PIR_LCD_2.h"

#ifdef RTC_INSERTED
extern TIME_BAND Band_1;
extern TIME_BAND Band_2;
extern DATE_FORMAT PresentDate;
extern TIME_FORMAT PresentTime;
#endif

extern EEPROM_ITEM EepromTab[];
// extern bool FlagBandOk;
// extern bool FlagAllBandsInvalid;
extern FLAGS Flags;

#ifdef RTC_INSERTED
void SaveBandToEeprom()
{
	WriteMemory(EepromTab[HOUR_BAND_1].eeprom_par_addr		, Band_1.BandTime.hour);
	WriteMemory(EepromTab[HOUR_BAND_2].eeprom_par_addr		, Band_2.BandTime.hour);
	WriteMemory(EepromTab[MINUTE_BAND_1].eeprom_par_addr	, Band_1.BandTime.minute);
	WriteMemory(EepromTab[MINUTE_BAND_2].eeprom_par_addr 	, Band_2.BandTime.minute);
	WriteMemory(EepromTab[DAY_BAND_1].eeprom_par_addr		, Band_1.BandDate.day);
	WriteMemory(EepromTab[DAY_BAND_2].eeprom_par_addr		, Band_2.BandDate.day);
	WriteMemory(EepromTab[MONTH_BAND_1].eeprom_par_addr		, Band_1.BandDate.month);
	WriteMemory(EepromTab[MONTH_BAND_2].eeprom_par_addr		, Band_2.BandDate.month);	
	return;
}

void ReadBandFromEeprom()
{
    ReadMemory(EepromTab[HOUR_BAND_1].eeprom_par_addr	, EepromTab[HOUR_BAND_1].eeprom_par_numReg	, Band_1.BandTime.hour);
	ReadMemory(EepromTab[HOUR_BAND_2].eeprom_par_addr	, EepromTab[HOUR_BAND_2].eeprom_par_numReg	, Band_2.BandTime.hour);
	ReadMemory(EepromTab[MINUTE_BAND_1].eeprom_par_addr , EepromTab[MINUTE_BAND_1].eeprom_par_numReg, Band_1.BandTime.minute);
	ReadMemory(EepromTab[MINUTE_BAND_2].eeprom_par_addr , EepromTab[MINUTE_BAND_2].eeprom_par_numReg, Band_2.BandTime.minute);
	ReadMemory(EepromTab[DAY_BAND_1].eeprom_par_addr	, EepromTab[DAY_BAND_1].eeprom_par_numReg	, Band_1.BandDate.day);
	ReadMemory(EepromTab[DAY_BAND_2].eeprom_par_addr	, EepromTab[DAY_BAND_2].eeprom_par_numReg	, Band_2.BandDate.day);
	ReadMemory(EepromTab[MONTH_BAND_1].eeprom_par_addr	, EepromTab[MONTH_BAND_1].eeprom_par_numReg	, Band_1.BandDate.month);
	ReadMemory(EepromTab[MONTH_BAND_2].eeprom_par_addr	, EepromTab[MONTH_BAND_2].eeprom_par_numReg	, Band_2.BandDate.month);	
	return;
}

void ChekBandValue()
{
	bool InvalidBand = false;
	
	if(Band_1.BandTime.hour == BAND_INVALID_VALUE || Band_1.BandTime.minute == BAND_INVALID_VALUE || Band_1.BandDate.day == BAND_INVALID_VALUE ||  Band_1.BandDate.month== BAND_INVALID_VALUE ||
	   Band_2.BandTime.hour == BAND_INVALID_VALUE || Band_2.BandTime.minute == BAND_INVALID_VALUE || Band_2.BandDate.day == BAND_INVALID_VALUE || Band_2.BandDate.month == BAND_INVALID_VALUE)
	   {
		   InvalidBand = true;
		   Flags.BandOk = false;
		   SetBandInvalid();	   
	   }
	if(!InvalidBand)
	{
		if((PresentTime.hour >= Band_1.BandTime.hour && PresentTime.hour <= Band_2.BandTime.hour) && (PresentTime.minute >= Band_1.BandTime.minute && PresentTime.minute <= Band_2.BandTime.minute) &&
		   (PresentDate.day >= Band_1.BandDate.day && PresentDate.day <= Band_2.BandDate.day) && (PresentDate.month >= Band_1.BandDate.month && PresentDate.month <= Band_2.BandDate.month))
		{
		   Flags.BandOk = true;
		}
		else
		{
		   Flags.BandOk = false;
		}		
	}
}

void SetBandInvalid()
{
  Band_1.BandTime.hour = BAND_INVALID_VALUE;
  Band_1.BandTime.minute = BAND_INVALID_VALUE;
  Band_1.BandDate.day = BAND_INVALID_VALUE;
  Band_1.BandDate.month = BAND_INVALID_VALUE;
  Band_2.BandTime.hour = BAND_INVALID_VALUE;
  Band_2.BandTime.minute = BAND_INVALID_VALUE;
  Band_2.BandDate.day = BAND_INVALID_VALUE;
  Band_2.BandDate.month = BAND_INVALID_VALUE;	
  Flags.AllBandsInvalid = true;
}
#endif
