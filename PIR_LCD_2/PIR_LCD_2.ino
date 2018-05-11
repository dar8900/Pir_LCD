#include <RTClib.h>
#include <Wire.h>  // Libreria di sistema - E' richiesta da I2CIO.cpp


#include "PIR_LCD_2.h"
#include "EEPROM_Ard.h"
#include "Band_Func.h"
#include "MenuFunc.h"
#include "LCDLib.h"

//LiquidCrystal_I2C lcd_main(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Old version


#ifdef RTC_INSERTED
TIME_DATE_FORMAT PresentTime;  // Variabili per l'orario
// DATE_FORMAT PresentDate;  // Si perdono allo spegnimento ma vengono aggiornate subito all'accensione
						 //	
TIME_DATE_FORMAT Band_1, Band_2;//

RTC_DS1307 RTC;
DateTime now;
#endif

FLAGS Flags;


short buttonUp = LOW, buttonDown = LOW, OkButton = LOW;
short MainMenuPage;

CREATE_MENU MainSetupItems[] = 
{
  {"Change light delay", ChangeValue		},
  {"Change PIR state"  , SwichState 		},
  {"Show Info"         , InfoScroll         },
  {"Manual State"	   , SwichState 		},
#ifdef RTC_INSERTED
  {"Change Time Bands" , ChangeTimeBands    },
#endif
};

EEPROM_ITEM EepromTab[] = 
{
  {MIN_LIGHT_DELAY			,  START_DELAY_ADDR       , 1,  "Light delay"   , CHANGE_VALUE},
  {TURN_OFF					,  SWITCH_PIR_ADDR        , 1,  "PIR state"     , SWITCH_STATE},
  {TURN_OFF					,  MANUAL_STATE_ADDR      , 1,  "Manual state"  , SWITCH_STATE},
#ifdef RTC_INSERTED
  {BAND_INVALID_VALUE       ,  HOUR_BAND_1_ADDR       , 1,  ""   , TIME_BAND_NUM},
  {BAND_INVALID_VALUE    	,  HOUR_BAND_2_ADDR       , 1,  ""   , TIME_BAND_NUM},
  {BAND_INVALID_VALUE       ,  MINUTE_BAND_1_ADDR     , 1,  ""   , TIME_BAND_NUM},
  {BAND_INVALID_VALUE    	,  MINUTE_BAND_2_ADDR     , 1,  ""   , TIME_BAND_NUM},
  {BAND_INVALID_VALUE    	,  DAY_BAND_1_ADDR        , 1,  ""    , TIME_BAND_NUM},
  {BAND_INVALID_VALUE    	,  DAY_BAND_2_ADDR        , 1,  ""    , TIME_BAND_NUM},
  {BAND_INVALID_VALUE    	,  MONTH_BAND_1_ADDR      , 1,  ""   , TIME_BAND_NUM},
  {BAND_INVALID_VALUE    	,  MONTH_BAND_2_ADDR      , 1,  ""   , TIME_BAND_NUM},
#endif
};



short ChekButtons()
{
	short ButtonPress = NO_BUTTON_PRESS;
	buttonUp = LOW;
	buttonDown = LOW;
	OkButton = LOW;
	buttonUp = READ(BUTTON_UP);
    buttonDown = READ(BUTTON_DOWN);
    OkButton = READ(BUTTON_SETUP);
	if(buttonUp == HIGH)
		ButtonPress = UP;
	if(buttonDown == HIGH)
		ButtonPress = DOWN;
	if(OkButton == HIGH)
		ButtonPress = OK_EXIT;
	return ButtonPress;
}
/************************ SETUP "RUNTIME" ******************************/

void MainSetup()
{
  bool ExitSetup = false;
  short Page = MIN_MENU_PAGES;
  short ButtonPress;
  
  OFF(RED_LED);
  OFF(GREEN_LED);
  ON(BLUE_LED);
  LCDDisplayOn();
  
  Flags.Backlight = true;

  // Pulire LCD
  ClearLCD();
  LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
  LCDPrintString(1,CENTER_ALIGN,"to scroll the menu");
  LCDPrintString(3,CENTER_ALIGN,"Press Ok to enter");
  
  delay(2000);
  ClearLCD();
  while(!ExitSetup)
  {
	ButtonPress = ChekButtons();
    delay(BUTTON_PRESS_DELAY);
    // Pulire LCD
   
    LCDPrintString(1, CENTER_ALIGN, MainSetupItems[Page].nameMenu);
    
	switch(ButtonPress)
	{
		case UP:
		    BlinkLed(YELLOW_LED); // blink giallo
			Page++;
			if(Page >= MAX_MENU_PAGES) 
			{
				Page = MIN_MENU_PAGES;
			}
			ClearLCD();
			break;
		case DOWN:
		    BlinkLed(YELLOW_LED); // blink giallo
			Page--;
			if(Page < MIN_MENU_PAGES)
			{
				Page = MAX_MENU_PAGES-1;
			}
			ClearLCD();
			break;
		case OK_EXIT:
		    BlinkLed(YELLOW_LED); // blink giallo
			MainMenuPage = Page;
			ExitSetup = MainSetupItems[Page].MenuFunc(); 
			ClearLCD();			
			break;
		default:
			break;	
	}
  }
  OFF(BLUE_LED);
}

void InterruptFunc()
{
  short timer = 6; // 0.1s c.a (105ms)
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
  if(Flags.Backlight && !Flags.ShowInfo)
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
    Flags.Setup = false;           
    Flags.Backlight = false;
    LCDDisplayOff(); 
    ClearLCD();
  }
  else if(Flags.Backlight && Flags.ShowInfo)
  {
	Flags.ShowInfo = 0;
	Flags.Setup = false;           
    Flags.Backlight = false;
    LCDDisplayOff(); 
    ClearLCD();
  }
  else
  {
    InterruptFunc();    
  }

}

void gestionePIR(short StatePIR)
{
  short val = 0;
  short numReg;
  if(StatePIR == TURN_ON)
  {
	  val = READ(PIR_INPUT);    
	  ClearLCD();
	  if(val > 0)
	  {
		LCDDisplayOn();
		Flags.Backlight = true;
		ON(GREEN_LED); 
		OFF(RED_LED);
		ReadMemory(NUM_REG_ADDR , 1, (short*)&numReg);
		ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
		ON(LIGHT_SWITCH);
		LcdTimeWrite(EepromTab[DELAY_AMOUNT].eeprom_par_value);
		OFF(LIGHT_SWITCH);
		OFF(GREEN_LED);
	  }
      else
	  {
		ON(RED_LED);
		OFF(GREEN_LED);
		OFF(LIGHT_SWITCH);
	  } 
  }
  else
  {
	  OFF(GREEN_LED);
	  OFF(RED_LED);
  }
  
}

void ShowInfoMsg()
{
	short InfoPressed = LOW;
	short timer = 6; // 0.06s c.a (60ms)
	String Time, Date;
#ifdef RTC_INSERTED
	TakePresentTime();
	if(PresentTime.minute < 10)
	{
		Time = String(PresentTime.hour) + ":" + "0" + String(PresentTime.minute);
	}
	else
	{
		Time = String(PresentTime.hour) + ":" + String(PresentTime.minute);	
	}
	Time = String(PresentTime.hour) + ":" + String(PresentTime.minute);
	Date = String(PresentTime.day) + "/" + String(PresentTime.month);
#endif
    while(timer > 0)
    {
		timer--;
		InfoPressed = READ(BUTTON_UP);
		if(InfoPressed == HIGH)
		{
			InfoPressed = LOW;
			Flags.ShowInfo = 1;
			break;
		}	
		Flags.ShowInfo = 0;		
		delay(10);
	}	
	if(Flags.ShowInfo)
	{
		LCDDisplayOn();
		Flags.Backlight = true;
		LCDPrintString(1, CENTER_ALIGN, "Press Setup/Ok");
		LCDPrintString(2, CENTER_ALIGN, "for the menu");
		delay(2000);
		ClearLCD();
#ifdef RTC_INSERTED
		LCDPrintString(0, CENTER_ALIGN, "Change the bands");
		LCDPrintString(1, CENTER_ALIGN, "to set when");
		LCDPrintString(2, CENTER_ALIGN, "turn ON");
		LCDPrintString(3, CENTER_ALIGN, "the sensor");
		delay(3000);
		ClearLCD();
		LCDPrintString(0, CENTER_ALIGN, "Change to");
		LCDPrintString(1, CENTER_ALIGN, "manual mode");
		LCDPrintString(2, CENTER_ALIGN, "to turn ON or OFF");
		LCDPrintString(3, CENTER_ALIGN, "the sensor");
		delay(3000);
		ClearLCD();
	    LCDPrintString(1, CENTER_ALIGN, Time);
		LCDPrintString(2, CENTER_ALIGN, Date);
		delay(3000);
		ClearLCD();
#endif
	}
	
	
}


// Blink del led per 5ms
void BlinkLed(short pin) 
{
  digitalWrite(pin, HIGH);
  delay(5);
  digitalWrite(pin, LOW);
}

void TakePresentTime()
{
  now = RTC.now();
  PresentTime.day = now.day();
  PresentTime.month = now.month();
  PresentTime.year = now.year();
  PresentTime.hour = now.hour();
  PresentTime.minute = now.minute();
}

static void InitMemory()
{
	short numReg = 0;
	short FirstStartCheck = 0;
	
	ReadMemory(FIRST_START_CHECK_ADDR, 1, &FirstStartCheck);
	if(FirstStartCheck == 255)
	{
	  WriteMemory(FIRST_START_CHECK_ADDR, 1);
	  FirstStartCheck = 1;
	  LCDPrintString(0,CENTER_ALIGN, "Default values");
	  LCDPrintString(1, CENTER_ALIGN, "restored");
	  delay(2000);
	}
	else
	{
	  FirstStartCheck = 0;
	}

	// Leggi dalla memoria perchè non è il primo avvio
	if(FirstStartCheck == 0)
	{
		ReadMemory(NUM_REG_ADDR, 1, &numReg); // Inizializzo il numero registri per il delay
		ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
		ReadMemory(EepromTab[PIR_STATE].eeprom_par_addr, EepromTab[PIR_STATE].eeprom_par_numReg, &EepromTab[PIR_STATE].eeprom_par_value);	  
	#ifdef RTC_INSERTED		
		ReadBandFromEeprom();
	#endif
	}
	else // Imposta i valori di default
	{
	#ifdef RTC_INSERTED
		SetBandInvalid();
		SaveBandToEeprom();
	#endif
		WriteMemory(NUM_REG_ADDR, 1);
		WriteMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, EepromTab[DELAY_AMOUNT].eeprom_par_value);
		WriteMemory(EepromTab[PIR_STATE].eeprom_par_addr, EepromTab[PIR_STATE].eeprom_par_value);		
	}
	return;
}

static void RTCInit()
{
	if (!RTC.begin()) 
	{
		while (1)
		{
			BlinkLed(YELLOW_LED);
			BlinkLed(RED_LED);
		}
	}

	if (!RTC.isrunning()) 
	{
		LCDDisplayOn();
		ClearLCD();
		LCDPrintString(0, CENTER_ALIGN, "RTC NOT running!");
		delay(1000);
		// following line sets the RTC to the date & time this sketch was compiled
		RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
		ON(YELLOW_LED);
		ON(BLUE_LED);
		ClearLCD();
		LCDPrintString(0, CENTER_ALIGN, "Setting the time");
		delay(2000);
		OFF(YELLOW_LED);
		OFF(BLUE_LED);	
		LCDDisplayOff();	
	}	
	return;
}


void setup()
{
  
	Flags.Backlight = false;
	Flags.Setup = false;
	Flags.ShowInfo = false;
	Flags.BandOk = false;
	Flags.AllBandsInvalid = false;
	Flags.ManualState = false;

	Serial.begin(9600);

	pinMode(BUTTON_UP, INPUT);
	pinMode(BUTTON_DOWN, INPUT);
	pinMode(BUTTON_SETUP, INPUT);
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	pinMode(LIGHT_SWITCH, OUTPUT);
	pinMode(PIR_INPUT, INPUT);
	
	LCDInit(); 
	delay(1000);
	LCDNoBlink(); 

#ifdef RTC_INSERTED
	RTCInit();
	TakePresentTime();
#endif
  
	InitMemory(); 
	ClearLCD();
	InfoScroll();
	delay(1000);
	ClearLCD(); 
}

void loop()
{
#ifdef RTC_INSERTED
	TakePresentTime();
#endif
	WriteHomeMsg();
	ShowInfoMsg();
	if(Flags.Setup)
	{
		MainSetup();
	}
	else
	{
		if(!Flags.ManualState)
		{
#ifdef RTC_INSERTED
			ChekBandValue();
			if(Flags.BandOk)
			{
				gestionePIR(EepromTab[PIR_STATE].eeprom_par_value);	
			}
			else
			{
				gestionePIR(TURN_OFF);		
			}
#else
			gestionePIR(EepromTab[PIR_STATE].eeprom_par_value);
#endif  	
		}
	}
}


