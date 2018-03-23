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

extern bool FlagBacklight;
extern bool SetupOk;
extern bool FlagSetup;
extern bool FlagShowInfo;
extern bool FlagBandOk;
extern bool FlagAllBandsInvalid;


const String OnOff[2] = {"On", "Off"};

const short TabDays4Month[] = 
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

bool EnterSetupButton()
{
  SetupOk = digitalRead(BUTTON_SETUP);
  if(SetupOk == HIGH)
  {
    BlinkLed(YELLOW_LED);
    SetupOk = LOW;
    FlagSetup = true;
  }
  else
  {
    FlagSetup = false;
  }
  return FlagSetup;
}

bool ChangeValue()
{
  short buttonUp = 0, buttonDown = 0;
  bool OkButton = false;
  short numReg;
  ReadMemory(NUM_REG_ADDR, 1, (short*)&numReg);
  ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
  short oldDelayAmount = EepromTab[DELAY_AMOUNT].eeprom_par_value;
  short ChangeDelayAmount = EepromTab[DELAY_AMOUNT].eeprom_par_value;

  // Pulire LCD
  ClearLCD();
  LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
  LCDPrintString(1,CENTER_ALIGN,"to change the value");
  LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");

  //  Resetto SetupOk
  SetupOk = LOW;

  delay(1000);
  // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
  // premere su setup per dare l'ok
  ClearLCD();
  while(1)
  {
    buttonUp = digitalRead(BUTTON_UP);
    buttonDown = digitalRead(BUTTON_DOWN);
    SetupOk = digitalRead(BUTTON_SETUP);
    delay(60);
    // Pulire LCD
    LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(1,CENTER_ALIGN,"to change the value:");
    LCDPrintLineVoid(2);
    LCDPrintValue(2,8, ChangeDelayAmount);
    LCDPrintString(2,11,"sec");
    LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
    // premere su setup per dare l'ok

    if(buttonUp == HIGH)
    {
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
    }
    if(buttonDown == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      ChangeDelayAmount--;
      if(ChangeDelayAmount < MIN_LIGHT_DELAY) // Raggiunti i secondi minimi per il delay nella gestione pir
      {
        ChangeDelayAmount = MAX_LIGHT_DELAY;
      }
    }
    if(SetupOk == HIGH)
    {
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
    }
  }

  if(OkButton)
  {
    OFF(BLUE_LED);
    ClearLCD();
  }   

  return OkButton;
}


bool SwichState()
{
  
  short buttonUp = 0, buttonDown = 0;
  bool OkButton = false;
  ReadMemory(EepromTab[PIR_STATE].eeprom_par_value, EepromTab[PIR_STATE].eeprom_par_numReg, &EepromTab[PIR_STATE].eeprom_par_value);
  short OldSwitch = EepromTab[PIR_STATE].eeprom_par_value;
  short SwitchOnOff = TURN_OFF;

  // Pulire LCD
  ClearLCD();
  LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
  LCDPrintString(1,CENTER_ALIGN,"to change the state");
  LCDPrintString(2,CENTER_ALIGN,"On or Off");
  LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");

  //  Resetto SetupOk
  SetupOk = LOW;

  delay(2000);
  // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
  // premere su setup per dare l'ok
  ClearLCD();
  while(1)
  {
    buttonUp = digitalRead(BUTTON_UP);
    buttonDown = digitalRead(BUTTON_DOWN);
    SetupOk = digitalRead(BUTTON_SETUP);
    delay(60);
    // Pulire LCD
    LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(1,CENTER_ALIGN,"to change the state");
    LCDPrintLineVoid(2);
    LCDPrintString(2, CENTER_ALIGN, OnOff[SwitchOnOff]);
    LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
    // premere su setup per dare l'ok

    if(buttonUp == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      SwitchOnOff++;
      if(SwitchOnOff > 1)
        SwitchOnOff = TURN_ON;
    }
    if(buttonDown == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      SwitchOnOff--;
      if(SwitchOnOff < 0)
        SwitchOnOff = TURN_OFF;
    }
    if(SetupOk == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      // Scrivere su LCD "Valori salvati"
      ClearLCD();
      LCDPrintString(1, CENTER_ALIGN, "The state is");
      LCDPrintString(2, CENTER_ALIGN, OnOff[SwitchOnOff]);
      
      if(SwitchOnOff != OldSwitch)
      {
        WriteMemory(EepromTab[PIR_STATE].eeprom_par_addr, SwitchOnOff);
      }
      
      OkButton = true;       
      delay(2000);
      break;
    }
  }

  if(OkButton)
  {
    OFF(BLUE_LED);
    ClearLCD();
  }   

  return OkButton;
    
}

#ifdef RTC_INSERTED
bool ChangeDateTime(TIME_BAND  Band)
{
  short buttonUp = LOW, buttonDown = LOW, OkTime = LOW;
  bool OkButton = false;
  TIME_FORMAT ChangedTime;
  DATE_FORMAT ChangedDate;
  ChangedTime = PresentTime;
  ChangedDate = PresentDate;
  ON(BLUE_LED);
  ON(RED_LED);
  // Pulire LCD
  ClearLCD();
  LCDPrintString(1,CENTER_ALIGN,"Change Time/Date");

  delay(2000);
  // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
  // premere su setup per dare l'ok
  ClearLCD();
  
  // CAMBIA ORE
  while(1)
  {
    buttonUp = digitalRead(BUTTON_UP);
    buttonDown = digitalRead(BUTTON_DOWN);
    OkTime = digitalRead(BUTTON_SETUP);
    delay(60);
    // Pulire LCD
    LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(1,CENTER_ALIGN,"to change the hour");
    LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
	LCDPrintValue(3, CENTER_ALIGN, ChangedTime.hour);
    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
    // premere su setup per dare l'ok

    if(buttonUp == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      ChangedTime.hour++;
      if(ChangedTime.hour > 23 && ChangedTime.hour != BAND_INVALID_VALUE)
        ChangedTime.hour = BAND_INVALID_VALUE;
	  if(ChangedTime.hour == BAND_INVALID_VALUE)
		  ChangedTime.hour = 0;
    }
    if(buttonDown == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      ChangedTime.hour--;
      if(ChangedTime.hour < 0 && ChangedTime.hour != BAND_INVALID_VALUE)
        ChangedTime.hour = BAND_INVALID_VALUE;
	  if(ChangedTime.hour == BAND_INVALID_VALUE)
		  ChangedTime.hour = 23;
    }
    if(SetupOk == HIGH)
    {
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
	  buttonUp = LOW;
	  buttonDown = LOW;
	  OkTime = LOW;
	  OkButton = true;
      delay(1000);
      break;
    }
  }
  ClearLCD();
  
  //CAMBIA MINUTI
  while(1)
  {
    buttonUp = digitalRead(BUTTON_UP);
    buttonDown = digitalRead(BUTTON_DOWN);
    OkTime = digitalRead(BUTTON_SETUP);
    delay(60);
    // Pulire LCD
    LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(1,CENTER_ALIGN,"to change the minutes");
    LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
	LCDPrintValue(3, CENTER_ALIGN, ChangedTime.minute);
    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
    // premere su setup per dare l'ok

    if(buttonUp == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      ChangedTime.minute++;
      if(ChangedTime.minute > 59 && ChangedTime.minute != BAND_INVALID_VALUE)
        ChangedTime.minute = BAND_INVALID_VALUE;
	  if(ChangedTime.minute == BAND_INVALID_VALUE)
		ChangedTime.minute = 0;
    }
    if(buttonDown == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      ChangedTime.minute--;
      if(ChangedTime.minute < 0 && ChangedTime.minute != BAND_INVALID_VALUE)
        ChangedTime.minute = BAND_INVALID_VALUE;
	  if(ChangedTime.minute == BAND_INVALID_VALUE)
		ChangedTime.minute = 59;
    }
    if(SetupOk == HIGH)
    {
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
	  buttonUp = LOW;
	  buttonDown = LOW;
	  OkTime = LOW;
	  OkButton = true;
      break;
    }
  }
  ClearLCD();
  
  // CAMBIA GIORNO
  while(1)
  {
    buttonUp = digitalRead(BUTTON_UP);
    buttonDown = digitalRead(BUTTON_DOWN);
    OkTime = digitalRead(BUTTON_SETUP);
    delay(60);
    // Pulire LCD
    LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(1,CENTER_ALIGN,"to change the day");
    LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
	  LCDPrintValue(3, CENTER_ALIGN, ChangedDate.day);
    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
    // premere su setup per dare l'ok

    if(buttonUp == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      ChangedDate.day++;
      if(ChangedDate.day > TabDays4Month[PresentDate.month] && ChangedDate.day != BAND_INVALID_VALUE)
        ChangedDate.day  = BAND_INVALID_VALUE;
	  if(ChangedDate.day == BAND_INVALID_VALUE)
		ChangedDate.day  = 1;
    }
    if(buttonDown == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      ChangedDate.day--;
      if(ChangedDate.day < 1 && ChangedDate.day != BAND_INVALID_VALUE)
		ChangedDate.day  = BAND_INVALID_VALUE;
	  if(ChangedDate.day == BAND_INVALID_VALUE)
        ChangedDate.day = TabDays4Month[PresentDate.month];
    }
    if(SetupOk == HIGH)
    {
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
	  buttonUp = LOW;
	  buttonDown = LOW;
	  OkTime = LOW;
	  OkButton = true;
      break;
    }
  }
  ClearLCD();
  
  // CAMBIA MESE
  while(1)
  {
    buttonUp = digitalRead(BUTTON_UP);
    buttonDown = digitalRead(BUTTON_DOWN);
    OkTime = digitalRead(BUTTON_SETUP);
    delay(60);
    // Pulire LCD
    LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(1,CENTER_ALIGN,"to change the month");
    LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
	LCDPrintValue(3, CENTER_ALIGN, ChangedDate.month);
    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
    // premere su setup per dare l'ok

    if(buttonUp == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      ChangedDate.month++;
      if(ChangedDate.month > 12 && ChangedDate.month != BAND_INVALID_VALUE)
        ChangedDate.month  = BAND_INVALID_VALUE;
	  if(ChangedDate.month == BAND_INVALID_VALUE)
        ChangedDate.month = 1;
    }
    if(buttonDown == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      ChangedDate.month--;
      if(ChangedDate.month < 1 && ChangedDate.month != BAND_INVALID_VALUE)
         ChangedDate.month  = BAND_INVALID_VALUE;
	  if(ChangedDate.month == BAND_INVALID_VALUE)
        ChangedDate.month = 12;
    }
    if(SetupOk == HIGH)
    {
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
	  buttonUp = LOW;
	  buttonDown = LOW;
	  OkTime = LOW;
	  OkButton = true;
      break;
    }
  }
  if(OkButton)
  {
    OFF(BLUE_LED);
	OFF(RED_LED);
    ClearLCD();
  }   

  return OkButton;

}
#endif

bool InfoScroll()
{
  short buttonUp = LOW, buttonDown = LOW;
  short ExitButton = LOW; //  Resetto ExitButton
  bool ExitInfo = false;
  short Page = MIN_INFO_PAGES;
  short numReg;
  String tmpEepromValue;
#ifdef RTC_INSERTED
  String TimeStr, DateStr;
#endif 
 
  OFF(RED_LED);
  ON(GREEN_LED);
  ON(BLUE_LED);
  LCDDisplayOn();
  
  FlagBacklight = true;

  // Pulire LCD
  ClearLCD();
  LCDPrintString(1,CENTER_ALIGN,"Press Up, Down");
  LCDPrintString(2,CENTER_ALIGN,"to scroll the info");
  LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
  
  delay(2000);
  
  while(1)
  {
    buttonUp = digitalRead(BUTTON_UP);
    buttonDown = digitalRead(BUTTON_DOWN);
    ExitButton = digitalRead(BUTTON_SETUP);
    delay(100);
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
		TimeStr = String(PresentTime.hour) + ":" + String(PresentTime.minute);
		DateStr = String(PresentDate.day) + "/" + String(PresentDate.month) + "/" + String(PresentDate.year);
		LCDPrintString(1, CENTER_ALIGN, TimeStr);
		LCDPrintString(2, CENTER_ALIGN, DateStr);
	}
#endif
    else
    {
      tmpEepromValue = String(EepromTab[Page].eeprom_par_value);
      tmpEepromValue = String(tmpEepromValue + "sec");
      LCDPrintString(2, CENTER_ALIGN, tmpEepromValue);
    }
     
    if(buttonUp == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      Page++;
      if(Page >= MAX_EEPROM_ITEM) 
      {
        Page = MIN_INFO_PAGES;
      }
    }
    if(buttonDown == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      Page--;
      if(Page < MIN_INFO_PAGES) 
      {
        Page = MAX_EEPROM_ITEM - 1;
      }
    }
    if(ExitButton == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      // Scrivere su LCD Valori salvati
      ClearLCD();
      LCDPrintString(1, CENTER_ALIGN, "Exit...");
      delay(2000);
      ExitInfo = true;
    }
    
    if(ExitInfo)
    {
      OFF(GREEN_LED);
      OFF(BLUE_LED);
      break;
    }
  }
  return ExitInfo;
}

#ifdef RTC_INSERTED
bool ChangeTimeBands()
{
	ClearLCD();
	LCDDisplayOn();
	FlagBacklight = 1;
	bool ExitChangeBand = false;
	if(FlagAllBandsInvalid)
	{
		LCDPrintString(1, CENTER_ALIGN, "All band are");
		LCDPrintString(2, CENTER_ALIGN, "INVALIDATED");
		delay(2000);
		ClearLCD();
	}
	while(!ExitChangeBand)
	{
		LCDPrintString(1, CENTER_ALIGN, "Change the");
		LCDPrintString(2, CENTER_ALIGN, "first band");		
		ExitChangeBand = ChangeDateTime(Band_1);
		ClearLCD();
	}
	ExitChangeBand = false;
	while(!ExitChangeBand)
	{
		LCDPrintString(0, CENTER_ALIGN, "Change the");
		LCDPrintString(1, CENTER_ALIGN, "second band");	
		LCDPrintString(2, CENTER_ALIGN, "Must be greater");
		LCDPrintString(3, CENTER_ALIGN, "then the first!");
		ExitChangeBand = ChangeDateTime(Band_2);
		ClearLCD();
	}	
	// CONTROLLO CHE LA BANDA 2 SIA MAGGIORE DELLA BANDA 1
	if((Band_1.BandTime.hour > Band_2.BandTime.hour) || 
       (Band_1.BandDate.day  > Band_2.BandDate.day ) || 
	   (Band_1.BandDate.month > Band_2.BandDate.month))
    {
		ClearLCD();
		LCDPrintString(0, CENTER_ALIGN, "ERROR!");
		LCDPrintString(1, CENTER_ALIGN, "Band 1 > Band 2");
		LCDPrintString(2, CENTER_ALIGN, "Restore invalid");
		LCDPrintString(3, CENTER_ALIGN, "band values");
		SetBandInvalid();
		delay(2000);
		ClearLCD();
	}
	else
	{
		FlagAllBandsInvalid = false;
		SaveBandToEeprom();		
	}

	return ExitChangeBand;
}
#endif
