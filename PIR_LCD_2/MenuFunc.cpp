#include "PIR_LCD_2.h"
#include "EEPROM_Ard.h"
#include "Band_Func.h"
#include "MenuFunc.h"
#include "LCDLib.h"

#ifdef RTC_INSERTED
extern TIME_DATE_FORMAT Band_1;
extern TIME_DATE_FORMAT Band_2;
extern TIME_DATE_FORMAT PresentTime;
#endif

extern EEPROM_ITEM EepromTab[];
extern short MainMenuPage;
extern FLAGS  Flags;

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
  short SetupOk = READ(BUTTON_SETUP);
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
			ClearLCD();
			break;
		case DOWN:
		    BlinkLed(YELLOW_LED); // blink giallo
			ChangeDelayAmount--;
			if(ChangeDelayAmount < MIN_LIGHT_DELAY) // Raggiunti i secondi minimi per il delay nella gestione pir
			{
				ChangeDelayAmount = MAX_LIGHT_DELAY;
			}
			ClearLCD();
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
	short EepromItem;
	bool OkSwitch = false;
	switch(MainMenuPage)
	{
		case SWITCH_STATE:
			EepromItem = PIR_STATE;
			break;
		case MANUAL_STATE:
			EepromItem = MANUAL_LIGHT;
			break;
		default:
			EepromItem = PIR_STATE;
			break;
	}
	ReadMemory(EepromTab[EepromItem].eeprom_par_value, EepromTab[EepromItem].eeprom_par_numReg, &EepromTab[EepromItem].eeprom_par_value);
	short OldSwitch = EepromTab[EepromItem].eeprom_par_value;
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
				ClearLCD();
				break;
			case DOWN:
				BlinkLed(YELLOW_LED); // blink giallo
				SwitchOnOff--;
				if(SwitchOnOff < 0)
					SwitchOnOff = TURN_OFF;
				ClearLCD();
				break;
			case OK_EXIT:
				BlinkLed(YELLOW_LED); // blink giallo
				// Scrivere su LCD "Valori salvati"
				ClearLCD();
				LCDPrintString(1, CENTER_ALIGN, "The state is");
				LCDPrintString(2, CENTER_ALIGN, OnOff[SwitchOnOff]);
				if(SwitchOnOff != OldSwitch)
				{
					WriteMemory(EepromTab[EepromItem].eeprom_par_addr, SwitchOnOff);
				}
				if(MainMenuPage == MANUAL_STATE && SwitchOnOff == TURN_ON)
				{
					ON(LIGHT_SWITCH);
					Flags.ManualState = true;
				}
				else if(MainMenuPage == MANUAL_STATE && SwitchOnOff == TURN_OFF)
				{
					OFF(LIGHT_SWITCH);
					Flags.ManualState = false;					
				}
				else
					OFF(LIGHT_SWITCH);
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

bool ChangeDateTime(short WichBand)
{
  short ButtonPress;
  bool ExitDateTime = false;
  TIME_DATE_FORMAT ChangedTime;
  // DATE_FORMAT ChangedDate;
  short   ChangeState = CHANGE_HOUR;
  ChangedTime = PresentTime;
  // ChangedDate = PresentDate;
  ON(BLUE_LED);
  ON(RED_LED);
  delay(2000);
  // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
  // premere su setup per dare l'ok
  ClearLCD();
  // CAMBIA ORE
  while(!ExitDateTime)
  {
	ButtonPress = ChekButtons();
	delay(BUTTON_PRESS_DELAY);
	switch(ChangeState)
	{	
		case CHANGE_HOUR:
			
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
					if(ChangedTime.hour == 23)
						ChangedTime.hour = BAND_INVALID_VALUE;
					else if(ChangedTime.hour == BAND_INVALID_VALUE)
						ChangedTime.hour = 0;
					else
						ChangedTime.hour++;
					ClearLCD();
					break;
				case DOWN:
					BlinkLed(YELLOW_LED); // blink giallo
					if(ChangedTime.hour == 0)
						ChangedTime.hour = BAND_INVALID_VALUE;
					else if(ChangedTime.hour == BAND_INVALID_VALUE)
						ChangedTime.hour = 23;
					else
						ChangedTime.hour--;
					ClearLCD();
					break;
				case OK_EXIT:
					BlinkLed(YELLOW_LED); // blink giallo
					// Scrivere su LCD "Valori salvati"
					ClearLCD();					
					if(ChangedTime.hour != PresentTime.hour && ChangedTime.hour != BAND_INVALID_VALUE)
					{
						LCDPrintString(1, CENTER_ALIGN, "Saved!");
						// scrivere il nuovo orario
						if(WichBand == 1)
							Band_1.hour = ChangedTime.hour;
						else
							Band_2.hour = ChangedTime.hour;
					}
					else if(ChangedTime.hour != PresentTime.hour && ChangedTime.hour == BAND_INVALID_VALUE)
					{
						LCDPrintString(0, CENTER_ALIGN, "Saved!");
						LCDPrintString(1, CENTER_ALIGN, "Invalidated value");
						LCDPrintString(2, CENTER_ALIGN, "This will turn OFF");
						LCDPrintString(3, CENTER_ALIGN, "the sensor");
						delay(1000);
						// scrivere il nuovo orario
						if(WichBand == 1)
							Band_1.hour = ChangedTime.hour;
						else
							Band_2.hour = ChangedTime.hour;
					}
					else
					{
						LCDPrintString(1, CENTER_ALIGN, "Unchanged");
						if(WichBand == 1)
							Band_1.hour = ChangedTime.hour;
						else
							Band_2.hour = ChangedTime.hour;						
					} 
					ChangeState = CHANGE_MINUTE;
					delay(1000);
					ClearLCD();
					break;
				default:
					break;	
			}
			break;
  
  //CAMBIA MINUTI
		case CHANGE_MINUTE:
			// Pulire LCD
			LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
			LCDPrintString(1,CENTER_ALIGN,"to change minutes");
			LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
			LCDPrintValue(3, CENTER_ALIGN, ChangedTime.minute);
			// Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
			// premere su setup per dare l'ok
			switch(ButtonPress)
			{
				case UP:
					BlinkLed(YELLOW_LED); // blink giallo
					if(ChangedTime.minute == 59)
						ChangedTime.minute = BAND_INVALID_VALUE;
					else if(ChangedTime.minute == BAND_INVALID_VALUE)
						ChangedTime.minute = 0;
					else
						ChangedTime.minute++;
					ClearLCD();
					break;
				case DOWN:
					BlinkLed(YELLOW_LED); // blink giallo
					if(ChangedTime.minute == 0)
						ChangedTime.minute = BAND_INVALID_VALUE;
					else if(ChangedTime.minute == BAND_INVALID_VALUE)
						ChangedTime.minute = 59;
					else
						ChangedTime.minute--;
					ClearLCD();
					break;
				case OK_EXIT:
					BlinkLed(YELLOW_LED); // blink giallo
					// Scrivere su LCD "Valori salvati"
					ClearLCD();      
					if(ChangedTime.minute != PresentTime.minute && ChangedTime.minute != BAND_INVALID_VALUE)
					{
						LCDPrintString(1, CENTER_ALIGN, "Saved!");
						// scrivere il nuovo orario
						if(WichBand == 1)
							Band_1.minute = ChangedTime.minute;
						else
							Band_2.minute = ChangedTime.minute;
					}
					else if(ChangedTime.minute != PresentTime.minute && ChangedTime.minute == BAND_INVALID_VALUE)
					{
						LCDPrintString(0, CENTER_ALIGN, "Saved!");
						LCDPrintString(1, CENTER_ALIGN, "Invalidated value");
						LCDPrintString(2, CENTER_ALIGN, "This will turn OFF");
						LCDPrintString(3, CENTER_ALIGN, "the sensor");
						delay(1000);
						// scrivere il nuovo orario
						if(WichBand == 1)
							Band_1.minute = ChangedTime.minute;
						else
							Band_2.minute = ChangedTime.minute;
					}
					else
					{
						LCDPrintString(1, CENTER_ALIGN, "Unchanged"); 
						if(WichBand == 1)
							Band_1.minute = ChangedTime.minute;
						else
							Band_2.minute = ChangedTime.minute;						
					}           
					delay(1000);
					ChangeState = CHANGE_DAY;
					ClearLCD();
					break;
	
				default:
					break;	
			}
			break;
  
  
  // CAMBIA GIORNO
		case CHANGE_DAY:

			// Pulire LCD
			LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
			LCDPrintString(1,CENTER_ALIGN,"to change the day");
			LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
			  LCDPrintValue(3, CENTER_ALIGN, ChangedTime.day);
			// Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
			// premere su setup per dare l'ok
			switch(ButtonPress)
			{
				case UP:
					BlinkLed(YELLOW_LED); // blink giallo
					if(ChangedTime.day == TabDays4Month[PresentTime.month])
						ChangedTime.day  = BAND_INVALID_VALUE;
					else if(ChangedTime.day == BAND_INVALID_VALUE)
						ChangedTime.day  = 1;
					else
						ChangedTime.day++;
					ClearLCD();
					break;
				case DOWN:
					BlinkLed(YELLOW_LED); // blink giallo
					if(ChangedTime.day == 1)
						ChangedTime.day  = BAND_INVALID_VALUE;
					else if(ChangedTime.day == BAND_INVALID_VALUE)
						ChangedTime.day = TabDays4Month[PresentTime.month];
					else
						ChangedTime.day--;
					ClearLCD();
					break;
				case OK_EXIT:
					BlinkLed(YELLOW_LED); // blink giallo
					// Scrivere su LCD "Valori salvati"
					ClearLCD();      
					if(ChangedTime.day != PresentTime.day && ChangedTime.day != BAND_INVALID_VALUE)
					{
						LCDPrintString(1, CENTER_ALIGN, "Saved!");
						// scrivere il nuovo orario
						if(WichBand == 1)
							Band_1.day = ChangedTime.day;
						else
							Band_2.day = ChangedTime.day;
					}
					else if(ChangedTime.day != PresentTime.day && ChangedTime.day == BAND_INVALID_VALUE)
					{
						LCDPrintString(0, CENTER_ALIGN, "Saved!");
						LCDPrintString(1, CENTER_ALIGN, "Invalidated value");
						LCDPrintString(2, CENTER_ALIGN, "This will turn OFF");
						LCDPrintString(3, CENTER_ALIGN, "the sensor");
						delay(1000);
						if(WichBand == 1)
							Band_1.day = ChangedTime.day;
						else
							Band_2.day = ChangedTime.day;
					}
					else
					{
						LCDPrintString(1, CENTER_ALIGN, "Unchanged");
						if(WichBand == 1)
							Band_1.day = ChangedTime.day;
						else
							Band_2.day = ChangedTime.day;						
					}           
					delay(1000);
					ChangeState = CHANGE_MONTH;
					ClearLCD();
					break;
				default:
					break;	
			}
			break;
  
 
  // CAMBIA MESE
		case CHANGE_MONTH:
			// Pulire LCD
			LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
			LCDPrintString(1,CENTER_ALIGN,"to change the month");
			LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
			LCDPrintValue(3, CENTER_ALIGN, ChangedTime.month);
			// Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
			// premere su setup per dare l'ok
			switch(ButtonPress)
			{
				case UP:
					BlinkLed(YELLOW_LED); // blink giallo

					if(ChangedTime.month == 12)
						ChangedTime.month  = BAND_INVALID_VALUE;
					else if(ChangedTime.month == BAND_INVALID_VALUE)
						ChangedTime.month = 1;
					else
						ChangedTime.month++;	
					ClearLCD();					
					break;
				case DOWN:
					BlinkLed(YELLOW_LED); // blink giallo
					if(ChangedTime.month == 1)
						ChangedTime.month  = BAND_INVALID_VALUE;
					else if(ChangedTime.month == BAND_INVALID_VALUE)
						ChangedTime.month = 12;
					else
						ChangedTime.month--;
					ClearLCD();
					break;
				case OK_EXIT:
					BlinkLed(YELLOW_LED); // blink giallo
					// Scrivere su LCD "Valori salvati"
					ClearLCD();      
					if(ChangedTime.month != PresentTime.month && ChangedTime.month != BAND_INVALID_VALUE)
					{
						LCDPrintString(1, CENTER_ALIGN, "Saved!");
						// scrivere il nuovo orario
						if(WichBand == 1)
							Band_1.month = ChangedTime.month;
						else
							Band_2.month = ChangedTime.month;
					}
					else if(ChangedTime.month != PresentTime.month && ChangedTime.month == BAND_INVALID_VALUE)
					{
						LCDPrintString(0, CENTER_ALIGN, "Saved!");
						LCDPrintString(1, CENTER_ALIGN, "Invalidated value");
						LCDPrintString(2, CENTER_ALIGN, "This will turn OFF");
						LCDPrintString(3, CENTER_ALIGN, "the sensor");
						delay(1000);
						// scrivere il nuovo orario
						if(WichBand == 1)
							Band_1.month = ChangedTime.month;
						else
							Band_2.month = ChangedTime.month;
					}
					else
					{
						LCDPrintString(1, CENTER_ALIGN, "Unchanged"); 
						if(WichBand == 1)
							Band_1.month = ChangedTime.month;
						else
							Band_2.month = ChangedTime.month;						
					}           
					
					ChangeState = EXIT;
					ClearLCD();
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
  ClearLCD();
  while(!ExitInfo)
  {
	ButtonPress = ChekButtons();
    delay(BUTTON_PRESS_DELAY);
    if(Page == DELAY_AMOUNT)
    {
      ReadMemory(NUM_REG_ADDR, 1, &numReg);
	  ReadMemory(EepromTab[Page].eeprom_par_addr, numReg, &EepromTab[Page].eeprom_par_value);

    }
	else
	{
	  ReadMemory(EepromTab[Page].eeprom_par_addr, EepromTab[Page].eeprom_par_numReg, &EepromTab[Page].eeprom_par_value);	
	}
   
    // Pulire LCD
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
			if(Band_1.minute < 10)
			{
				TimeStr = String(Band_1.hour) + ":" + "0" + String(Band_1.minute);
			}
			else
			{
				TimeStr = String(Band_1.hour) + ":" + String(Band_1.minute);	
			}
			DateStr = String(Band_1.day) + "/" + String(Band_1.month);
			LCDPrintString(0, CENTER_ALIGN, "Band 1 Time Set:");
			LCDPrintString(1, LEFT_ALIGN, TimeStr);
			LCDPrintString(1, RIGHT_ALIGN, DateStr);
			if(Band_2.minute < 10)
			{
				TimeStr = String(Band_2.hour) + ":" + "0" + String(Band_2.minute);
			}
			else
			{
				TimeStr = String(Band_2.hour) + ":" + String(Band_2.minute);	
			}
			TimeStr = String(Band_2.hour) + ":" + String(Band_2.minute);
			DateStr = String(Band_2.day) + "/" + String(Band_2.month);
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
			if(Page >= HOUR_BAND_1 && Page <= MONTH_BAND_2)
			{
				Page = MONTH_BAND_2;
			}
			ClearLCD();
			break;
		case DOWN:
			BlinkLed(YELLOW_LED); // blink giallo
			Page--;
			if(Page < MIN_INFO_PAGES) 
			{
				Page = MAX_EEPROM_ITEM - 1;
			}
			if(Page >= HOUR_BAND_1 && Page <= MONTH_BAND_2)
			{
				Page = HOUR_BAND_1;
			}
			ClearLCD();
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
  ClearLCD();
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
		LCDPrintString(1, CENTER_ALIGN, "All bands are still");
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
				ChangeDateTime(BAND_1);
				ClearLCD();
				StateChangeBand = CHANGE_BAND_2;
				ExitChangeBand = false;
				break;
				
			case CHANGE_BAND_2:
				LCDPrintString(0, CENTER_ALIGN, "Change the");
				LCDPrintString(1, CENTER_ALIGN, "second band");	
				LCDPrintString(2, CENTER_ALIGN, "Must be greater");
				LCDPrintString(3, CENTER_ALIGN, "then the first!");
				ChangeDateTime(BAND_2);			
				ClearLCD();
				StateChangeBand = CHEK_BAND;
				ExitChangeBand = false;
				break;
			
			case CHEK_BAND:
				if(((Band_1.hour > Band_2.hour) || 
					(Band_1.day  > Band_2.day ) || 
					(Band_1.month > Band_2.month)) &&
					((Band_1.hour != BAND_INVALID_VALUE) &&
					(Band_1.day  != BAND_INVALID_VALUE) &&
					(Band_1.month != BAND_INVALID_VALUE)))					
				{
					LCDPrintString(0, CENTER_ALIGN, "ERROR!");
					LCDPrintString(1, CENTER_ALIGN, "Band 1 > Band 2");
					LCDPrintString(2, CENTER_ALIGN, "Repeat the");
					LCDPrintString(3, CENTER_ALIGN, "procedure");
					delay(2000);
					ClearLCD();
					StateChangeBand = CHANGE_BAND_1;
				}
				else if((Band_1.hour == BAND_INVALID_VALUE) ||
					(Band_1.day  == BAND_INVALID_VALUE) ||
					(Band_1.month == BAND_INVALID_VALUE))
					{
						LCDPrintString(1, CENTER_ALIGN, "All bands are");
						LCDPrintString(2, CENTER_ALIGN, "INVALIDATED");
						SetBandInvalid();
						delay(2000);
						StateChangeBand = EXIT_CHANGE_BAND;
					}
				else if((Band_2.hour == BAND_INVALID_VALUE) ||
					(Band_2.day  == BAND_INVALID_VALUE) ||
					(Band_2.month == BAND_INVALID_VALUE))
					{
						LCDPrintString(1, CENTER_ALIGN, "All bands are");
						LCDPrintString(2, CENTER_ALIGN, "INVALIDATED");
						SetBandInvalid();
						delay(2000);
						StateChangeBand = EXIT_CHANGE_BAND;							
					}
				else
				{
					StateChangeBand	= EXIT_CHANGE_BAND;
					Flags.AllBandsInvalid = false;
				}
				ExitChangeBand = false;
				break;
				
			case EXIT_CHANGE_BAND:
				
				SaveBandToEeprom();	
				for(short i = 0; i < 5; i++)
				{
					BlinkLed(RED_LED);
				}
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
