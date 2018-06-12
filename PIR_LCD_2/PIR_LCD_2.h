#include <Arduino.h>
#include <avr/wdt.h>

#define MAX_LIGHT_DELAY   600 // In secondi
#define MIN_LIGHT_DELAY    20 // In secondi


#define  OFF(pin)  digitalWrite(pin, LOW)
#define  ON(pin)   digitalWrite(pin, HIGH)

#define READ(pin)  digitalRead(pin)


#define NO_BUTTON_PRESS      3
#define BUTTON_PRESS_DELAY	20

#define RTC_INSERTED

typedef struct
{
  String nameMenu;
  bool (*MenuFunc)(void);
} CREATE_MENU;


typedef struct
{
  short eeprom_par_value;
  short eeprom_par_addr;
  short eeprom_par_numReg;
  String  eeprom_par_name;
} EEPROM_ITEM;

typedef enum
{
  DELAY_AMOUNT = 0,
  PIR_STATE,
  MANUAL_LIGHT,
  MAX_EEPROM_ITEM

}EEPROM_ITEM_ENUM;


enum
{
					// PIN
  BUTTON_UP = 2,	//  2
  BUTTON_DOWN,      //  3
  BUTTON_SETUP,     //  4
  RED_LED,          //  5
  GREEN_LED,        //  6
  BLUE_LED,         //  7
  YELLOW_LED,       //  8
  LIGHT_SWITCH,     //  9
  PIR_INPUT         // 10
};

typedef enum
{
    UP = 0,
    DOWN,
    OK_EXIT,
    EXIT_MANUAL,
    NOPRESS
} BUTTONS;


typedef struct
{
	bool Backlight;
    bool IsBandSetted;
    bool BandActive;
    bool InBand;
	bool ManualState;
} FLAGS;

void ResetWD(void);
void EnableWD(short Delay);
short CheckButtons(void);
void BlinkLed(short pin);
