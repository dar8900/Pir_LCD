
#include <Arduino.h>

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
  PIR_STATE,
  HOUR_BAND_1,
  HOUR_BAND_2, 
  MINUTE_BAND_1, 
  MINUTE_BAND_2, 
  DAY_BAND_1, 
  DAY_BAND_2, 
  MONTH_BAND_1, 
  MONTH_BAND_2 
 
}EEPROM_ITEM_ENUM;

enum
{
  TURN_ON = 0,
  TURN_OFF
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