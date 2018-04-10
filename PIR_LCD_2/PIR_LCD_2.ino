
#include <Wire.h>  // Libreria di sistema - E' richiesta da I2CIO.cpp
#include <LiquidCrystal_I2C.h> // Libreria LCD I2C
#include "RTC_EEPROMLib.h"

#include "PIR_LCD_2.h"
#include "EEPROM_Ard.h"
#include "Band_Func.h"
#include "MenuFunc.h"

//LiquidCrystal_I2C lcd_main(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Old version
LiquidCrystal_I2C lcd_main(0x27, 20,4);

#ifdef RTC_INSERTED
TIME_DATE_FORMAT PresentTime;  // Variabili per l'orario
// DATE_FORMAT PresentDate;  // Si perdono allo spegnimento ma vengono aggiornate subito all'accensione
						 //	
TIME_DATE_FORMAT Band_1, Band_2;//

// RTC_DS1307 RTC;
// DateTime now;
RTCLib now;
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



void ClearLCD()
{
    //delay(10);
    lcd_main.clear();
    //lcd_main.home();
}

void LcdTimeWrite(int Time2Write)
{
  int cnt = Time2Write;
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
void LCDPrintString(short row, short col, String string) 
{
  if(col > MAX_LCD_COL || row > MAX_LCD_ROW || string.length() > 20) // ???
  {
	lcd_main.clear();
	if(string.length() > 20)
	{
		col = CENTER_ALIGN;
		string = "STRING TOO BIG";  		
	}
	else
	{
		col = CENTER_ALIGN;
		row = 3;
		string = "OVER DIMENSION"; 		
	}
  
  }
  switch(col)
  {
    case CENTER_ALIGN:
      col = ((MAX_LCD_COL+1) - string.length()) / 2;
      break;
    case RIGHT_ALIGN:
      col = (MAX_LCD_COL+1) - string.length();
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

void LCDPrintValue(short row, short col, short value)
{
  String ValStr = String(value);
  if(col > MAX_LCD_COL || row > MAX_LCD_ROW || ValStr.length() > 20)
  {
	lcd_main.clear();
	col = CENTER_ALIGN;
	row = 3;
    ValStr = "OVER DIMENSION"; 
    return;
  }
  
  switch(col)
  {
    case CENTER_ALIGN:
      col = ((MAX_LCD_COL+1) - ValStr.length()) / 2;
      break;
    case RIGHT_ALIGN:
      col = (MAX_LCD_COL+1) - ValStr.length();
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

void LCDPrintLineVoid(short row)
{
  lcd_main.setCursor(0, row);
  lcd_main.print("                    ");
}

void LCDDisplayOn()
{
	lcd_main.backlight();
}

short ChekButtons()
{
	short ButtonPress = NO_BUTTON_PRESS;
	buttonUp = LOW;
	buttonDown = LOW;
	OkButton = LOW;
	buttonUp = digitalRead(BUTTON_UP);
    buttonDown = digitalRead(BUTTON_DOWN);
    OkButton = digitalRead(BUTTON_SETUP);
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
  lcd_main.backlight();
  
  Flags.Backlight = true;

  // Pulire LCD
  ClearLCD();
  LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
  LCDPrintString(1,CENTER_ALIGN,"to scroll the menu");
  LCDPrintString(3,CENTER_ALIGN,"Press Ok to enter");
  
  delay(2000);
  
  while(!ExitSetup)
  {
	ButtonPress = ChekButtons();
    delay(BUTTON_PRESS_DELAY);
    // Pulire LCD
    ClearLCD();
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
			break;
		case DOWN:
		    BlinkLed(YELLOW_LED); // blink giallo
			Page--;
			if(Page < MIN_MENU_PAGES)
			{
				Page = MAX_MENU_PAGES-1;
			}
			break;
		case OK_EXIT:
		    BlinkLed(YELLOW_LED); // blink giallo
			MainMenuPage = Page;
			ExitSetup = MainSetupItems[Page].MenuFunc();      
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
    lcd_main.noBacklight(); 
    ClearLCD();
  }
  else if(Flags.Backlight && Flags.ShowInfo)
  {
	Flags.ShowInfo = 0;
	Flags.Setup = false;           
    Flags.Backlight = false;
    lcd_main.noBacklight(); 
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
	  val = digitalRead(PIR_PIN);    
	  ClearLCD();
	  if(val == HIGH)
	  {
		lcd_main.backlight();
		Flags.Backlight = true;
		ON(GREEN_LED); 
		OFF(RED_LED);
		ReadMemory(NUM_REG_ADDR , 1, &numReg);
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
		InfoPressed = digitalRead(BUTTON_UP);
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
		lcd_main.backlight();
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
		LCDPrintString(2, CENTER_ALIGN, "to turn OFF");
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

#ifdef RTC_INSERTED
void TakePresentTime()
{
  now.refresh();
  PresentTime.day = now.day();
  PresentTime.month = now.month();
  PresentTime.hour = now.hour();
  PresentTime.minute = now.minute();
}
#endif

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

#ifdef RTC_INSERTED
static uint8_t conv2d(const char* p) 
{
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}
void RTCInit(const char* date, const char* time) 
{
	unsigned char hour, minute, second, day, month;
    switch (date[0]) 
	{
        case 'J': month = (date[1] == 'a') ? 1 : ((date[2] == 'n') ? 6 : 7); break;
        case 'F': month = 2; break;
        case 'A': month = date[2] == 'r' ? 4 : 8; break;
        case 'M': month = date[2] == 'r' ? 3 : 5; break;
        case 'S': month = 9; break;
        case 'O': month = 10; break;
        case 'N': month = 11; break;
        case 'D': month = 12; break;
    }
   day = conv2d(date + 4);
   hour = conv2d(time);
   minute = conv2d(time + 3);
   second = conv2d(time + 6);
   now.set(second, minute, hour, 1, day, month, 18);
   // RTCLib::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
}
#endif

void setup()
{
  
	Flags.Backlight = false;
	Flags.Setup = false;
	Flags.ShowInfo = false;
	Flags.BandOk = false;
	Flags.AllBandsInvalid = false;

	Serial.begin(9600);

	pinMode(BUTTON_UP, INPUT);
	pinMode(BUTTON_DOWN, INPUT);
	pinMode(BUTTON_SETUP, INPUT);
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	pinMode(LIGHT_SWITCH, OUTPUT);
	pinMode(PIR_PIN, INPUT);
	
	lcd_main.begin();  
	delay(1000);
	lcd_main.noBlink(); 

#ifdef RTC_INSERTED
	RTCInit(__DATE__, __TIME__);
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
#ifdef RTC_INSERTED
		ChekBandValue();
		if(Flags.BandOk)
		{
			gestionePIR(EepromTab[PIR_STATE].eeprom_par_value);	
			// BlinkLed(RED_LED);
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


