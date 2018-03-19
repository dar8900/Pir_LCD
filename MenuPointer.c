#define MIN_MENU_PAGES 	1
#define MAX_MENU_PAGES  sizeof(MainSetupItems)/sizeof(CREATE_MENU)


#define MIN_INFO_PAGES 	1
#define MAX_INFO_PAGES  sizeof(TotEepromItem)/sizeof(EEPROM_ITEM)

typedef enum
{
	CHANGE_VALUE = 0, 	// menu dove cambi solo un valore numerico
	SWITCH_STATE,		// menu dove viene gestito solo lo switch on off per uscite digitali
	INFO 				// menu a scorrimento automatico con tutte le info salvate in eeprom
} MENU_TYPE_NBR;


typedef struct
{
	String nameMenu;
	MENU_TYPE_NBR typeMenu;
	bool (*MenuFunc)(void);
} CREATE_MENU;

MENU_TYPE_NBR MainSetupItems[] = 
{
	{"Change light delay", CHANGE_VALUE, ChangeValue},
	{"Change PIR state", SWITCH_STATE, SwichState},
	{"Show Info", INFO, InfoScroll},
};

typedef struct
{
	int eeprom_par_value;
	int eeprom_par_addr;
	int eeprom_par_numReg;
	String 	eeprom_par_name;
} EEPROM_ITEM;

EEPROM_ITEM DelayAmount;
EEPROM_ITEM PirSwitch;

EEPROM_ITEM EepromTab[] = 
{
	DelayAmount,
	PirSwitch,
};

DelayAmount.eeprom_par_name = "Light delay";
PirSwitch.eeprom_par_name = "PIR state";


bool InfoScroll()
{
	int buttonUp = LOW, buttonDown = LOW;
	int ExitButton = LOW; // 	Resetto ExitButton
	bool ExitInfo = false;
	int AutoScrollTimer = AUTOSCROLL_TIMER; // DA CONTROLLARE #define AUTOSCROLL_TIMER	100
	int Page = MIN_INFO_PAGES;
	
	OFF(RED_LED);
	ON(GREEN_LED);
	ON(BLUE_LED);
	lcd_main.blink();
	
	// ATTIVARE FLAG RETROILLUMINAZIONE !!!! /*************************************/

	// Pulire LCD
	ClearLCD();
	LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
	LCDPrintString(1,CENTER_ALIGN,"to scroll the info");
	LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
	
	delay(2000);
	
	while(1)
	{
		buttonUp = digitalRead(BUTTON_UP);
		buttonDown = digitalRead(BUTTON_DOWN);
		ExitButton = digitalRead(BUTTON_SETUP);
		
		if(AutoScrollTimer == 0)
		{
			Page++;
			if(Page > MAX_INFO_PAGES) // DEFINIRE I MAX INFO PAGES
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

void MainSetup()
{
	int buttonUp = LOW, buttonDown = LOW;
	int OkButton = LOW; // 	Resetto ExitButton
	bool ExitSetup = false;
	int Page = MIN_MENU_PAGES;
	
	OFF(RED_LED);
	OFF(GREEN_LED);
	ON(BLUE_LED);
	lcd_main.blink();
	
	// ATTIVARE FLAG RETROILLUMINAZIONE !!!! /*************************************/

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