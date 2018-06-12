#include "LCDLib.h"
#include "TimeLib.h"
#include "PIR_LCD_2.h"
#include "EEPROM_Ard.h"
#include "MenuFunc.h"
#include "Band.h"

extern TIME_DATE_FORMAT PresentTime;
BAND_FORMAT Band;
extern FLAGS Flags;

void BandInit()
{
	short BandValidationValue = 0, ReadBandIndx = 0;
	ReadMemory(BAND_INVALIDATION_VALUE_ADDR, 1, &BandValidationValue);
	TakePresentTime();
	ClearLCD();
	if(BandValidationValue == VALID)
	{
		LCDPrintString(TWO, CENTER_ALIGN, "Ripristino i valori");
		LCDPrintString(THREE, CENTER_ALIGN, "della banda");
		delay(DELAY_MESSAGE_MENU);
		for(ReadBandIndx = INIT_HOUR; ReadBandIndx <= END_MINUTE; ReadBandIndx++)
		{
			ReadBandValues(ReadBandIndx);
		}
		if(Band.InitHour > Band.EndHour)
		{
			Band.InitDay = PresentTime.day;
			Band.EndDay = PresentTime.day + 1;
		}
		Flags.IsBandSetted = true;
		ClearLCD();
	}
	else
	{
		SetBandInvalid();
	}
}

bool CheckBand()
{
	bool InBand = false, BandInvalid = false;
	short  BandValidationValue = INVALID;
	TakePresentTime();
	ReadMemory(BAND_INVALIDATION_VALUE_ADDR, 1, &BandValidationValue);
	if(BandValidationValue == INVALID)
		BandInvalid = true;
	else
		BandInvalid = false;
	if(BandInvalid)
	{
		InBand = false;
		Flags.BandActive = false;
	}
	else
	{
		if(Band.InitHour < Band.EndHour)
		{
			if((Band.InitHour <= PresentTime.hour) && (Band.EndHour > PresentTime.hour))
			{
				InBand = true;
				Flags.BandActive = true;
			}
			else if(Band.EndHour == PresentTime.hour)
			{
				if((Band.EndMinute >= PresentTime.minute))
				{
					InBand = true;
					Flags.BandActive = true;
				}
				else
				{
					InBand = false;
					Flags.BandActive = false;
				}
			}
			else
			{
				InBand = false;
				Flags.BandActive = false;
			}
		}
		else if(Band.InitHour == Band.EndHour)
		{
			if((Band.InitMinute <= PresentTime.minute) && (Band.EndMinute >= PresentTime.minute))
			{
				InBand = true;
				Flags.BandActive = true;
			}
			else
			{
				InBand = false;
				Flags.BandActive = false;
			}
		}
		else if(Band.InitHour > Band.EndHour)
		{
			if((PresentTime.hour < 24) && (Band.InitHour <= PresentTime.hour))
			{
				InBand = true;
				Flags.BandActive = true;
			}
			else if(PresentTime.hour <= Band.EndHour)
			{
				if(PresentTime.hour < Band.EndHour)
				{
					InBand = true;
					Flags.BandActive = true;
				}
				else
				{
					if(PresentTime.minute <= Band.EndMinute)
					{
						InBand = true;
						Flags.BandActive = true;
					}
					else
					{
						InBand = false;
						Flags.BandActive = false;
					}
				}
			}
			else
			{
				InBand = false;
				Flags.BandActive = false;
			}
		}
	}
	return InBand;
}

bool IsBandCorrect()
{
	bool BandCorrect = false;
	short BandSaveIdx = 0;
	if(Band.InitHour <= Band.EndHour)
	{
		if(Band.InitHour == Band.EndHour)
		{
			if(Band.InitMinute < Band.EndMinute)
			{
				BandCorrect = true;
				for(BandSaveIdx = INIT_HOUR; BandSaveIdx <= END_MINUTE; BandSaveIdx++)
				{
					SaveBandValues(BandSaveIdx);
				}
				WriteMemory(BAND_INVALIDATION_VALUE_ADDR, VALID);
			}
			else
			{
				BandCorrect = false;
			}
		}
		else if(Band.InitHour < Band.EndHour)
		{
			BandCorrect = true;
			for(BandSaveIdx = INIT_HOUR; BandSaveIdx <= END_MINUTE; BandSaveIdx++)
			{
				SaveBandValues(BandSaveIdx);
			}
			WriteMemory(BAND_INVALIDATION_VALUE_ADDR, VALID);
		}
	}
	else
	{
		TakePresentTime();
		if(Band.InitDay < Band.EndDay)
		{
			BandCorrect = true;
			for(BandSaveIdx = INIT_HOUR; BandSaveIdx <= END_MINUTE; BandSaveIdx++)
			{
				SaveBandValues(BandSaveIdx);
			}
			WriteMemory(BAND_INVALIDATION_VALUE_ADDR, VALID);
		}
		else
		{
			BandCorrect = false;
		}

	}
	return BandCorrect;
}


void SetBandInvalid()
{
	Band.InitHour = INVALID_BAND_VALUE;
	Band.InitMinute = INVALID_BAND_VALUE;
	Band.EndHour = INVALID_BAND_VALUE;
	Band.EndMinute = INVALID_BAND_VALUE;
	ClearLCD();
	LCDPrintString(TWO, CENTER_ALIGN, "Banda");
	LCDPrintString(THREE, CENTER_ALIGN, "disabilitata");
	delay(DELAY_MESSAGE_MENU);

	ClearLCD();
	Flags.IsBandSetted = false;
	WriteMemory(BAND_INVALIDATION_VALUE_ADDR, INVALID);
}

void SaveBandValues(short WichItem)
{
	switch (WichItem)
	{
		case INIT_HOUR:
			WriteMemory(BAND_INIT_HOUR_ADDR, Band.InitHour);
			break;
		case INIT_MINUTE:
			WriteMemory(BAND_INIT_MINUTE_ADDR, Band.InitMinute);
			break;
		case END_HOUR:
			WriteMemory(BAND_END_HOUR_ADDR, Band.EndHour);
			break;
		case END_MINUTE:
			WriteMemory(BAND_END_MINUTE_ADDR, Band.EndMinute);
			break;
		default:
			break;
	}
}

void ReadBandValues(short WichItem)
{
	switch (WichItem)
	{
		case INIT_HOUR:
			ReadMemory(BAND_INIT_HOUR_ADDR, 1, &Band.InitHour);
			break;
		case INIT_MINUTE:
			ReadMemory(BAND_INIT_MINUTE_ADDR, 1,&Band.InitMinute);
			break;
		case END_HOUR:
			ReadMemory(BAND_END_HOUR_ADDR, 1, &Band.EndHour);
			break;
		case END_MINUTE:
			ReadMemory(BAND_END_MINUTE_ADDR, 1, &Band.EndMinute);
			break;
		default:
			break;
	}
}


bool SetTimeBand()
{
	bool ExitSetTimeBand = false, ValidSet = false;
	short ButtonPress, TimeVar = INIT_HOUR;
	short Hour = PresentTime.hour, Minute = PresentTime.minute;
	short OldHour = PresentTime.hour, OldMinute = PresentTime.minute;
	String MinuteStr;
	ClearLCD();

	LCDPrintString(ONE  , CENTER_ALIGN, "Cambiare l'orario");
	LCDPrintString(TWO  , CENTER_ALIGN, "della banda");
	LCDPrintString(THREE, CENTER_ALIGN, "con Up e Down");
	delay(DELAY_MESSAGE_MENU);
	ClearLCD();
	LCDPrintString(ONE  , CENTER_ALIGN, "Premere Ok/Set");
	LCDPrintString(TWO  , CENTER_ALIGN, "per confermare");
	LCDPrintString(THREE, CENTER_ALIGN, "Premere Left/Back");
	LCDPrintString(FOUR , CENTER_ALIGN, "per uscire");
	delay(DELAY_MESSAGE_MENU);
	ClearLCD();

	while(!ExitSetTimeBand)
	{
		TakePresentTime();

		LCDPrintString(ONE  , CENTER_ALIGN, "Cambiare l'orario");
		LCDPrintString(TWO  , CENTER_ALIGN, "della banda");
		switch(TimeVar)
		{
			case INIT_HOUR:
				ButtonPress = CheckButtons();
				LCDPrintString(THREE, CENTER_ALIGN, "Ora iniziale:");
				LCDPrintValue(FOUR, CENTER_ALIGN, Hour);

				switch(ButtonPress)
				{
					case UP:
						BlinkLed(YELLOW_LED);
						if(Hour > 0)
							Hour--;
						else
							Hour = HOUR_IN_DAY;
						ClearLCD();
						break;
					case DOWN:
						BlinkLed(YELLOW_LED);
						if(Hour < HOUR_IN_DAY)
							Hour++;
						else
							Hour = 0;
						ClearLCD();
						break;
					case OK_EXIT:
						BlinkLed(YELLOW_LED);
						if(Hour != OldHour && Hour > OldHour)
						{
							ClearLCD();
							LCDPrintString(TWO, CENTER_ALIGN, "Valore Salvato");
							delay(DELAY_MESSAGE_MENU);
							Band.InitHour = Hour;
							ValidSet = true;
							TimeVar = INIT_MINUTE;
							ClearLCD();
						}
						else if(Hour != OldHour && Hour < OldHour)
						{
							ClearLCD();
							LCDPrintString(TWO, CENTER_ALIGN,   "Valore non valido");
							LCDPrintString(THREE, CENTER_ALIGN, "Re-inserire l'ora");
							delay(DELAY_MESSAGE_MENU);
							ClearLCD();
							ValidSet = false;
							TimeVar = INIT_HOUR;
						}
						else if(Hour == OldHour)
						{
							ClearLCD();
							LCDPrintString(TWO, CENTER_ALIGN, "Valore uguale");
							delay(DELAY_MESSAGE_MENU);
							ValidSet = true;
							Band.InitHour = Hour;
							TimeVar = INIT_MINUTE;
							ClearLCD();
						}
						break;
					default:
						break;
				}
				ButtonPress = NOPRESS;
				break;
			case INIT_MINUTE:
				ButtonPress = CheckButtons();
				MinuteStr = "0";
				LCDPrintString(THREE, CENTER_ALIGN, "Minuti:");

				if(Minute < 10)
				{
					MinuteStr += String(Minute);
				}
				else
				{
					MinuteStr = String(Minute);
				}
				LCDPrintString(FOUR, CENTER_ALIGN, MinuteStr);
				switch(ButtonPress)
				{
					case UP:
						BlinkLed(YELLOW_LED);
						if(Minute > 0)
							Minute--;
						else
							Minute = MINUTE_IN_HOUR;
						ClearLCD();
						break;
					case DOWN:
						BlinkLed(YELLOW_LED);
						if(Minute < MINUTE_IN_HOUR)
							Minute++;
						else
							Minute = 0;
						ClearLCD();
						break;
					case OK_EXIT:
						BlinkLed(YELLOW_LED);
						if(Hour != OldHour)
						{
							ClearLCD();
							LCDPrintString(TWO, CENTER_ALIGN, "Valore Salvato");
							delay(DELAY_MESSAGE_MENU);
							Band.InitMinute = Minute;
							ValidSet = true;
							TimeVar = END_HOUR;
							ClearLCD();
							WriteMemory(BAND_INVALIDATION_VALUE_ADDR, 0);
						}
						else
						{
							if(Minute > OldMinute)
							{
								ClearLCD();
								LCDPrintString(TWO, CENTER_ALIGN, "Valore Salvato");
								delay(DELAY_MESSAGE_MENU);
								Band.InitMinute = Minute;
								ValidSet = true;
								TimeVar = END_HOUR;
								ClearLCD();
								WriteMemory(BAND_INVALIDATION_VALUE_ADDR, 0);
							}
							else
							{
								ClearLCD();
								LCDPrintString(TWO  , CENTER_ALIGN, "Valore non valido");
								LCDPrintString(THREE, CENTER_ALIGN, "Re-inserire i");
								LCDPrintString(FOUR , CENTER_ALIGN, "minuti");
								delay(DELAY_MESSAGE_MENU);
								ClearLCD();
								ValidSet = false;
								TimeVar = INIT_MINUTE;
							}
						}
						break;
					default:
						break;
				}
				ButtonPress = NOPRESS;
				break;
			case END_HOUR:
				ButtonPress = CheckButtons();
				LCDPrintString(THREE, CENTER_ALIGN, "Ora finale:");
				LCDPrintValue(FOUR, CENTER_ALIGN, Hour);

				switch(ButtonPress)
				{
					case UP:
						BlinkLed(YELLOW_LED);
						if(Hour > 0)
							Hour--;
						else
							Hour = HOUR_IN_DAY;
						ClearLCD();
						break;
					case DOWN:
						BlinkLed(YELLOW_LED);
						if(Hour < HOUR_IN_DAY)
							Hour++;
						else
							Hour = 0;
						ClearLCD();
						break;
					case OK_EXIT:
						BlinkLed(YELLOW_LED);
						if(Hour != OldHour && Hour > OldHour)
						{
							ClearLCD();
							LCDPrintString(TWO, CENTER_ALIGN, "Valore Salvato");
							delay(DELAY_MESSAGE_MENU);
							Band.EndHour = Hour;
							ValidSet = true;
							TimeVar = END_MINUTE;
							ClearLCD();
						}
						else if(Hour != OldHour && Hour < OldHour)
						{
							ClearLCD();
							LCDPrintString(TWO, CENTER_ALIGN, "L'ora e' valida");
							LCDPrintString(THREE, CENTER_ALIGN, "per domani");
							delay(DELAY_MESSAGE_MENU);
							ValidSet = true;
							Band.EndHour = Hour;
							Band.InitDay = PresentTime.day;
							Band.EndDay = PresentTime.day + 1;
							TimeVar = END_MINUTE;
							ClearLCD();
						}
						else if(Hour == OldHour)
						{
							ClearLCD();
							LCDPrintString(TWO, CENTER_ALIGN, "Valore uguale");
							delay(DELAY_MESSAGE_MENU);
							ValidSet = true;
							Band.EndHour = Hour;
							TimeVar = END_MINUTE;
							ClearLCD();
						}
						break;
					default:
						break;
				}
				ButtonPress = NOPRESS;
				break;
			case END_MINUTE:
				ButtonPress = CheckButtons();
				MinuteStr = "0";
				LCDPrintString(THREE, CENTER_ALIGN, "Minuti:");

				if(Minute < 10)
				{
					MinuteStr += String(Minute);
				}
				else
				{
					MinuteStr = String(Minute);
				}
				LCDPrintString(FOUR, CENTER_ALIGN, MinuteStr);
				switch(ButtonPress)
				{
					case UP:
						BlinkLed(YELLOW_LED);
						if(Minute > 0)
							Minute--;
						else
							Minute = MINUTE_IN_HOUR;
						ClearLCD();
						break;
					case DOWN:
						BlinkLed(YELLOW_LED);
						if(Minute < MINUTE_IN_HOUR)
							Minute++;
						else
							Minute = 0;
						ClearLCD();
						break;
					case OK_EXIT:
						BlinkLed(YELLOW_LED);
						if(Hour != OldHour)
						{
							ClearLCD();
							LCDPrintString(TWO, CENTER_ALIGN, "Valore Salvato");
							delay(DELAY_MESSAGE_MENU);
							Band.EndMinute = Minute;
							ValidSet = true;
							TimeVar = EXIT;
							WriteMemory(BAND_INVALIDATION_VALUE_ADDR, 0);
							ClearLCD();
						}
						else
						{
							if(Minute > OldMinute)
							{
								ClearLCD();
								LCDPrintString(TWO, CENTER_ALIGN, "Valore Salvato");
								delay(DELAY_MESSAGE_MENU);
								Band.EndMinute = Minute;
								ValidSet = true;
								TimeVar = EXIT;
								WriteMemory(BAND_INVALIDATION_VALUE_ADDR, 0);
								ClearLCD();
							}
							else
							{
								ClearLCD();
								LCDPrintString(TWO, CENTER_ALIGN, "Valore non valido");
								LCDPrintString(THREE, CENTER_ALIGN, "Re-inserire i");
								LCDPrintString(FOUR, CENTER_ALIGN, "minuti");
								delay(DELAY_MESSAGE_MENU);
								ClearLCD();
								ValidSet = false;
								TimeVar = END_MINUTE;
							}
						}
						break;
					default:
						break;
				}
				ButtonPress = NOPRESS;
				break;
			case EXIT:
				ExitSetTimeBand = true;
				ClearLCD();
				break;
			default:
				break;

		}
		delay(WHILE_LOOP_DELAY);
	}
	return ValidSet;
}
