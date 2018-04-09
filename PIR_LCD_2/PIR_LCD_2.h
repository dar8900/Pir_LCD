
#include <Arduino.h>

#define MAX_LIGHT_DELAY   600 // In secondi
#define MIN_LIGHT_DELAY    20 // In secondi

#define MIN_MENU_PAGES   0
#define MAX_MENU_PAGES  sizeof(MainSetupItems)/sizeof(CREATE_MENU)

#define MIN_INFO_PAGES  0

#define MAX_LCD_ROW    3
#define MAX_LCD_COL   19

#define CENTER_ALIGN  10
#define RIGHT_ALIGN    19
#define LEFT_ALIGN     0

#define  OFF(pin)  digitalWrite(pin, LOW)
#define  ON(pin)   digitalWrite(pin, HIGH)

#define BUTTON_UP		PB13
#define BUTTON_DOWN		PB14
#define BUTTON_SETUP	PB15
#define RED_LED			PB4
#define GREEN_LED		PB3
#define BLUE_LED		PA15
#define YELLOW_LED		PA12
#define LIGHT_SWITCH	PA11
#define PIR_ANALOG_PIN	PA0

#define AUTOSCROLL_TIMER  100

#define BAND_INVALID_VALUE 	99

#define NO_BUTTON_PRESS      3
#define BUTTON_PRESS_DELAY	50

#define RTC_INSERTED

#ifdef RTC_INSERTED
typedef enum
{
  CHANGE_VALUE = 0,   // menu dove cambi solo un valore numerico
  SWITCH_STATE,   // menu dove viene gestito solo lo switch on off per uscite digitali
  INFO,        // menu a scorrimento automatico con tutte le info salvate in eeprom
  MANUAL_STATE,
  TIME_BAND_NUM
} MENU_TYPE_NBR;
#else
typedef enum
{
  CHANGE_VALUE = 0,   // menu dove cambi solo un valore numerico
  SWITCH_STATE,   // menu dove viene gestito solo lo switch on off per uscite digitali
  INFO       // menu a scorrimento automatico con tutte le info salvate in eeprom
} MENU_TYPE_NBR;
#endif

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
  MENU_TYPE_NBR typeMenu;
} EEPROM_ITEM;

typedef enum
{
  DELAY_AMOUNT = 0,
  PIR_STATE,
  MANUAL_LIGHT,
#ifdef RTC_INSERTED
  HOUR_BAND_1,
  HOUR_BAND_2, 
  MINUTE_BAND_1, 
  MINUTE_BAND_2, 
  DAY_BAND_1, 
  DAY_BAND_2, 
  MONTH_BAND_1, 
  MONTH_BAND_2,
#endif
  MAX_EEPROM_ITEM
 
}EEPROM_ITEM_ENUM;

enum
{
  TURN_ON = 0,
  TURN_OFF
};

#ifdef RTC_INSERTED
typedef struct
{
	short hour;
	short minute;
	short day;
	short month;
	short year;
} TIME_DATE_FORMAT;

// typedef struct
// {
	// short day;
	// short month;
	// short year;
// } DATE_FORMAT;

// typedef struct
// {
	// TIME_FORMAT BandTime;
	// DATE_FORMAT BandDate;
// } TIME_BAND;
#endif

// enum
// {
  // BUTTON_UP = 2,
  // BUTTON_DOWN,
  // BUTTON_SETUP,
  // RED_LED,
  // GREEN_LED,
  // BLUE_LED,
  // YELLOW_LED,
  // LIGHT_SWITCH
// };



typedef struct
{
	bool Backlight;
	bool Setup;
	bool ShowInfo;
    bool BandOk;
    bool AllBandsInvalid;
} FLAGS;

enum 
{
	UP = 0,
	DOWN,
	OK_EXIT
};
