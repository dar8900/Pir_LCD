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
TIME_FOMAT PresentTime;  // Variabili per l'orario
DATE_FOMAT PresentDate;  // Si perdono allo spegnimento ma vengono aggiornate subito all'accensione
						 //	
TIME_BAND Band_1, Band_2;//

RTC_DS1307 RTC;
#endif

bool FlagBacklight = false;
bool SetupOk = 0;
bool FlagSetup = 0;
bool FlagShowInfo = 0;

#ifdef RTC_INSERTED
bool FlagBandOk = false;
bool FlagAllBandsInvalid = false;
#endif

// bool ChangeDateTime(TIME_BAND  Band);

CREATE_MENU MainSetupItems[] = 
{
  {"Change light delay", CHANGE_VALUE , ChangeValue		},
  {"Change PIR state"  , SWITCH_STATE , SwichState 		},
  {"Show Info"         , INFO         , InfoScroll      },
#ifdef RTC_INSERTED
  {"Change Time Bands" , TIME_BAND_NUM, ChangeTimeBands },
#endif
};

EEPROM_ITEM EepromTab[] = 
{
  {MIN_LIGHT_DELAY			,  START_DELAY_ADDR       , 1,  "Light delay"   , CHANGE_VALUE},
  {OFF     					,  SWITCH_PIR_ADDR        , 1,  "PIR state"     , SWITCH_STATE},
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
void LCDPrintString(byte row, byte col, String string) 
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

void LCDPrintValue(byte row, byte col, short value)
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

void LCDPrintLineVoid(byte row)
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
  byte buttonUp = LOW, buttonDown = LOW;
  byte OkButton = LOW; //  Resetto ExitButton
  bool ExitSetup = false;
  byte Page = MIN_MENU_PAGES;
  
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

void InterruptFunc()
{
  byte timer = 6; // 0.1s c.a (105ms)
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
  short val = 0;
  byte numReg;
  val = analogRead(analogPin);    
  val = (val *5)/1024;
  ClearLCD();
  if(val > 0)
  {
    lcd_main.backlight();
    FlagBacklight = true;
    ON(GREEN_LED); 
    OFF(RED_LED);
    ReadMemory(NUM_REG_ADDR , 1, (short*)&numReg);
    ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
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
	byte InfoPressed = LOW;
	byte timer = 6; // 0.06s c.a (60ms)
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
#ifdef RTC_INSERTED
		LCDPrintString(0, CENTER_ALIGN, "Change the bands");
		LCDPrintString(1, CENTER_ALIGN, "to set when");
		LCDPrintString(2, CENTER_ALIGN, "turn ON");
		LCDPrintString(2, CENTER_ALIGN, "the sensor");
		delay(3000);
		ClearLCD();
#endif
	}
	
	
}


// Blink del led per 5ms
void BlinkLed(byte pin) 
{
  digitalWrite(pin, HIGH);
  delay(5);
  digitalWrite(pin, LOW);
}

void setup()
{
  byte numReg;
  
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

#ifdef RTC_INSERTED
  if (!RTC.begin()) 
  {
    Serial.println("Couldn't find RTC");
    BlinkLed(YELLOW_LED);
    while (1);
  }
  
    if (!RTC.isrunning()) 
	{
		Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
		RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
		ON(YELLOW_LED);
		ON(BLUE_LED);
		delay(2000);
		OFF(YELLOW_LED);
		OFF(BLUE_LED);		
    }
#endif
	
  lcd_main.begin(20,4);  
  delay(1000);
  lcd_main.noBlink(); 

  /*
	ANDREBBE SETTATO IL TEMPO E LA DATA CORRENTI NELLE VARIABILI GLOBALI
	IN MODO CHE NON SI POSSA SUPERARE IL NUMERO DI GIORNI PER IL MESE CORRENTE
	QUANDO USO LA FUNZIONE PER IL CAMBIAMENTO DI DATA
	*/
#ifdef RTC_INSERTED  
  DateTime now = RTC.now();
  PresentDate.day = now.day();
  PresentDate.month = now.month();
  PresentDate.year = now.year();
  PresentTime.hour = now.hour();
  PresentTime.minute = now.minute();
 
  SetBandInvalid(); // NON QUESTO MA SETTARE LE VARIABILI GLOBALI BAND_1 BAND_2 LEGGENDO DALLA MEMORIA 
  //ReadBandFromEeprom();
 #endif 
  
  ReadMemory(NUM_REG_ADDR, 1, (short*)&numReg); // Inizializzo il numero registri per il delay
  ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
  ReadMemory(EepromTab[PIR_STATE].eeprom_par_addr, EepromTab[PIR_STATE].eeprom_par_numReg, &EepromTab[PIR_STATE].eeprom_par_value);
 
  
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
#ifdef RTC_INSERTED
	ChekBandValue();
	if(FlagBandOk)
	{
		if(EepromTab[PIR_STATE].eeprom_par_value == TURN_ON)
		{
			ON(PIR_SWITCH);
			delay(150);
			gestionePIR(AnalogPirPin);
		}
		else
		{
			OFF(PIR_SWITCH);
		}  		
	}
	else
		OFF(PIR_SWITCH);
#else
	if(EepromTab[PIR_STATE].eeprom_par_value == TURN_ON)
	{
		ON(PIR_SWITCH);
		delay(150);
		gestionePIR(AnalogPirPin);
	}
	else
	{
		OFF(PIR_SWITCH);
	}
#endif  	

  }
}


