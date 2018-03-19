typedef enum
{
	CHANGE_VALUE = 0, 	// menu dove cambi solo un valore numerico
	SWITCH_STATE,				// menu dove viene gestito solo lo switch on off per uscite digitali
	INFO 				// menu a scorrimento automatico con tutte le info salvate in eeprom
} MENU_TYPE_NBR;

typedef struct
{
	bool (*MenuFunc)(void);
} MENU_FUNC;

MENU_FUNC TypesMenuFunc[] = 
{
	ChangeValue,
	SwichState,
	InfoFunc
};


typedef struct
{
	int eeprom_par_value;
	int eeprom_par_addr;
	int eeprom_par_numReg;	
} EEPROM_ITEM;

EEPROM_ITEM PirSwitch;
EEPROM_ITEM DelayAmount;


