#include "PIR_LCD_2.h"
#include "EEPROM_Ard.h"
#include "Band_Func.h"
#include "MenuFunc.h"

#ifdef RTC_INSERTED
extern TIME_BAND Band_1;
extern TIME_BAND Band_2;
extern DATE_FORMAT PresentDate;
extern TIME_FORMAT PresentTime;
#endif

extern EEPROM_ITEM EepromTab[];

extern FLAGS  Flags;
extern bool SetupOk;

static const String OnOff[2] = {"On", "Off"};

#ifdef RTC_INSERTED
static const short TabDays4Month[] = 
{
  31,
  28,
  31,
  30,
  31,
  30,
  31,
  31,
  30,
  31,
  30,
  31,
};
#endif

bool EnterSetupButton()
{
  SetupOk = digitalRead(BUTTON_SETUP);
  if(SetupOk == HIGH)
  {
    BlinkLed(YELLOW_LED);
    SetupOk = LOW;
    Flags.Setup = true;
  }
  else
  {
    Flags.Setup = false;
  }
  return Flags.Setup;
}

bool ChangeValue()
{
  bool OkButton = false;
  short numReg;
  short ButtonPress;
  ReadMemory(NUM_REG_ADDR, 1, &numReg);
  ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
  short oldDelayAmount = EepromTab[DELAY_AMOUNT].eeprom_par_value;
  short ChangeDelayAmount = EepromTab[DELAY_AMOUNT].eeprom_par_value;
  String TimeOverSixty;
  short Minute = 0, Second = 0;

  // Pulire LCD
  ClearLCD();
  LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
  LCDPrintString(1,CENTER_ALIGN,"to change the value");
  LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");

  delay(1000);
  // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
  // premere su setup per dare l'ok
  ClearLCD();
  while(!OkButton)
  {
	ButtonPress = ChekButtons();
    delay(BUTTON_PRESS_DELAY);
    // Pulire LCD
	
	if(ChangeDelayAmount <= 60)
	{
		LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
		LCDPrintString(1,CENTER_ALIGN,"to change the value:");
		LCDPrintLineVoid(2);
		LCDPrintValue(2,8, ChangeDelayAmount);
		LCDPrintString(2,11,"sec");
		LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");		
	}
	else
	{
		Minute = ChangeDelayAmount / 60;
		Second = ChangeDelayAmount % 60;
		if(Second < 10)
		{
			TimeOverSixty = String(Minute) + ":0" + String(Second) + "min";	
		}
		else
		{
			TimeOverSixty = String(Minute) + ":" + String(Second) + "min";	
		}
	    LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
		LCDPrintString(1,CENTER_ALIGN,"to change the value:");
		LCDPrintLineVoid(2);
		LCDPrintString(2,CENTER_ALIGN, TimeOverSixty);
		LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
	}

    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
    // premere su setup per dare l'ok

	switch(ButtonPress)
	{
		case UP:
		    BlinkLed(YELLOW_LED); // blink giallo
			if(ChangeDelayAmount == 0)
			{
				ChangeDelayAmount == MIN_LIGHT_DELAY;
			}
			ChangeDelayAmount++;
			if(ChangeDelayAmount > MAX_LIGHT_DELAY) // Raggiunti i secondi massimi per il delay nella gestione pir
			{
				ChangeDelayAmount = MIN_LIGHT_DELAY;
			}
			break;
		case DOWN:
		    BlinkLed(YELLOW_LED); // blink giallo
			ChangeDelayAmount--;
			if(ChangeDelayAmount < MIN_LIGHT_DELAY) // Raggiunti i secondi minimi per il delay nella gestione pir
			{
				ChangeDelayAmount = MAX_LIGHT_DELAY;
			}
			break;
		case OK_EXIT:
		    BlinkLed(YELLOW_LED); // blink giallo
			// Scrivere su LCD "Valori salvati"
			ClearLCD();
			if(oldDelayAmount != ChangeDelayAmount)
			{
				LCDPrintString(1,CENTER_ALIGN,"Value Saved!");
				WriteMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, ChangeDelayAmount);
				ReadMemory(NUM_REG_ADDR, 1, (short*)&numReg);
				ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &ChangeDelayAmount);        
			}
			else
			{
				LCDPrintString(1,CENTER_ALIGN,"Value Unchanged");
			}
			OkButton = true;       
			delay(2000);
			break;
		default:
			break;
		
	}
  }
  OFF(BLUE_LED);
  ClearLCD();
  return OkButton;
}


bool SwichState()
{
  short ButtonPress;
  bool OkSwitch = false;
  ReadMemory(EepromTab[PIR_STATE].eeprom_par_value, EepromTab[PIR_STATE].eeprom_par_numReg, &EepromTab[PIR_STATE].eeprom_par_value);
  short OldSwitch = EepromTab[PIR_STATE].eeprom_par_value;
  short SwitchOnOff = TURN_OFF;

  // Pulire LCD
  ClearLCD();
  LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
  LCDPrintString(1,CENTER_ALIGN,"to change the state");
  LCDPrintString(2,CENTER_ALIGN,"On or Off");
  LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");

  delay(2000);
  // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
  // premere su setup per dare l'ok
  ClearLCD();
  while(!OkSwitch)
  {
	ButtonPress = ChekButtons();
    delay(BUTTON_PRESS_DELAY);
    // Pulire LCD
    LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(1,CENTER_ALIGN,"to change the state");
    LCDPrintLineVoid(2);
    LCDPrintString(2, CENTER_ALIGN, OnOff[SwitchOnOff]);
    LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
    // premere su setup per dare l'ok

	switch(ButtonPress)
	{
		case UP:
			BlinkLed(YELLOW_LED); // blink giallo
			SwitchOnOff++;
			if(SwitchOnOff > 1)
				SwitchOnOff = TURN_ON;
			break;
		case DOWN:
			BlinkLed(YELLOW_LED); // blink giallo
			SwitchOnOff--;
			if(SwitchOnOff < 0)
				SwitchOnOff = TURN_OFF;
			break;
		case OK_EXIT:
			BlinkLed(YELLOW_LED); // blink giallo
		// Scrivere su LCD "Valori salvati"
			ClearLCD();
			LCDPrintString(1, CENTER_ALIGN, "The state is");
			LCDPrintString(2, CENTER_ALIGN, OnOff[SwitchOnOff]);
      
			if(SwitchOnOff != OldSwitch)
			{
				WriteMemory(EepromTab[PIR_STATE].eeprom_par_addr, SwitchOnOff);
			}
      
			OkSwitch = true;       
			delay(2000);
			break;
		default:
			break;
		
	}
  }
  OFF(BLUE_LED);
  ClearLCD();
  return OkSwitch;
    
}

#ifdef RTC_INSERTED

bool ChangeDateTime(TIME_BAND  Band)
{
  short ButtonPress;
  bool ExitDateTime = false;
  TIME_FORMAT ChangedTime;
  DATE_FORMAT ChangedDate;
  short   ChangeState = CHANGE_MINUTE;
  ChangedTime = PresentTime;
  ChangedDate = PresentDate;
  ON(BLUE_LED);
  ON(RED_LED);
  delay(2000);
  // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
  // premere su setup per dare l'ok
  
  // CAMBIA ORE
  while(!ExitDateTime)
  {
	ButtonPress = ChekButtons();
	delay(BUTTON_PRESS_DELAY);
	switch(ChangeState)
	{	
		case CHANGE_HOUR:
			ClearLCD();
			// Pulire LCD
			LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
			LCDPrintString(1,CENTER_ALIGN,"to change the hour");
			LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
			LCDPrintValue(3, CENTER_ALIGN, ChangedTime.hour);
			// Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
			// premere su setup per dare l'ok
			switch(ButtonPress)
			{
				case UP:
					BlinkLed(YELLOW_LED); // blink giallo
					ChangedTime.hour++;
					if(ChangedTime.hour > 23 && ChangedTime.hour != BAND_INVALID_VALUE)
						ChangedTime.hour = BAND_INVALID_VALUE;
					if(ChangedTime.hour >= BAND_INVALID_VALUE)
						ChangedTime.hour = 0;
					break;
				case DOWN:
					BlinkLed(YELLOW_LED); // blink giallo
					ChangedTime.hour--;
					if(ChangedTime.hour < 0 && ChangedTime.hour != BAND_INVALID_VALUE)
						ChangedTime.hour = BAND_INVALID_VALUE;
					if(ChangedTime.hour <= BAND_INVALID_VALUE)
						ChangedTime.hour = 23;
					break;
				case OK_EXIT:
					BlinkLed(YELLOW_LED); // blink giallo
					// Scrivere su LCD "Valori salvati"
					ClearLCD();      
					if(ChangedTime.hour != PresentTime.hour && ChangedTime.hour != BAND_INVALID_VALUE)
					{
						LCDPrintString(1, CENTER_ALIGN, "Saved!");
						// scrivere il nuovo orario
						Band.BandTime.hour = ChangedTime.hour;
					}
					else if(ChangedTime.hour != PresentTime.hour && ChangedTime.hour == BAND_INVALID_VALUE)
					{
						LCDPrintString(0, CENTER_ALIGN, "Saved!");
						LCDPrintString(1, CENTER_ALIGN, "Invalidated value");
						LCDPrintString(2, CENTER_ALIGN, "This will turn OFF");
						LCDPrintString(3, CENTER_ALIGN, "the sensor");
						// scrivere il nuovo orario
						Band.BandTime.hour = ChangedTime.hour;
					}
					else
					{
						LCDPrintString(1, CENTER_ALIGN, "Unchanged");  
					} 
					ChangeState = CHANGE_DAY;
					delay(1000);
					break;
				default:
					break;	
			}
			break;
  
  //CAMBIA MINUTI
		case CHANGE_MINUTE:
			ClearLCD();
			// Pulire LCD
			LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
			LCDPrintString(1,CENTER_ALIGN,"to change the minutes");
			LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
			LCDPrintValue(3, CENTER_ALIGN, ChangedTime.minute);
			// Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
			// premere su setup per dare l'ok
			switch(ButtonPress)
			{
				case UP:
					BlinkLed(YELLOW_LED); // blink giallo
					ChangedTime.minute++;
					if(ChangedTime.minute > 59 && ChangedTime.minute != BAND_INVALID_VALUE)
						ChangedTime.minute = BAND_INVALID_VALUE;
					if(ChangedTime.minute >= BAND_INVALID_VALUE)
						ChangedTime.minute = 0;
					break;
				case DOWN:
					BlinkLed(YELLOW_LED); // blink giallo
					ChangedTime.minute--;
					if(ChangedTime.minute < 0 && ChangedTime.minute != BAND_INVALID_VALUE)
						ChangedTime.minute = BAND_INVALID_VALUE;
					if(ChangedTime.minute <= BAND_INVALID_VALUE)
						ChangedTime.minute = 59;
						break;
				case OK_EXIT:
					BlinkLed(YELLOW_LED); // blink giallo
					// Scrivere su LCD "Valori salvati"
					ClearLCD();      
					if(ChangedTime.minute != PresentTime.minute && ChangedTime.minute != BAND_INVALID_VALUE)
					{
						LCDPrintString(1, CENTER_ALIGN, "Saved!");
						// scrivere il nuovo orario
						Band.BandTime.minute = ChangedTime.minute;
					}
					else if(ChangedTime.minute != PresentTime.minute && ChangedTime.minute == BAND_INVALID_VALUE)
					{
						LCDPrintString(0, CENTER_ALIGN, "Saved!");
						LCDPrintString(1, CENTER_ALIGN, "Invalidated value");
						LCDPrintString(2, CENTER_ALIGN, "This will turn OFF");
						LCDPrintString(3, CENTER_ALIGN, "the sensor");
						// scrivere il nuovo orario
						Band.BandTime.minute = ChangedTime.minute;
					}
					else
					{
						LCDPrintString(1, CENTER_ALIGN, "Unchanged");  
					}           
					delay(1000);
					ChangeState = CHANGE_HOUR;
					break;
				default:
					break;	
			}
			break;
  
  
  // CAMBIA GIORNO
		case CHANGE_DAY:
			ClearLCD();
			// Pulire LCD
			LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
			LCDPrintString(1,CENTER_ALIGN,"to change the day");
			LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
			  LCDPrintValue(3, CENTER_ALIGN, ChangedDate.day);
			// Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
			// premere su setup per dare l'ok
			switch(ButtonPress)
			{
				case UP:
					BlinkLed(YELLOW_LED); // blink giallo
					ChangedDate.day++;
					if(ChangedDate.day > TabDays4Month[PresentDate.month] && ChangedDate.day != BAND_INVALID_VALUE)
						ChangedDate.day  = BAND_INVALID_VALUE;
					if(ChangedDate.day >= BAND_INVALID_VALUE)
						ChangedDate.day  = 1;
					break;
				case DOWN:
					BlinkLed(YELLOW_LED); // blink giallo
					ChangedDate.day--;
					if(ChangedDate.day < 1 && ChangedDate.day != BAND_INVALID_VALUE)
						ChangedDate.day  = BAND_INVALID_VALUE;
					if(ChangedDate.day <= BAND_INVALID_VALUE)
						ChangedDate.day = TabDays4Month[PresentDate.month];
					break;
				case OK_EXIT:
					BlinkLed(YELLOW_LED); // blink giallo
					// Scrivere su LCD "Valori salvati"
					ClearLCD();      
					if(ChangedDate.day != PresentDate.day && ChangedDate.day != BAND_INVALID_VALUE)
					{
						LCDPrintString(1, CENTER_ALIGN, "Saved!");
						// scrivere il nuovo orario
						Band.BandDate.day = ChangedDate.day;
					}
					else if(ChangedDate.day != PresentDate.day && ChangedDate.day == BAND_INVALID_VALUE)
					{
						LCDPrintString(0, CENTER_ALIGN, "Saved!");
						LCDPrintString(1, CENTER_ALIGN, "Invalidated value");
						LCDPrintString(2, CENTER_ALIGN, "This will turn OFF");
						LCDPrintString(3, CENTER_ALIGN, "the sensor");
						// scrivere il nuovo orario
						Band.BandDate.day = ChangedDate.day;
					}
					else
					{
						LCDPrintString(1, CENTER_ALIGN, "Unchanged");  
					}           
					delay(1000);
					ChangeState = CHANGE_MONTH;
					break;
				default:
					break;	
			}
			break;
  
 
  // CAMBIA MESE
		case CHANGE_MONTH:
			ClearLCD();
			// Pulire LCD
			LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
			LCDPrintString(1,CENTER_ALIGN,"to change the month");
			LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
			LCDPrintValue(3, CENTER_ALIGN, ChangedDate.month);
			// Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
			// premere su setup per dare l'ok
			switch(ButtonPress)
			{
				case UP:
					BlinkLed(YELLOW_LED); // blink giallo
					ChangedDate.month++;
					if(ChangedDate.month > 12 && ChangedDate.month != BAND_INVALID_VALUE)
						ChangedDate.month  = BAND_INVALID_VALUE;
					if(ChangedDate.month >= BAND_INVALID_VALUE)
						ChangedDate.month = 1;
					break;
				case DOWN:
					BlinkLed(YELLOW_LED); // blink giallo
					ChangedDate.month--;
					if(ChangedDate.month < 1 && ChangedDate.month != BAND_INVALID_VALUE)
						ChangedDate.month  = BAND_INVALID_VALUE;
					if(ChangedDate.month <= BAND_INVALID_VALUE)
						ChangedDate.month = 12;
					break;
				case OK_EXIT:
					BlinkLed(YELLOW_LED); // blink giallo
					// Scrivere su LCD "Valori salvati"
					ClearLCD();      
					if(ChangedDate.month != PresentDate.month && ChangedDate.month != BAND_INVALID_VALUE)
					{
						LCDPrintString(1, CENTER_ALIGN, "Saved!");
						// scrivere il nuovo orario
						Band.BandDate.month = ChangedDate.month;
					}
					else if(ChangedDate.month != PresentDate.month && ChangedDate.month == BAND_INVALID_VALUE)
					{
						LCDPrintString(0, CENTER_ALIGN, "Saved!");
						LCDPrintString(1, CENTER_ALIGN, "Invalidated value");
						LCDPrintString(2, CENTER_ALIGN, "This will turn OFF");
						LCDPrintString(3, CENTER_ALIGN, "the sensor");
						// scrivere il nuovo orario
						Band.BandDate.month = ChangedDate.month;
					}
					else
					{
						LCDPrintString(1, CENTER_ALIGN, "Unchanged");  
					}           
					delay(1000);
					ChangeState = EXIT;
					break;
				default:
					break;	
			}
			break;
	    case EXIT:
			ExitDateTime = true;
			break;
		default:
			ChangeState = CHANGE_HOUR;
			break;
	}
  }
  return ExitDateTime;
}
#endif

bool InfoScroll()
{
  short ButtonPress;
  bool ExitInfo = false;
  short Page = MIN_INFO_PAGES;
  short numReg;
  String tmpEepromValue;
  short Minute = 0, Second = 0;
#ifdef RTC_INSERTED
  String TimeStr, DateStr;
#endif 
 
  OFF(RED_LED);
  ON(GREEN_LED);
  ON(BLUE_LED);
  LCDDisplayOn();
  
  Flags.Backlight = true;

  // Pulire LCD
  ClearLCD();
  LCDPrintString(1,CENTER_ALIGN,"Press Up, Down");
  LCDPrintString(2,CENTER_ALIGN,"to scroll the info");
  LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
  
  delay(2000);
  
  while(!ExitInfo)
  {
	ButtonPress = ChekButtons();
    delay(BUTTON_PRESS_DELAY);
    if(Page == DELAY_AMOUNT)
    {
      ReadMemory(NUM_REG_ADDR, 1, (short*)&numReg);
	  ReadMemory(EepromTab[Page].eeprom_par_addr, numReg, &EepromTab[Page].eeprom_par_value);

    }
	else
	{
	  ReadMemory(EepromTab[Page].eeprom_par_addr, EepromTab[Page].eeprom_par_numReg, &EepromTab[Page].eeprom_par_value);	
	}
   
    // Pulire LCD
    ClearLCD();
    LCDPrintString(0, CENTER_ALIGN, EepromTab[Page].eeprom_par_name);
    if(EepromTab[Page].typeMenu == SWITCH_STATE)
    {
      if(EepromTab[Page].eeprom_par_value == 0)
        LCDPrintString(2, CENTER_ALIGN, "On");
      else
        LCDPrintString(2, CENTER_ALIGN, "Off");
    }
#ifdef RTC_INSERTED
	else if(EepromTab[Page].typeMenu == TIME_BAND_NUM)
	{
		// Info sulle bande
		if(Page >= HOUR_BAND_1 && Page <= MONTH_BAND_2)
		{
			TimeStr = String(Band_1.BandTime.hour) + ":" + String(Band_1.BandTime.minute);
			DateStr = String(Band_1.BandDate.day) + "/" + String(Band_1.BandDate.month);
			LCDPrintString(0, CENTER_ALIGN, "Band 1 Time Set:");
			LCDPrintString(1, LEFT_ALIGN, TimeStr);
			LCDPrintString(1, RIGHT_ALIGN, DateStr);
			TimeStr = String(Band_2.BandTime.hour) + ":" + String(Band_2.BandTime.minute);
			DateStr = String(Band_2.BandDate.day) + "/" + String(Band_2.BandDate.month);
			LCDPrintString(2, CENTER_ALIGN, "Band 2 Time Set:");
			LCDPrintString(3, LEFT_ALIGN, TimeStr);
			LCDPrintString(3, RIGHT_ALIGN, DateStr);
		}
	}
#endif
    else // Change Value
    {	
		if(EepromTab[Page].eeprom_par_value <= 60)
		{
			tmpEepromValue = String(EepromTab[Page].eeprom_par_value);
			tmpEepromValue = String(tmpEepromValue + "sec");
			LCDPrintString(2, CENTER_ALIGN, tmpEepromValue);			
			
		}

		else
		{
			Minute = EepromTab[Page].eeprom_par_value / 60;
			Second = EepromTab[Page].eeprom_par_value % 60;
			if(Second < 10)
			{
				tmpEepromValue = String(Minute) + ":0" + String(Second) + "min";	
			}
			else
				tmpEepromValue = String(Minute) + ":" + String(Second) + "min";	
			LCDPrintString(2, CENTER_ALIGN, tmpEepromValue);	
		}
	}
	switch(ButtonPress)
	{
		case UP:
			BlinkLed(YELLOW_LED); // blink giallo
			Page++;
			if(Page > MAX_EEPROM_ITEM - 1) 
			{
				Page = MIN_INFO_PAGES;
			}
			break;
		case DOWN:
			BlinkLed(YELLOW_LED); // blink giallo
			Page--;
			if(Page < MIN_INFO_PAGES) 
			{
				Page = MAX_EEPROM_ITEM - 1;
			}
			break;
		case OK_EXIT:
			BlinkLed(YELLOW_LED); // blink giallo
			// Scrivere su LCD Valori salvati
			ClearLCD();
			LCDPrintString(1, CENTER_ALIGN, "Exit...");
			delay(2000);
			ExitInfo = true;
			break;
		default:
			break;	
	}
  }
  OFF(GREEN_LED);
  OFF(BLUE_LED);
  return ExitInfo;
}

#ifdef RTC_INSERTED
bool ChangeTimeBands()
{
	ClearLCD();
	LCDDisplayOn();
	Flags.Backlight = true;
	bool ExitChangeBand = false;
	short StateChangeBand = CHANGE_BAND_1;
	if(Flags.AllBandsInvalid)
	{
		LCDPrintString(1, CENTER_ALIGN, "All band are");
		LCDPrintString(2, CENTER_ALIGN, "INVALIDATED");
		delay(2000);
		ClearLCD();
	}
	while(!ExitChangeBand)
	{
		switch(StateChangeBand)
		{
			case CHANGE_BAND_1:
				LCDPrintString(1, CENTER_ALIGN, "Change the");
				LCDPrintString(2, CENTER_ALIGN, "first band");		
				ChangeDateTime(Band_1);
				ClearLCD();
				StateChangeBand = CHANGE_BAND_2;
				ExitChangeBand = false;
				break;
				
			case CHANGE_BAND_2:
				LCDPrintString(0, CENTER_ALIGN, "Change the");
				LCDPrintString(1, CENTER_ALIGN, "second band");	
				LCDPrintString(2, CENTER_ALIGN, "Must be greater");
				LCDPrintString(3, CENTER_ALIGN, "then the first!");
				ChangeDateTime(Band_2);			
				ClearLCD();
				StateChangeBand = CHEK_BAND;
				ExitChangeBand = false;
				break;
			
			case CHEK_BAND:
				if((Band_1.BandTime.hour > Band_2.BandTime.hour) || 
					(Band_1.BandDate.day  > Band_2.BandDate.day ) || 
					(Band_1.BandDate.month > Band_2.BandDate.month))
				{
					LCDPrintString(0, CENTER_ALIGN, "ERROR!");
					LCDPrintString(1, CENTER_ALIGN, "Band 1 > Band 2");
					LCDPrintString(2, CENTER_ALIGN, "Repeat the");
					LCDPrintString(3, CENTER_ALIGN, "procedure");
					ClearLCD();
					StateChangeBand = CHANGE_BAND_1;
				}
				else
				{
					StateChangeBand	= EXIT_CHANGE_BAND;
				}
				ExitChangeBand = false;
				break;
				
			case EXIT_CHANGE_BAND:
				Flags.AllBandsInvalid = false;
				SaveBandToEeprom();	
				ExitChangeBand = true;
				break;
				
			default:
				StateChangeBand = CHANGE_BAND_1;
				ExitChangeBand = false;
				break;	
		}
	}
	return ExitChangeBand;
}
#endif
