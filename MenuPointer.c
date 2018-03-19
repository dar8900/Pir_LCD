typedef struct
{
	byte numMenu;
	String nameMenu;
	int (*SetupVoices)(void);
	
} SETUP_MENU_T;

SETUP_MENU_T MainSetup[] = 
{
	{DELAY_SETUP, "Setup Delay Time", SetupDelay},
	{SWITCH_PIR, "Setup PIR", SwitchPir}
};