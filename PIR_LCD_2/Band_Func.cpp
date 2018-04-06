#include "Band_Func.h"
#include "EEPROM_Ard.h"
#include "PIR_LCD_2.h"

#ifdef RTC_INSERTED
extern TIME_DATE_FORMAT Band_1;
extern TIME_DATE_FORMAT Band_2;
// extern DATE_FORMAT PresentDate;
extern TIME_DATE_FORMAT PresentTime;
#endif
extern short MainMenuPage;
extern EEPROM_ITEM EepromTab[];
// extern bool FlagBandOk;
// extern bool FlagAllBandsInvalid;
extern FLAGS Flags;

#ifdef RTC_INSERTED
void SaveBandToEeprom()
{
	WriteMemory(EepromTab[HOUR_BAND_1].eeprom_par_addr		, Band_1.hour);
	WriteMemory(EepromTab[HOUR_BAND_2].eeprom_par_addr		, Band_2.hour);
	WriteMemory(EepromTab[MINUTE_BAND_1].eeprom_par_addr	, Band_1.minute);
	WriteMemory(EepromTab[MINUTE_BAND_2].eeprom_par_addr 	, Band_2.minute);
	WriteMemory(EepromTab[DAY_BAND_1].eeprom_par_addr		, Band_1.day);
	WriteMemory(EepromTab[DAY_BAND_2].eeprom_par_addr		, Band_2.day);
	WriteMemory(EepromTab[MONTH_BAND_1].eeprom_par_addr		, Band_1.month);
	WriteMemory(EepromTab[MONTH_BAND_2].eeprom_par_addr		, Band_2.month);	
	return;
}

void ReadBandFromEeprom()
{
    ReadMemory(EepromTab[HOUR_BAND_1].eeprom_par_addr	, EepromTab[HOUR_BAND_1].eeprom_par_numReg	, &Band_1.hour);
	ReadMemory(EepromTab[HOUR_BAND_2].eeprom_par_addr	, EepromTab[HOUR_BAND_2].eeprom_par_numReg	, &Band_2.hour);
	ReadMemory(EepromTab[MINUTE_BAND_1].eeprom_par_addr , EepromTab[MINUTE_BAND_1].eeprom_par_numReg, &Band_1.minute);
	ReadMemory(EepromTab[MINUTE_BAND_2].eeprom_par_addr , EepromTab[MINUTE_BAND_2].eeprom_par_numReg, &Band_2.minute);
	ReadMemory(EepromTab[DAY_BAND_1].eeprom_par_addr	, EepromTab[DAY_BAND_1].eeprom_par_numReg	, &Band_1.day);
	ReadMemory(EepromTab[DAY_BAND_2].eeprom_par_addr	, EepromTab[DAY_BAND_2].eeprom_par_numReg	, &Band_2.day);
	ReadMemory(EepromTab[MONTH_BAND_1].eeprom_par_addr	, EepromTab[MONTH_BAND_1].eeprom_par_numReg	, &Band_1.month);
	ReadMemory(EepromTab[MONTH_BAND_2].eeprom_par_addr	, EepromTab[MONTH_BAND_2].eeprom_par_numReg	, &Band_2.month);	
	return;
}

void ChekBandValue()
{
	bool InvalidBand = false;
	
	// if(Band_1.BandTime.hour == BAND_INVALID_VALUE || Band_1.BandTime.minute == BAND_INVALID_VALUE || Band_1.BandDate.day == BAND_INVALID_VALUE ||  Band_1.BandDate.month== BAND_INVALID_VALUE ||
	   // Band_2.BandTime.hour == BAND_INVALID_VALUE || Band_2.BandTime.minute == BAND_INVALID_VALUE || Band_2.BandDate.day == BAND_INVALID_VALUE || Band_2.BandDate.month == BAND_INVALID_VALUE)
	   // {
		   // InvalidBand = true;
		   // Flags.BandOk = false;
		   // SetBandInvalid();	   
	   // }
	if(!Flags.AllBandsInvalid)
	{
		if((PresentTime.hour >= Band_1.hour && PresentTime.hour <= Band_2.hour) && (PresentTime.minute >= Band_1.minute && PresentTime.minute <= Band_2.minute) &&
		   (PresentTime.day >= Band_1.day && PresentTime.day <= Band_2.day) && (PresentTime.month >= Band_1.month && PresentTime.month <= Band_2.month))
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
  Band_1.hour = BAND_INVALID_VALUE;
  Band_1.minute = BAND_INVALID_VALUE;
  Band_1.day = BAND_INVALID_VALUE;
  Band_1.month = BAND_INVALID_VALUE;
  Band_2.hour = BAND_INVALID_VALUE;
  Band_2.minute = BAND_INVALID_VALUE;
  Band_2.day = BAND_INVALID_VALUE;
  Band_2.month = BAND_INVALID_VALUE;	
  Flags.AllBandsInvalid = true;
}
#endif
