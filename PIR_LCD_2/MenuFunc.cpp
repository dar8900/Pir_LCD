

#include "PIR_LCD_2.h"
#include "EEPROM_Ard.h"

#include "MenuFunc.h"
#include "LCDLib.h"
#include "TimeLib.h"
#include "Band.h"

#define LCD_MANUAL_DELAY_TIMER  300
#define INFO_SCROLL_DELAY_TIMER  40



extern EEPROM_ITEM EepromTab[];
extern FLAGS  Flags;
extern BAND_FORMAT Band;

static const String OnOff[2] = {"On", "Off"};


const CREATE_MENU MainSetupItems[] =
{
  {"Change light delay", ChangeValue		},
  {"Change PIR state"  , SwichState 		},
  {"Show Info"         , InfoScroll         },
  {"Manual State"	   , ManualState 		},
  {"Change Time Bands" , ChangeTimeBands    },
  {"Change Date/Time"  , ChangeTime		    },
  {"Reset Default"     , ResetAll           },
};

void MainScreen()
{
    short ButtonPress = NOPRESS, PirState = EepromTab[PIR_STATE].eeprom_par_value;
    bool ShowMsg = false, EnterSetup = false;
    LedCtrl(NO_LED);
    if(Flags.Backlight)
    {
        ClearLCD();
        LCDPrintString(TWO,CENTER_ALIGN,"Press on Enter/Ok");
        LCDPrintString(THREE,CENTER_ALIGN,"for enter in");
        LCDPrintString(FOUR,CENTER_ALIGN,"Main Setup");
        delay(DELAY_MESSAGE_MENU);
        ClearLCD();
        LCDPrintString(TWO,CENTER_ALIGN,"After the display");
        LCDPrintString(THREE,CENTER_ALIGN,"is turned off");
        delay(DELAY_MESSAGE_MENU);
        ClearLCD();
        LCDDisplayOff();
        Flags.Backlight = false;
    }
    ClearLCD();
    if(!CheckBand())
    {
        while(!EnterSetup)
        {
            if(Flags.InBand)
            {
                if(!Flags.Backlight)
                {
                    LCDDisplayOn();
                    Flags.Backlight = true;
                }
                LCDPrintString(TWO,CENTER_ALIGN,"Exit from");
                LCDPrintString(THREE,CENTER_ALIGN,"band...");
                delay(DELAY_MESSAGE_MENU);
                ShowMsg = true;
                Flags.InBand = false;
            }
            ButtonPress = CheckButtons();
            switch (ButtonPress)
            {
                case UP:
                case DOWN:
                    BlinkLed(YELLOW_LED);
                    ShowMsg = true;
                    break;
                case OK_EXIT:
                    BlinkLed(YELLOW_LED);
                    EnterSetup = true;
                    break;
                default:
                    break;
            }
            ButtonPress = NOPRESS;
            if(ShowMsg)
            {
                ClearLCD();
                if(!Flags.Backlight)
                {
                    LCDDisplayOn();
                    Flags.Backlight = true;
                }
                LedCtrl(MESSAGE_POS);
                LCDPrintString(TWO,CENTER_ALIGN,"Press on Enter/Ok");
                LCDPrintString(THREE,CENTER_ALIGN,"for enter in");
                LCDPrintString(FOUR,CENTER_ALIGN,"Main Setup");
                delay(DELAY_MESSAGE_MENU);
                ClearLCD();
                LCDPrintString(TWO,CENTER_ALIGN,"After the display");
                LCDPrintString(THREE,CENTER_ALIGN,"is turned off");
                delay(DELAY_MESSAGE_MENU);
                if(Flags.Backlight)
                {
                    LCDDisplayOff();
                    Flags.Backlight = false;
                }
                ClearLCD();
                ShowMsg = false;
                LedCtrl(NO_LED);
            }
            if(!EnterSetup)
            {
                gestionePIR(PirState);
            }
            delay(WHILE_LOOP_DELAY);
            //ResetWD();
        }
        if(EnterSetup)
        {
            EnterSetup = false;
            MainSetup();
        }
    }
    else
    {
        gestionePIR(OFF);
        if(Flags.Backlight)
        {
            LCDDisplayOff();
            Flags.Backlight = false;
        }
        GesLight(OFF);
        Flags.InBand = true;
        //ResetWD();
    }
}

void gestionePIR(short ActivePIR)
{
    short StatePir = LOW, TimeDelay = 0, numReg = 0;
    if(ActivePIR == ON)
    {
        LedCtrl(PIR_ACTIVE_POS);
        StatePir = READ(PIR_INPUT);
        if(StatePir == HIGH)
        {
            ReadMemory(NUM_REG_ADDR , 1, &numReg);
            ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
            TimeDelay = EepromTab[DELAY_AMOUNT].eeprom_par_value;
            ON(LIGHT_SWITCH);
			if(!Flags.Backlight)
				LCDDisplayOn();
            LcdTimeWrite(TimeDelay);
            OFF(LIGHT_SWITCH);
			LCDDisplayOff();
        }
        LedCtrl(NO_LED);
        //ResetWD();
    }
}

void GesLight(short LightState)
{
    if(LightState == ON)
    {
        ON(LIGHT_SWITCH);
        EepromUpdate(LIGHT_STATE_ADDR, ON);
    }
    else
    {
        OFF(LIGHT_SWITCH);
        EepromUpdate(LIGHT_STATE_ADDR, OFF);
    }
}

void ManualScreen()
{
    short ButtonPress = NOPRESS, LightState = OFF, OldLightState = OFF;
	int TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
    bool ExitManualState = false, OldValues = false;;
    ReadMemory(LIGHT_STATE_ADDR, 1, &OldLightState);
    ClearLCD();
    LCDDisplayOn();
    Flags.Backlight = true;
    LedCtrl(MANUAL_POS);
    LCDPrintString(ONE, CENTER_ALIGN, "Manual State");
    LCDPrintString(TWO, CENTER_ALIGN, "The light is:");
    LCDPrintString(FOUR, CENTER_ALIGN, "Press Ok to confirm");
    while(!ExitManualState)
    {
        if(OldValues)
        {
            LCDPrintString(ONE, CENTER_ALIGN, "Manual State");
            LCDPrintString(TWO, CENTER_ALIGN, "The light is:");
            LCDPrintString(FOUR, CENTER_ALIGN, "Press Ok to confirm");
            OldValues = false;
        }
        LCDPrintString(THREE, CENTER_ALIGN, OnOff[LightState]);
        ButtonPress = CheckButtons();
        switch (ButtonPress)
         {
            case UP:
                BlinkLed(YELLOW_LED);
                if(TimerDisplayOn == 0)
                {
                    TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
                }
                else
                {
                    TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
                    if(LightState == OFF)
                        LightState = ON;
                    else
                        LightState = OFF;
                }
                ClearLCDLine(THREE);
                break;
            case DOWN:
                BlinkLed(YELLOW_LED);
                if(TimerDisplayOn == 0)
                {
                    TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
                }
                else
                {
                    TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
                    if(LightState == OFF)
                        LightState = ON;
                    else
                        LightState = OFF;
                }
                ClearLCDLine(THREE);
                break;
            case OK_EXIT:
                BlinkLed(YELLOW_LED);
                if(TimerDisplayOn == 0)
                {
                    TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
                }
                else
                {
                    TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
                    if(LightState != OldLightState)
					{
						ClearLCD();
                        GesLight(LightState);
						EepromUpdate(LIGHT_STATE_ADDR, LightState);
						OldLightState = LightState;
						LCDPrintString(TWO, CENTER_ALIGN, "The light is");
						LCDPrintString(THREE, CENTER_ALIGN, OnOff[LightState]);
						delay(DELAY_MESSAGE_MENU);
						ClearLCD();
					}
                    else
                    {
                        ClearLCD();
                        LCDPrintString(ONE, CENTER_ALIGN, "The light is");
                        LCDPrintString(TWO, CENTER_ALIGN, "already");
                        LCDPrintString(THREE, CENTER_ALIGN, OnOff[LightState]);
                        delay(DELAY_MESSAGE_MENU);
                        ClearLCD();
                        OldValues = true;
                    }
                }
                break;
        }
        if(ButtonPress == EXIT_MANUAL)
        {
            if(!Flags.Backlight)
            {
                LCDDisplayOn();
                Flags.Backlight = true;
            }
			ClearLCD();
			LCDPrintString(TWO, CENTER_ALIGN, "Exit from");
			LCDPrintString(THREE, CENTER_ALIGN, "manual state");
			delay(DELAY_MESSAGE_MENU);
			ClearLCD();
			Flags.ManualState = false;
			ExitManualState = true;
			WriteMemory(MANUAL_STATE_ADDR, OFF);
        }
		if(TimerDisplayOn > 0)
			TimerDisplayOn--;
        if(TimerDisplayOn == 0)
        {
            if(Flags.Backlight)
            {
                LCDDisplayOff();
                Flags.Backlight = false;
            }
        }
        else
        {
            LCDDisplayOn();
            Flags.Backlight = true;
        }
        delay(10);
        //ResetWD();
    }
    LedCtrl(NO_LED);
    return true;
}


void MainSetup()
{
    bool ExitSetup = false;
    short MenuItem = LIGHT_DELAY;
    short ButtonPress;
    LedCtrl(MENU_POS);
    LCDDisplayOn();
    Flags.Backlight = true;
    ClearLCD();
    LCDPrintString(ONE,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(TWO,CENTER_ALIGN,"to scroll the menu");
    LCDPrintString(FOUR,CENTER_ALIGN,"Press Ok to enter");
    delay(DELAY_MESSAGE_MENU);
    ClearLCD();
    while(!ExitSetup)
    {
		ShowDateTime(ONE);
        LCDPrintString(TWO, CENTER_ALIGN, MainSetupItems[MenuItem].nameMenu);
        ButtonPress = CheckButtons();
        switch(ButtonPress)
        {
            case UP:
                BlinkLed(YELLOW_LED); // blink giallo
                MenuItem--;
                if(MenuItem < LIGHT_DELAY)
                {
                    MenuItem = MAX_MENU_ITEMS - 1;
                }
                ClearLCDLine(TWO);
                break;
            case DOWN:
            BlinkLed(YELLOW_LED); // blink giallo
                MenuItem++;
                if(MenuItem > MAX_MENU_ITEMS - 1)
                {
                    MenuItem = LIGHT_DELAY;
                }
                ClearLCDLine(TWO);
                break;
            case OK_EXIT:
                BlinkLed(YELLOW_LED); // blink giallo
                ExitSetup = true;
                ClearLCD();
                break;
            default:
                break;
        }
        //ResetWD();
    }
    if(ExitSetup)
    {
        MainSetupItems[MenuItem].MenuFunc();
    }
    LedCtrl(NO_LED);
}

bool ManualState()
{
    short ButtonPress = NOPRESS, SwitchOnOff = OFF;
    bool ExitSetManual = false;
    ClearLCD();
    LCDPrintString(ONE, CENTER_ALIGN, "Set the");
    LCDPrintString(TWO, CENTER_ALIGN, "manual state?");
    if(CheckYesNo())
    {
        LedCtrl(MESSAGE_POS);
        ClearLCD();
        LCDPrintString(THREE, CENTER_ALIGN, "Setted!");
        delay(DELAY_MESSAGE_MENU);
        ClearLCD();
        Flags.ManualState = true;
        EepromUpdate(MANUAL_STATE_ADDR, ON);
        LedCtrl(MENU_POS);
    }
    else
    {
        LedCtrl(MESSAGE_POS);
        ClearLCD();
        LCDPrintString(THREE, CENTER_ALIGN, "Not setted");
        delay(DELAY_MESSAGE_MENU);
        ClearLCD();
        Flags.ManualState = false;
        EepromUpdate(MANUAL_STATE_ADDR, OFF);
        LedCtrl(MENU_POS);
    }
    return true;
}

bool ChangeValue()
{
    bool OkButton = false;
    short numReg;
    short ButtonPress;
    ReadMemory(NUM_REG_ADDR, 1, &numReg);
    ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
    short oldDelayAmount = EepromTab[DELAY_AMOUNT].eeprom_par_value;
    short ChangeDelayAmount = EepromTab[DELAY_AMOUNT].eeprom_par_value;
    String TimeOverSixty;
    short Minute = 0, Second = 0;

    // Pulire LCD
    ClearLCD();
    LCDPrintString(ONE,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(TWO,CENTER_ALIGN,"to change the value");
    LCDPrintString(FOUR,CENTER_ALIGN,"Press Ok to exit");

    delay(DELAY_MESSAGE_MENU);
    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di
    // premere su setup per dare l'ok
    ClearLCD();
    while(!OkButton)
    {
        ButtonPress = CheckButtons();
        if(ChangeDelayAmount <= 60)
        {
            LCDPrintString(ONE,CENTER_ALIGN,"Press Up or Down");
            LCDPrintString(TWO,CENTER_ALIGN,"to change the value:");
            LCDPrintValue(THREE,8, ChangeDelayAmount);
            LCDPrintString(THREE,11,"sec");
            LCDPrintString(FOUR,CENTER_ALIGN,"Press Ok to exit");
        }
        else
        {
            Minute = ChangeDelayAmount / 60;
            Second = ChangeDelayAmount % 60;
            if(Second < 10)
            {
                TimeOverSixty = String(Minute) + ":0" + String(Second) + "min";
            }
            else
            {
                TimeOverSixty = String(Minute) + ":" + String(Second) + "min";
            }
            LCDPrintString(ONE,CENTER_ALIGN,"Press Up or Down");
            LCDPrintString(TWO,CENTER_ALIGN,"to change the value:");
            LCDPrintString(THREE,CENTER_ALIGN, TimeOverSixty);
            LCDPrintString(FOUR,CENTER_ALIGN,"Press Ok to exit");
        }
        switch(ButtonPress)
        {
            case UP:
                BlinkLed(YELLOW_LED); // blink giallo
                if(ChangeDelayAmount == 0)
                {
                    ChangeDelayAmount == MIN_LIGHT_DELAY;
                }
                ChangeDelayAmount++;
                if(ChangeDelayAmount > MAX_LIGHT_DELAY) // Raggiunti i secondi massimi per il delay nella gestione pir
                {
                    ChangeDelayAmount = MIN_LIGHT_DELAY;
                }
                ClearLCDLine(THREE);
                break;
            case DOWN:
            BlinkLed(YELLOW_LED); // blink giallo
                ChangeDelayAmount--;
                if(ChangeDelayAmount < MIN_LIGHT_DELAY) // Raggiunti i secondi minimi per il delay nella gestione pir
                {
                    ChangeDelayAmount = MAX_LIGHT_DELAY;
                }
                ClearLCDLine(THREE);
            break;
            case OK_EXIT:
            BlinkLed(YELLOW_LED); // blink giallo
            // Scrivere su LCD "Valori salvati"
            ClearLCD();
            LedCtrl(MESSAGE_POS);
            if(oldDelayAmount != ChangeDelayAmount)
            {
                LCDPrintString(TWO,CENTER_ALIGN,"Value Saved!");
                WriteMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, ChangeDelayAmount);
                ReadMemory(NUM_REG_ADDR, 1, (short*)&numReg);
                ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &ChangeDelayAmount);
				BlinkAllLed();
            }
            else
            {
                LCDPrintString(TWO,CENTER_ALIGN,"Value Unchanged");
            }
            OkButton = true;
            delay(DELAY_MESSAGE_MENU);
            LedCtrl(MENU_POS);
            break;
            default:
            break;

        }
        //ResetWD();
    }
    ClearLCD();
    return true;
}


bool SwichState()
{
    short ButtonPress;
    short EepromItem;
    bool OkSwitch = false;
    EepromItem = PIR_STATE;
	short PirState;
    ReadMemory(EepromTab[EepromItem].eeprom_par_value, 1, &PirState);
    short OldSwitch = PirState;
    short SwitchOnOff = OFF;
    ClearLCD();
    LCDPrintString(ONE,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(TWO,CENTER_ALIGN,"to change the state");
    LCDPrintString(THREE,CENTER_ALIGN,"On or Off");
    LCDPrintString(FOUR,CENTER_ALIGN,"Press Ok to exit");
    delay(DELAY_MESSAGE_MENU);
    ClearLCD();
    while(!OkSwitch)
    {
        ButtonPress = CheckButtons();
        delay(BUTTON_PRESS_DELAY);
        LCDPrintString(ONE,CENTER_ALIGN,"Press Up or Down");
        LCDPrintString(TWO,CENTER_ALIGN,"to change the state");
        ClearLCDLine(THREE);
        LCDPrintString(THREE, CENTER_ALIGN, OnOff[SwitchOnOff]);
        LCDPrintString(FOUR,CENTER_ALIGN,"Press Ok to exit");
        switch(ButtonPress)
        {
            case UP:
                BlinkLed(YELLOW_LED);
                if(SwitchOnOff == OFF)
                    SwitchOnOff = ON;
                else
                    SwitchOnOff = OFF;
                ClearLCDLine(THREE);
                break;
            case DOWN:
                BlinkLed(YELLOW_LED);
                if(SwitchOnOff == OFF)
                    SwitchOnOff = ON;
                else
                    SwitchOnOff = OFF;
                ClearLCDLine(THREE);
                break;
            case OK_EXIT:
                BlinkLed(YELLOW_LED); // blink giallo
                ClearLCD();
                LedCtrl(MESSAGE_POS);
                LCDPrintString(TWO, CENTER_ALIGN, "The state is");
                LCDPrintString(THREE, CENTER_ALIGN, OnOff[SwitchOnOff]);
                if(SwitchOnOff != OldSwitch)
                {
                    EepromUpdate(EepromTab[EepromItem].eeprom_par_addr, SwitchOnOff);
					BlinkAllLed();	
                }
                OkSwitch = true;
                delay(DELAY_MESSAGE_MENU);
                LedCtrl(MENU_POS);
                break;
            default:
                break;
        }
        //ResetWD();
    }
    ClearLCD();
    return OkSwitch;

}

bool InfoScroll()
{
    short TimerScrollInfo = INFO_SCROLL_DELAY_TIMER, ButtonPress = NOPRESS;
    bool ExitInfo = false;
    short Page = LIGHT_DELAY_INFO;
    short numReg;
	short StatePir;
    String tmpEepromValue;
    short Minute = 0, Second = 0;
    String TimeStr, DateStr;
    String BandTime1, BandTime2;
	String ONOFFStr[] = {"ON", "OFF"};
	ReadMemory(SWITCH_PIR_ADDR, 1, &StatePir);
    // Pulire LCD
    ClearLCD();
    LCDPrintString(ONE, CENTER_ALIGN, "General Info");
    LCDPrintString(TWO, CENTER_ALIGN, "Press Ok/Exit");
    LCDPrintString(THREE, CENTER_ALIGN, "to exit");
    delay(DELAY_MESSAGE_MENU);
    ClearLCD();
    LedCtrl(MESSAGE_POS);
    ReadMemory(EepromTab[PIR_STATE].eeprom_par_value, EepromTab[PIR_STATE].eeprom_par_numReg, &EepromTab[PIR_STATE].eeprom_par_value);
    while(!ExitInfo)
    {
        switch(Page)
        {
            case LIGHT_DELAY_INFO:
                LCDPrintString(ONE, CENTER_ALIGN, "The delay for");
                LCDPrintString(TWO, CENTER_ALIGN, "the light is:");
                ReadMemory(NUM_REG_ADDR, 1, &numReg);
                ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
                if(EepromTab[DELAY_AMOUNT].eeprom_par_value <= 60)
                {
                    tmpEepromValue = String(EepromTab[Page].eeprom_par_value);
                    tmpEepromValue = String(tmpEepromValue + "sec");
                    LCDPrintString(THREE, CENTER_ALIGN, tmpEepromValue);
                }
                else
                {
                    Minute = EepromTab[DELAY_AMOUNT].eeprom_par_value / 60;
                    Second = EepromTab[DELAY_AMOUNT].eeprom_par_value % 60;
                    if(Second < 10)
                    {
                        tmpEepromValue = String(Minute) + ":0" + String(Second) + "min";
                    }
                    else
                    tmpEepromValue = String(Minute) + ":" + String(Second) + "min";
                    LCDPrintString(THREE, CENTER_ALIGN, tmpEepromValue);
                }
                break;
            case PIR_STATE_MENU_INFO:
                LCDPrintString(ONE, CENTER_ALIGN, "The state of");
                LCDPrintString(TWO, CENTER_ALIGN, "the PIR is:");
                LCDPrintString(THREE, CENTER_ALIGN, ONOFFStr[StatePir]);
                break;
            case TIME_BANDS_INFO:
                LCDPrintString(ONE, CENTER_ALIGN, "The band is:");
                if(Flags.IsBandSetted)
                {
                    LCDPrintString(TWO, CENTER_ALIGN, "setted");
                    BandTime1 = String(Band.InitHour) + ":";
                    BandTime2 = String(Band.EndHour) + ":";
                    if(Band.InitMinute < 10)
                        BandTime1 += "0" + String(Band.InitMinute);
                    else
                        BandTime1 += String(Band.InitMinute);
                    if(Band.EndMinute < 10)
                        BandTime2 += "0" + String(Band.EndMinute);
                    else
                        BandTime2 += String(Band.EndMinute);

                    BandTime1 = BandTime1 + "  " + BandTime2;
                    LCDPrintString(THREE, CENTER_ALIGN, BandTime1);
                }
                else
                {
                    LCDPrintString(TWO, CENTER_ALIGN, "not setted");
                }
                break;
            case VERSION_DATA_INFO:
                LCDPrintString(ONE, CENTER_ALIGN, "Version Number:");
                LCDPrintString(TWO, CENTER_ALIGN, VERSION_NUMBER);
                LCDPrintString(THREE, CENTER_ALIGN, "Release Date:");
                LCDPrintString(FOUR, CENTER_ALIGN, VERSION_DATA);
                break;
            default:
                break;
        }
        ButtonPress = CheckButtons();
        switch (ButtonPress)
        {
            case UP:
            case DOWN:
            default:
                break;
            case OK_EXIT:
                ExitInfo = true;
        }
        TimerScrollInfo--;
        if(TimerScrollInfo == 0)
        {
            TimerScrollInfo = INFO_SCROLL_DELAY_TIMER;
            Page++;
            if(Page > MAX_INFO_ITEMS - 1)
                Page = LIGHT_DELAY_INFO;
            ClearLCD();
        }
        delay(WHILE_LOOP_DELAY);
        //ResetWD();
    }
    LedCtrl(MENU_POS);
    ClearLCD();
    return true;
}

bool ChangeTimeBands()
{
    bool BandSetted = false;
    ClearLCD();
    LCDPrintString(ONE, CENTER_ALIGN, "Set the");
    LCDPrintString(TWO, CENTER_ALIGN, "time band to");
    LCDPrintString(THREE, CENTER_ALIGN, "turn off");
    LCDPrintString(FOUR, CENTER_ALIGN, "the sensor");
    delay(DELAY_MESSAGE_MENU+800);
    ClearLCD();
    while(!BandSetted)
    {
        if(SetTimeBand())
        {
            if(IsBandCorrect())
            {
                Flags.IsBandSetted = true;
                BandSetted = true;
            }
        }
        else
        {
            LedCtrl(MESSAGE_POS);
            ClearLCD();
            LCDPrintString(ONE, CENTER_ALIGN, "Error!");
            LCDPrintString(TWO, CENTER_ALIGN, "Repeat the");
            LCDPrintString(THREE, CENTER_ALIGN, "procedure");
            delay(DELAY_MESSAGE_MENU);
            ClearLCD();
            Flags.IsBandSetted = false;
            BandSetted = false;
            LedCtrl(MENU_POS);
        }
        //ResetWD();
    }

    return true;
}

bool ResetAll()
{

    bool Reset = false;
    ClearLCD();
    LCDPrintString(TWO, CENTER_ALIGN, "Reset to");
    LCDPrintString(THREE, CENTER_ALIGN, "default values?");
    Reset = CheckYesNo();
    if(Reset)
    {
        ClearLCD();
        LCDPrintString(TWO, CENTER_ALIGN, "Reset in progress");
        LCDPrintString(THREE, CENTER_ALIGN, "wait...");
        WriteMemory(RESET_DEFAULT_ADDR, RESET);
        EnableWD(WDTO_2S);
        while(1)
        {
			LedCtrl(NO_LED);
			delay(2);
			LedCtrl(ALL_LED);
			delay(8);
        }
    }
    else
    {
        EepromUpdate(RESET_DEFAULT_ADDR, NO_RESET);
    }
}


bool CheckYesNo()
{
	bool Exit = false, Choice = false;
	String YesNo[] = {"Yes", "No"};
	short ButtonPress = NOPRESS;
	ANSWER_TYPE YesNoChoice = NO;
	while(!Exit)
	{
		LCDPrintString(FOUR, CENTER_ALIGN, YesNo[YesNoChoice]);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case UP:
				BlinkLed(YELLOW_LED);
				if(YesNoChoice == YES)
					YesNoChoice = NO;
				else
					YesNoChoice = YES;
				ClearLCDLine(FOUR);
				break;
			case DOWN:
				BlinkLed(YELLOW_LED);
				if(YesNoChoice == YES)
					YesNoChoice = NO;
				else
					YesNoChoice = YES;
					ClearLCDLine(FOUR);
				break;
			case OK_EXIT:
				BlinkLed(YELLOW_LED);
				if(YesNoChoice == YES)
				{
					Exit = true;
					Choice = true;
				}
				else
				{
					Exit = true;
					Choice = false;
				}
				break;
			default:
				break;
		}
		ButtonPress = NOPRESS;
		delay(WHILE_LOOP_DELAY);
        //ResetWD();
	}
	return Choice;
}
