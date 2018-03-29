#include <RTClib.h>
#include <Wire.h>  // Libreria di sistema - E' richiesta da I2CIO.cpp
#include <LiquidCrystal_I2C.h> // Libreria LCD I2C

#include "PIR_LCD_2.h"
#include "EEPROM_Ard.h"
#include "Band_Func.h"
#include "MenuFunc.h"

//LiquidCrystal_I2C lcd_main(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Old version
LiquidCrystal_I2C lcd_main(0x27, 20,4);

#ifdef RTC_INSERTED
TIME_FORMAT PresentTime;  // Variabili per l'orario
DATE_FORMAT PresentDate;  // Si perdono allo spegnimento ma vengono aggiornate subito all'accensione
						 //	
TIME_BAND Band_1, Band_2;//

RTC_DS1307 RTC;
DateTime now;
#endif

FLAGS Flags;

bool SetupOk = 0;

// bool FlagBacklight = false;

// bool FlagSetup = 0;
// bool FlagShowInfo = 0;

// #ifdef RTC_INSERTED
// bool FlagBandOk = false;
// bool FlagAllBandsInvalid = false;
// DateTime now;
// #endif

// bool ChangeDateTime(TIME_BAND  Band);

CREATE_MENU MainSetupItems[] = 
{
  {"Change light delay", ChangeValue		},
  {"Change PIR state"  , SwichState 		},
  {"Show Info"         , InfoScroll         },
#ifdef RTC_INSERTED
  {"Change Time Bands" , ChangeTimeBands    },
#endif
};

EEPROM_ITEM EepromTab[] = 
{
  {MIN_LIGHT_DELAY			,  START_DELAY_ADDR       , 1,  "Light delay"   , CHANGE_VALUE},
  {TURN_OFF					,  SWITCH_PIR_ADDR        , 1,  "PIR state"     , SWITCH_STATE},
#ifdef RTC_INSERTED
  {BAND_INVALID_VALUE       ,  HOUR_BAND_1_ADDR       , 1,  "Hour band 1"   , TIME_BAND_NUM},
  {BAND_INVALID_VALUE    	,  HOUR_BAND_2_ADDR       , 1,  "Hour band 2"   , TIME_BAND_NUM},
  {BAND_INVALID_VALUE       ,  MINUTE_BAND_1_ADDR     , 1,  "Minutes band 1", TIME_BAND_NUM},
  {BAND_INVALID_VALUE    	,  MINUTE_BAND_2_ADDR     , 1,  "Minutes band 2", TIME_BAND_NUM},
  {BAND_INVALID_VALUE    	,  DAY_BAND_1_ADDR        , 1,  "Day band 1"    , TIME_BAND_NUM},
  {BAND_INVALID_VALUE    	,  DAY_BAND_2_ADDR        , 1,  "Day band 2"    , TIME_BAND_NUM},
  {BAND_INVALID_VALUE    	,  MONTH_BAND_1_ADDR      , 1,  "Month band 1"   , TIME_BAND_NUM},
  {BAND_INVALID_VALUE    	,  MONTH_BAND_2_ADDR      , 1,  "Month band 2"   , TIME_BAND_NUM},
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
void LCDPrintString(short row, short col, String string) 
{
  if(col > MAX_LCD_COL || row > MAX_LCD_ROW || string.length() > 20) // ???
  {
	lcd_main.clear();
	col = CENTER_ALIGN;
	row = 3;
    string = "OVER DIMENSION";   
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

void LCDPrintLineVoid(short row)
{
  lcd_main.setCursor(0, row);
  lcd_main.print("                    ");
}

void LCDDisplayOn()
{
	lcd_main.backlight();
}


/************************ SETUP "RUNTIME" ******************************/

void MainSetup()
{
  short buttonUp = LOW, buttonDown = LOW;
  short OkButton = LOW; //  Resetto ExitButton
  bool ExitSetup = false;
  short Page = MIN_MENU_PAGES;
  
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
    EnterSetupButton();           
    Flags.Backlight = false;
    lcd_main.noBacklight(); 
    ClearLCD();
  }
  else if(Flags.Backlight && Flags.ShowInfo)
  {
	Flags.ShowInfo = 0;
	EnterSetupButton();           
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
	  val = analogRead(AnalogPirPin);    
	  val = (val *5)/1024;
	  ClearLCD();
	  if(val > 0)
	  {
		lcd_main.backlight();
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
	Time = String(PresentTime.hour) + ":" + String(PresentTime.minute);
	Date = String(PresentDate.day) + "/" + String(PresentDate.month);
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
		LCDPrintString(2, CENTER_ALIGN, "the sensor");
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

void setup()
{
  short numReg = 0;
  short FirstStartCheck = 0;
  
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

	
  lcd_main.begin();  
  delay(1000);
  lcd_main.noBlink(); 

  #ifdef RTC_INSERTED
  if (!RTC.begin()) 
  {
    BlinkLed(YELLOW_LED);
	BlinkLed(RED_LED);
    while (1);
  }
  
  if (!RTC.isrunning()) 
  {
	lcd_main.backlight();
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
    lcd_main.noBacklight();	
  }
#endif
  
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

   /*
	ANDREBBE SETTATO IL TEMPO E LA DATA CORRENTI NELLE VARIABILI GLOBALI
	IN MODO CHE NON SI POSSA SUPERARE IL NUMERO DI GIORNI PER IL MESE CORRENTE
	QUANDO USO LA FUNZIONE PER IL CAMBIAMENTO DI DATA
	*/
#ifdef RTC_INSERTED  
  now = RTC.now();
  PresentDate.day = now.day();
  PresentDate.month = now.month();
  PresentDate.year = now.year();
  PresentTime.hour = now.hour();
  PresentTime.minute = now.minute();
#endif 
  
  ClearLCD();

  InfoScroll();
  
  delay(1000);
  
  ClearLCD(); 
}

void loop()
{
#ifdef RTC_INSERTED
  now = RTC.now();
  PresentDate.day = now.day();
  PresentDate.month = now.month();
  PresentTime.hour = now.hour();
  PresentTime.minute = now.minute();
#endif
  WriteHomeMsg();
  ShowInfoMsg();
  if(Flags.Setup)
  {
    MainSetup();
  }
  else
  {
	// INSERIRE BOOLEANO PER CHEK DEL BAND, NEL CASO FOSSE FALSE (OVVERO FUORI FASCIA O INVALIDO) 
	// SPEGNERE IL SENSORE, ALTRIMENTI LASCIARE LA GESTIONE CHE C'è ORA
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


