#include "EEPROM_Ard.h"

#include <Wire.h>  // Libreria di sistema - E' richiesta da I2CIO.cpp
#include <LiquidCrystal_I2C.h> // Libreria LCD I2C

LiquidCrystal_I2C lcd_main(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#define MAX_LIGHT_DELAY   600 // In secondi
#define MIN_LIGHT_DELAY    20 // In secondi

#define MIN_MENU_PAGES   1
#define MAX_MENU_PAGES  sizeof(MainSetupItems)/sizeof(CREATE_MENU)


#define MIN_INFO_PAGES  1
#define MAX_INFO_PAGES  sizeof(EepromTab)/sizeof(EEPROM_ITEM)

#define MAX_LCD_ROW    3
#define MAX_LCD_COL   19

#define CENTER_ALIGN  10
#define RIGHT_ALIGN    19
#define LEFT_ALIGN     0

#define  OFF(pin)  digitalWrite(pin, LOW)
#define  ON(pin)   digitalWrite(pin, HIGH)

#define AnalogPirPin  0

typedef enum
{
  CHANGE_VALUE = 0,   // menu dove cambi solo un valore numerico
  SWITCH_STATE,   // menu dove viene gestito solo lo switch on off per uscite digitali
  INFO        // menu a scorrimento automatico con tutte le info salvate in eeprom
} MENU_TYPE_NBR;


typedef struct
{
  String nameMenu;
  MENU_TYPE_NBR typeMenu;
  bool (*MenuFunc)(void);
} CREATE_MENU;

MENU_TYPE_NBR MainSetupItems[] = 
{
  {"Change light delay", CHANGE_VALUE , ChangeValue},
  {"Change PIR state"  , SWITCH_STATE , SwichState },
  {"Show Info"         , INFO         , InfoScroll },
};

typedef struct
{
  int eeprom_par_value;
  int eeprom_par_addr;
  int eeprom_par_numReg;
  String  eeprom_par_name;
} EEPROM_ITEM;

typedef enum
{
  DELAY_AMOUNT = 0,
  PIR_SWITCH
  
}EEPROM_ITEM_ENUM;

EEPROM_ITEM EepromTab[] = 
{
  {MINIMO_DEALY,  START_DELAY_ADDR, 1,  "Light delay"},
  {0       ,  START_PIR_ADDR  , 1,  "PIR state"  },
};


extern int numReg;

int DelayAmount = MIN_LIGHT_DELAY;
int SwitchPIR = 0;
int FlagBacklight = 0;
int SetupOk = 0;
int FlagSetup = 0;


enum
{
  BUTTON_UP = 2,
  BUTTON_DOWN,
  BUTTON_SETUP,
  RED_LED,
  GREEN_LED,
  BLUE_LED,
  YELLOW_LED,
  PIR_SWITCH
};

const String OnOff[2] = {"On", "Off"};

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
      col = (MAX_LCD_COL - string.length()) / 2;
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
      col = (MAX_LCD_COL - ValStr.length()) / 2;
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

void SetupDelayInit()
{
  int timer = 80; // tempo di attesa di 5s con delay di 62ms
  int buttonUp = 0, buttonDown = 0;
  bool OkButton = false;
  int oldDelayAmount = DelayAmount;

  lcd_main.backlight();
  FlagBacklight = true;
  ClearLCD();
  LCDPrintString(0,CENTER_ALIGN,"Press on Enter/Ok");
  LCDPrintString(1,CENTER_ALIGN,"for enter in");
  LCDPrintString(2,CENTER_ALIGN,"initial Setup");
  LCDPrintString(3,CENTER_ALIGN,"You have 5 seconds");
  // Pulire LCD
  // Scrivere su LCD che si deve premere su SetupOk per accedere entro 5s
  while(timer > 0)
  {
    SetupOk = digitalRead(BUTTON_SETUP);
    if(SetupOk == HIGH)
    {
      // blink giallo
      
      BlinkLed(YELLOW_LED);
      ON(BLUE_LED); // Accendo il led blu di setup      
      // Pulire LCD
      ClearLCD();
      LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
      LCDPrintString(1,CENTER_ALIGN,"to change the delay");
      LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
      
      //  Resetto SetupOk
      SetupOk = LOW;
      
      delay(1000);
      // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
      // premere su setup per dare l'ok
      // Pulire LCD
      ClearLCD();
      while(1)
      {
        buttonUp = digitalRead(BUTTON_UP);
        buttonDown = digitalRead(BUTTON_DOWN);
        SetupOk = digitalRead(BUTTON_SETUP);
        delay(50);
        LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
        LCDPrintString(1,CENTER_ALIGN,"to change the delay");
        LCDPrintLineVoid(2);
        LCDPrintValue(2,8, DelayAmount);
        LCDPrintString(2,11,"sec");
        LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
        // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
        // premere su setup per dare l'ok
        
        if(buttonUp == HIGH)
        {
          BlinkLed(YELLOW_LED); // blink giallo
          if(DelayAmount == 0)
          {
            DelayAmount == MIN_LIGHT_DELAY;
          }
          DelayAmount++;
          if(DelayAmount > MAX_LIGHT_DELAY) // Raggiunti i secondi massimi per il delay nella gestione pir
          {
            DelayAmount = MIN_LIGHT_DELAY;
          }
        }
        if(buttonDown == HIGH)
        {
          BlinkLed(YELLOW_LED); // blink giallo
          DelayAmount--;
          if(DelayAmount < MIN_LIGHT_DELAY) // Raggiunti i secondi minimi per il delay nella gestione pir
          {
            DelayAmount = MAX_LIGHT_DELAY;
          }
        }
        if(SetupOk == HIGH)
        {
          BlinkLed(YELLOW_LED); // blink giallo
          // Scrivere su LCD "Valori salvati"
          ClearLCD();
          if(oldDelayAmount != DelayAmount)
          {
            LCDPrintString(1,CENTER_ALIGN,"Value Saved!");  
            WriteMemory(START_DELAY_ADDR, DelayAmount);       
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
    }
    if(OkButton)
    {
      OFF(BLUE_LED);
      break; // Esci da SetupInit
    }
    timer--;
    delay(62);    
  }
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
  lcd_main.blink();
  
  FlagBacklight = true;

  // Pulire LCD
  ClearLCD();
  LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
  LCDPrintString(1,CENTER_ALIGN,"to scroll the menù");
  LCDPrintString(3,CENTER_ALIGN,"Press Ok to enter");
  
  delay(2000);
  
  while(1)
  {
    buttonUp = digitalRead(BUTTON_UP);
    buttonDown = digitalRead(BUTTON_DOWN);
    OkButton = digitalRead(BUTTON_SETUP);
    // Pulire LCD
    ClearLCD();
    LCDPrintString(2, CENTER_ALIGN, MainSetupItems[Page].nameMenu);
    
    if(buttonUp == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      Page++;
      if(Page > MAX_MENU_PAGES) 
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
        Page = MAX_MENU_PAGES;
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

bool SwichState()
{
  
  int buttonUp = 0, buttonDown = 0;
  bool OkButton = false;
  int SwitchOnOff = SwitchPIR;

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
    delay(50);
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
      
      if(SwitchOnOff != SwitchPIR)
      {
        SwitchPIR = SwitchOnOff;
        WriteMemory(START_SWITCH_PIR_ADDR, SwitchPIR);
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
  int oldDelayAmount = DelayAmount;

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
    delay(50);
    // Pulire LCD
    LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(1,CENTER_ALIGN,"to change the value:");
    LCDPrintLineVoid(2);
    LCDPrintValue(2,8, DelayAmount);
    LCDPrintString(2,11,"sec");
    LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
    // premere su setup per dare l'ok

    if(buttonUp == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      if(DelayAmount == 0)
      {
        DelayAmount == MIN_LIGHT_DELAY;
      }
      DelayAmount++;
      if(DelayAmount > MAX_LIGHT_DELAY) // Raggiunti i secondi massimi per il delay nella gestione pir
      {
        DelayAmount = MIN_LIGHT_DELAY;
      }
    }
    if(buttonDown == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      DelayAmount--;
      if(DelayAmount < MIN_LIGHT_DELAY) // Raggiunti i secondi minimi per il delay nella gestione pir
      {
        DelayAmount = MAX_LIGHT_DELAY;
      }
    }
    if(SetupOk == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo

      // Scrivere su LCD "Valori salvati"
      ClearLCD();
      if(oldDelayAmount != DelayAmount)
      {
        LCDPrintString(1,CENTER_ALIGN,"Value Saved!");
        WriteMemory(START_DELAY_ADDR, DelayAmount);         
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
  int AutoScrollTimer = AUTOSCROLL_TIMER; // DA CONTROLLARE #define AUTOSCROLL_TIMER  100
  int Page = MIN_INFO_PAGES;
  
  OFF(RED_LED);
  ON(GREEN_LED);
  ON(BLUE_LED);
  lcd_main.blink();
  
  FlagBacklight = true;

  // Pulire LCD
  ClearLCD();
  LCDPrintString(0,CENTER_ALIGN,"Press Up, Down");
  LCDPrintString(0,CENTER_ALIGN,"or nothing");
  LCDPrintString(2,CENTER_ALIGN,"to scroll the info");
  LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
  
  delay(2000);
  
  while(1)
  {
    buttonUp = digitalRead(BUTTON_UP);
    buttonDown = digitalRead(BUTTON_DOWN);
    ExitButton = digitalRead(BUTTON_SETUP);
    
    if(AutoScrollTimer == 0)
    {
      delay(1000);
      Page++;
      if(Page > MAX_INFO_PAGES) 
      {
        Page = MIN_INFO_PAGES;
      }
    }
    
    // Pulire LCD
    ClearLCD();
    LCDPrintString(1, CENTER_ALIGN, EepromTab[Page].eeprom_par_name);
    LCDPrintString(2, CENTER_ALIGN, EepromTab[Page].eeprom_par_value);
    
    if(buttonUp == HIGH)
    {
      BlinkLed(YELLOW_LED); // blink giallo
      AutoScrollTimer = AUTOSCROLL_TIMER;
      Page++;
      if(Page > MAX_INFO_PAGES) 
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
        Page = MAX_INFO_PAGES;
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
  if(FlagBacklight)
  {
    LCDPrintString(0,CENTER_ALIGN,"Press on Enter/Ok");
    LCDPrintString(1,CENTER_ALIGN,"for enter in");
    LCDPrintString(2,CENTER_ALIGN,"Main Setup");
    delay(2000); 
    ClearLCD(); 
    LCDPrintString(1,CENTER_ALIGN,"After the display");
    LCDPrintString(2,CENTER_ALIGN,"is turned off"); 
    delay(2000);
    EnterSetupButton();           
    FlagBacklight = false;
    lcd_main.noBacklight(); 
  ClearLCD();
  }
  else
  {
    InterruptFunc();
//    LCDPrintString(0,CENTER_ALIGN,"Press on Enter/Ok");
//    LCDPrintString(1,CENTER_ALIGN,"for enter in");
//    LCDPrintString(2,CENTER_ALIGN,"Delay Setup");    
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
    ReadMemory(NUM_REG_ADDR , 1, &numReg);
    ReadMemory(START_DELAY_ADDR, numReg, &DelayAmount);
    LcdTimeWrite(DelayAmount);
  }
  else
  {
    ON(RED_LED);
    OFF(GREEN_LED);
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
  
  lcd_main.begin(20,4);  
  delay(1000);
  lcd_main.noBlink(); 

  ReadMemory(NUM_REG_ADDR, 1, &numReg);
  ReadMemory(START_DELAY_ADDR, numReg, &DelayAmount);
  ReadMemory(START_SWITCH_PIR_ADDR, 1, &SwitchPIR);
  
  lcd_main.backlight();
  FlagBacklight = true;
  ClearLCD();
  LCDPrintString(0, CENTER_ALIGN, "The delay is");
  LCDPrintValue(1, CENTER_ALIGN, DelayAmount);
  LCDPrintString(2, CENTER_ALIGN, "The PIR is");
  LCDPrintString(3, CENTER_ALIGN, OnOff[SwitchPIR]);
  delay(4000);
  //SetupDelayInit();
  
  ClearLCD(); 
}

void loop()
{
  WriteHomeMsg();
  if(FlagSetup)
  {
    MainSetup();
  }
  else
  {
  if(SwitchPIR == 0)
  {
    digitalWrite(PIR_SWITCH, HIGH);
    gestionePIR(AnalogPirPin);
  }
  else
  {
    digitalWrite(PIR_SWITCH, LOW);
  }  
  }
}

