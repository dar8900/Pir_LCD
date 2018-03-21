#include "EEPROM_Ard.h"

#include <Wire.h>  // Libreria di sistema - E' richiesta da I2CIO.cpp
#include <LiquidCrystal_I2C.h> // Libreria LCD I2C

//LiquidCrystal_I2C lcd_main(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
LiquidCrystal_I2C lcd_main(0x27, 20,4);

#define MAX_LIGHT_DELAY   600 // In secondi
#define MIN_LIGHT_DELAY    20 // In secondi

#define MIN_MENU_PAGES   0
#define MAX_MENU_PAGES  sizeof(MainSetupItems)/sizeof(CREATE_MENU)


#define MIN_INFO_PAGES  0
#define MAX_INFO_PAGES  sizeof(EepromTab)/sizeof(EEPROM_ITEM)

#define MAX_LCD_ROW    3
#define MAX_LCD_COL   19

#define CENTER_ALIGN  10
#define RIGHT_ALIGN    19
#define LEFT_ALIGN     0

#define  OFF(pin)  digitalWrite(pin, LOW)
#define  ON(pin)   digitalWrite(pin, HIGH)

#define AnalogPirPin  0

#define AUTOSCROLL_TIMER  100

#define BAND_INVALID_VALUE 	99

typedef enum
{
  CHANGE_VALUE = 0,   // menu dove cambi solo un valore numerico
  SWITCH_STATE,   // menu dove viene gestito solo lo switch on off per uscite digitali
  INFO,        // menu a scorrimento automatico con tutte le info salvate in eeprom
  TIME_BAND_NUM
} MENU_TYPE_NBR;


typedef struct
{
  String nameMenu;
  MENU_TYPE_NBR typeMenu;
  bool (*MenuFunc)(void);
} CREATE_MENU;


typedef struct
{
  int eeprom_par_value;
  int eeprom_par_addr;
  int eeprom_par_numReg;
  String  eeprom_par_name;
  MENU_TYPE_NBR typeMenu;
} EEPROM_ITEM;

typedef enum
{
  DELAY_AMOUNT = 0,
  PIR_STATE
  
}EEPROM_ITEM_ENUM;

enum
{
  ON = 0,
  OFF
};

typedef struct
{
	byte hour;
	byte minute;
} TIME_FOMAT;

typedef struct
{
	byte day;
	byte month;
	byte year;
} DATE_FOMAT;

typedef struct
{
	TIME_FOMAT BandTime;
	DATE_FOMAT BandDate;
} TIME_BAND;

//int numReg;

//int DelayAmount = MIN_LIGHT_DELAY;
//int SwitchPIR = 0;
int FlagBacklight = 0;
int SetupOk = 0;
int FlagSetup = 0;
int FlagShowInfo = 0;

TIME_FOMAT PresentTime;  // Variabili per l'orario
DATE_FOMAT PresentDate;  // Si perdono allo spegnimento ma vengono aggiornate subito all'accensione
						 //	
TIME_BAND Band_1, Band_2;//


enum
{
  BUTTON_UP = 2,
  BUTTON_DOWN,
  BUTTON_SETUP,
  RED_LED,
  GREEN_LED,
  BLUE_LED,
  YELLOW_LED,
  PIR_SWITCH,
  LIGHT_SWITCH
};

bool ChangeValue();
bool SwichState();
bool InfoScroll();
bool ChangeTimeBands();

CREATE_MENU MainSetupItems[] = 
{
  {"Change light delay", CHANGE_VALUE , ChangeValue		},
  {"Change PIR state"  , SWITCH_STATE , SwichState 		},
  {"Show Info"         , INFO         , InfoScroll      },
  {"Change Time Bands" , TIME_BAND_NUM, ChangeTimeBands },
};

EEPROM_ITEM EepromTab[] = 
{
  {MIN_LIGHT_DELAY,  START_DELAY_ADDR, 1,  "Light delay", CHANGE_VALUE},
  {OFF     ,  START_SWITCH_PIR_ADDR  , 1,  "PIR state"  , SWITCH_STATE},
  {0       ,  0                      , 1,  "Time and Date", TIME_BAND_NUM},
};

const String OnOff[2] = {"On", "Off"};

const byte TabDays4Month[] = 
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


void ClearLCD()
{
    //delay(10);
    lcd_main.clear();
    //lcd_main.home();
}

void LcdTimeWrite(int Time2Write)
{
  int cnt = Time2Write;
  //String numero = String(cnt, DEC);
  while(cnt != 0)
  {
    LCDPrintValue(1, CENTER_ALIGN, cnt);
    delay(1000);
    ClearLCD();
    cnt--;
  }  

  LCDPrintString(1, CENTER_ALIGN, "The light is going");
  LCDPrintString(2, CENTER_ALIGN, "to turn off");
  delay(3000);
  ClearLCD();
}

// Utilizzano un oggetto di tipo LCD
void LCDPrintString(int row, int col, String string) 
{
  if(col > MAX_LCD_COL || row > MAX_LCD_ROW || string.length() > 20) // ???
  {
    lcd_main.clear();
    lcd_main.home();
    lcd_main.print("OVER DIMENSION");
    delay(2000);
    return;
  }
  switch(col)
  {
    case CENTER_ALIGN:
      col = ((MAX_LCD_COL+1) - string.length()) / 2;
      break;
    case RIGHT_ALIGN:
      col = MAX_LCD_COL - string.length();
      break;
    case LEFT_ALIGN:
      col = 0;
      break;
    default:
      lcd_main.home();
      break;
  }
  lcd_main.setCursor(col, row);
  lcd_main.print(string);

}

void LCDPrintValue(int row, int col, int value)
{
  String ValStr = String(value);
  if(col > MAX_LCD_COL || row > MAX_LCD_ROW || ValStr.length() > 20)
  {
    lcd_main.clear();
    lcd_main.home();
    lcd_main.print("OVER DIMENSION");
    delay(2000);
    return;
  }
  
  switch(col)
  {
    case CENTER_ALIGN:
      col = ((MAX_LCD_COL+1) - ValStr.length()) / 2;
      break;
    case RIGHT_ALIGN:
      col = MAX_LCD_COL - ValStr.length();
      break;
    case LEFT_ALIGN:
      col = 0;
      break;
    default:
      lcd_main.home();
      break;
  }
  lcd_main.setCursor(col, row);
  lcd_main.print(ValStr);
}

void LCDPrintLineVoid(int row)
{
  lcd_main.setCursor(0, row);
  lcd_main.print("                    ");
}



/************************ SETUP "RUNTIME" ******************************/

void MainSetup()
{
  int buttonUp = LOW, buttonDown = LOW;
  int OkButton = LOW; //  Resetto ExitButton
  bool ExitSetup = false;
  int Page = MIN_MENU_PAGES;
  
  OFF(RED_LED);
  OFF(GREEN_LED);
  ON(BLUE_LED);
  lcd_main.backlight();
  
  FlagBacklight = true;

  // Pulire LCD
  ClearLCD();
  LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
  LCDPrintString(1,CENTER_ALIGN,"to scroll the menu");
  LCDPrintString(3,CENTER_ALIGN,"Press Ok to enter");
  
  delay(2000);
  
  while(1)
  {
    buttonUp = digitalRead(BUTTON_UP);
    buttonDown = digitalRead(BUTTON_DOWN);
    OkButton = digitalRead(BUTTON_SETUP);
    delay(80);
    // Pulire LCD
    ClearLCD();
    LCDPrintString(1, CENTER_ALIGN, MainSetupItems[Page].nameMenu);
    
    if(buttonUp == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      Page++;
      if(Page >= MAX_MENU_PAGES) 
      {
        Page = MIN_MENU_PAGES;
      }
    }
    if(buttonDown == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      Page--;
      if(Page < MIN_MENU_PAGES)
      {
        Page = MAX_MENU_PAGES-1;
      }
    }
    if(OkButton == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      ExitSetup = MainSetupItems[Page].MenuFunc();      
    }
    
    if(ExitSetup)
    {
      OFF(BLUE_LED);
      break;
    }
  }
}


bool ChangeDateTime(TIME_BAND  Band)
{
  int buttonUp = LOW, buttonDown = LOW, OkTime = LOW;
  bool OkButton = false;
  TIME_FOMAT ChangedTime;
  DATE_FOMAT ChangedDate;
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
		LCDPrintString(1, CENTER_ALIGN, "Saved!");
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
		LCDPrintString(1, CENTER_ALIGN, "Saved!");
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
		LCDPrintString(1, CENTER_ALIGN, "Saved!");
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
		LCDPrintString(1, CENTER_ALIGN, "Saved!");
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

bool ChangeTimeBands()
{
	ClearLCD();
	lcd_main.backlight();
	FlagBacklight = 1;
	bool ExitChangeBand = false;
	while(!ExitChangeBand)
	{
		LCDPrintString(1, CENTER_ALIGN, "Change the");
		LCDPrintString(2, CENTER_ALIGN, "first band");		
		ExitChangeBand = ChangeDateTime(Band_1);
	}
	ExitChangeBand = false;
	while(!ExitChangeBand)
	{
		LCDPrintString(1, CENTER_ALIGN, "Change the");
		LCDPrintString(2, CENTER_ALIGN, "second band");		
		ExitChangeBand = ChangeDateTime(Band_2);
	}	
	return ExitChangeBand;
}


bool SwichState()
{
  
  int buttonUp = 0, buttonDown = 0;
  bool OkButton = false;
  ReadMemory(EepromTab[PIR_STATE].eeprom_par_value, EepromTab[PIR_STATE].eeprom_par_numReg, &EepromTab[PIR_STATE].eeprom_par_value);
  int OldSwitch = EepromTab[PIR_STATE].eeprom_par_value;
  int SwitchOnOff = EepromTab[PIR_STATE].eeprom_par_value;

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
        SwitchOnOff = 0;
    }
    if(buttonDown == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      SwitchOnOff--;
      if(SwitchOnOff < 0)
        SwitchOnOff = 1;
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




bool ChangeValue()
{
  int buttonUp = 0, buttonDown = 0;
  bool OkButton = false;
  //ReadMemory(NUM_REG_ADDR, 1, &EepromTab[DELAY_AMOUNT].eeprom_par_numReg);
  ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, EepromTab[DELAY_AMOUNT].eeprom_par_numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
  Serial.println(EepromTab[DELAY_AMOUNT].eeprom_par_numReg);
  int oldDelayAmount = EepromTab[DELAY_AMOUNT].eeprom_par_value;
  int ChangeDelayAmount = EepromTab[DELAY_AMOUNT].eeprom_par_value;

  // Pulire LCD
  ClearLCD();
  LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
  LCDPrintString(1,CENTER_ALIGN,"to change the value");
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
        EepromTab[DELAY_AMOUNT].eeprom_par_numReg = WriteMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, ChangeDelayAmount);
        WriteMemory(NUM_REG_ADDR, EepromTab[DELAY_AMOUNT].eeprom_par_numReg);
        Serial.println(EepromTab[DELAY_AMOUNT].eeprom_par_numReg);         
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

bool InfoScroll()
{
  int buttonUp = LOW, buttonDown = LOW;
  int ExitButton = LOW; //  Resetto ExitButton
  bool ExitInfo = false;
  int AutoScrollTimer = 200; // DA CONTROLLARE #define AUTOSCROLL_TIMER  100
  int Page = MIN_INFO_PAGES;
  String tmpEepromValue;
  String TimeStr, DateStr;
  
  OFF(RED_LED);
  ON(GREEN_LED);
  ON(BLUE_LED);
  lcd_main.backlight();
  
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
      ReadMemory(NUM_REG_ADDR, 1, &EepromTab[Page].eeprom_par_numReg);
      Serial.println(EepromTab[Page].eeprom_par_numReg);
    }
    ReadMemory(EepromTab[Page].eeprom_par_addr, EepromTab[Page].eeprom_par_numReg, &EepromTab[Page].eeprom_par_value);
    
//    if(AutoScrollTimer == 0)
//    {
//      delay(1000);
//      AutoScrollTimer = AUTOSCROLL_TIMER;
//      Page++;
//      Serial.println(MAX_INFO_PAGES);
//      if(Page > MAX_INFO_PAGES) 
//      {
//        Page = MIN_INFO_PAGES;
//      }
//    }
    
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
	else if(EepromTab[Page].typeMenu == TIME_BAND_NUM)
	{
		TimeStr = String(PresentTime.hour) + ":" + String(PresentTime.minute);
		DateStr = String(PresentDate.day) + "/" + String(PresentDate.month) + "/" + String(PresentDate.year);
		LCDPrintString(1, CENTER_ALIGN, TimeStr);
		LCDPrintString(2, CENTER_ALIGN, DateStr);
	}
    else
    {
      tmpEepromValue = String(EepromTab[Page].eeprom_par_value);
      tmpEepromValue = String(tmpEepromValue + "sec");
      LCDPrintString(2, CENTER_ALIGN, tmpEepromValue);
    }
     
    if(buttonUp == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      AutoScrollTimer = AUTOSCROLL_TIMER;
      Page++;
      if(Page >= MAX_INFO_PAGES) 
      {
        Page = MIN_INFO_PAGES;
      }
    }
    if(buttonDown == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      AutoScrollTimer = AUTOSCROLL_TIMER;
      Page--;
      if(Page < MIN_INFO_PAGES) 
      {
        Page = MAX_INFO_PAGES-1;
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
    AutoScrollTimer--;
  }
  return ExitInfo;
}
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

void InterruptFunc()
{
  int timer = 6; // 0.1s c.a (105ms)
  while(timer > 0)
  {
    timer--;
    if(EnterSetupButton())
      break;
    delay(15);
  }
}


void WriteHomeMsg()
{
  if(FlagBacklight && !FlagShowInfo)
  {
    LCDPrintString(1,CENTER_ALIGN,"Press on Enter/Ok");
    LCDPrintString(2,CENTER_ALIGN,"for enter in");
    LCDPrintString(3,CENTER_ALIGN,"Main Setup");
    delay(2000); 
    ClearLCD(); 
    LCDPrintString(1,CENTER_ALIGN,"After the display");
    LCDPrintString(2,CENTER_ALIGN,"is turned off"); 
    delay(2000);
    ClearLCD(); 
    LCDPrintString(1,CENTER_ALIGN,"Press Up");
    LCDPrintString(2,CENTER_ALIGN,"for Help info"); 
    delay(2000);
    EnterSetupButton();           
    FlagBacklight = false;
    lcd_main.noBacklight(); 
    ClearLCD();
  }
  else if(FlagBacklight && FlagShowInfo)
  {
	FlagShowInfo = 0;
	EnterSetupButton();           
    FlagBacklight = false;
    lcd_main.noBacklight(); 
    ClearLCD();
  }
  else
  {
    InterruptFunc();    
  }

}

void gestionePIR(int analogPin)
{
  int val = 0;
  val = analogRead(analogPin);    
  val = (val *5)/1024;
  ClearLCD();
  if(val > 0)
  {
    lcd_main.backlight();
    FlagBacklight = true;
    ON(GREEN_LED); 
    OFF(RED_LED);
    ReadMemory(NUM_REG_ADDR , 1, &EepromTab[DELAY_AMOUNT].eeprom_par_numReg);
    ReadMemory(START_DELAY_ADDR, EepromTab[DELAY_AMOUNT].eeprom_par_numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
	ON(LIGHT_SWITCH);
    LcdTimeWrite(EepromTab[DELAY_AMOUNT].eeprom_par_value);
	OFF(LIGHT_SWITCH);
  }
  else
  {
    ON(RED_LED);
    OFF(GREEN_LED);
	OFF(LIGHT_SWITCH);
  } 
}

void ShowInfoMsg()
{
	int InfoPressed = LOW;
	int timer = 6; // 0.06s c.a (60ms)
	String Time, Date;
    while(timer > 0)
    {
		timer--;
		InfoPressed = digitalRead(BUTTON_UP);
		if(InfoPressed == HIGH)
		{
			InfoPressed = LOW;
			FlagShowInfo = 1;
			break;
		}	
		FlagShowInfo = 0;		
		delay(10);
	}	
	if(FlagShowInfo)
	{
		lcd_main.backlight();
		FlagBacklight = 1;
		LCDPrintString(1, CENTER_ALIGN, "Press Setup/Ok");
		LCDPrintString(2, CENTER_ALIGN, "for the menu");
		delay(1000);
		ClearLCD();
		LCDPrintString(0, CENTER_ALIGN, "Change the bands");
		LCDPrintString(1, CENTER_ALIGN, "to set when");
		LCDPrintString(2, CENTER_ALIGN, "turn ON");
		LCDPrintString(2, CENTER_ALIGN, "the sensor");
		delay(3000);
		ClearLCD();
	}
	
	
}




// Blink del led per 5ms
void BlinkLed(int pin) 
{
  digitalWrite(pin, HIGH);
  delay(5);
  digitalWrite(pin, LOW);
}

void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_DOWN, INPUT);
  pinMode(BUTTON_SETUP, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(PIR_SWITCH, OUTPUT);
  pinMode(LIGHT_SWITCH, OUTPUT);
  
  lcd_main.begin(20,4);  
  delay(1000);
  lcd_main.noBlink(); 

  /*
	ANDREBBE SETTATO IL TEMPO E LA DATA CORRENTI NELLE VARIABILI GLOBALI
	IN MODO CHE NON SI POSSA SUPERARE IL NUMERO DI GIORNI PER IL MESE CORRENTE
	QUANDO USO LA FUNZIONE PER IL CAMBIAMENTO DI DATA
	DateTime now = RTC.now();
	PresentDate.day = now.day();
	PresentDate.month = now.month();
	PresentDate.year = now.year();
	PresentTime.hour = now.hour();
	PresentTime.minute = now.minute();
  */
  
  //ReadMemory(NUM_REG_ADDR, 1, &EepromTab[DELAY_AMOUNT].eeprom_par_numReg); // Inizializzo il numero registri per il delay
  ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, EepromTab[DELAY_AMOUNT].eeprom_par_numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
  ReadMemory(EepromTab[PIR_STATE].eeprom_par_addr, EepromTab[PIR_STATE].eeprom_par_numReg, &EepromTab[PIR_STATE].eeprom_par_value);
  
  Band_1.BandTime.hour = BAND_INVALID_VALUE;
  Band_1.BandTime.minute = BAND_INVALID_VALUE;
  Band_1.BandTime.day = BAND_INVALID_VALUE;
  Band_1.BandTime.month = BAND_INVALID_VALUE;
  Band_2.BandTime.hour = BAND_INVALID_VALUE;
  Band_2.BandTime.minute = BAND_INVALID_VALUE;
  Band_2.BandTime.day = BAND_INVALID_VALUE;
  Band_2.BandTime.month = BAND_INVALID_VALUE;
  
  ClearLCD();

  InfoScroll();
  
  delay(1000);
  
  ClearLCD(); 
}

void loop()
{
  // now = RTC.now();
  // PresentDate.day = now.day();
  // PresentDate.month = now.month();
  // PresentDate.year = now.year();
  // PresentTime.hour = now.hour();
  // PresentTime.minute = now.minute();
	
  WriteHomeMsg();
  if(FlagSetup)
  {
    MainSetup();
  }
  else
  {
	// INSERIRE BOOLEANO PER CHEK DEL BAND, NEL CASO FOSSE FALSE (OVVERO FUORI FASCIA O INVALIDO) 
	// SPEGNERE IL SENSORE, ALTRIMENTI LASCIARE LA GESTIONE CHE C'è ORA
	
	if(EepromTab[PIR_STATE].eeprom_par_value == ON)
    {
     	digitalWrite(PIR_SWITCH, HIGH);
		delay(150);
		gestionePIR(AnalogPirPin);
    }
    else
    {
		digitalWrite(PIR_SWITCH, LOW);
    }  
  }
}


