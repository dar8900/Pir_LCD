#include "PIR_LCD_2.h"
#include "EEPROM_Ard.h"

#include "MenuFunc.h"
#include "LCDLib.h"
#include "TimeLib.h"
#include "Band.h"

#define LCD_MANUAL_DELAY_TIMER  3000
#define INFO_SCROLL_DELAY_TIMER  66

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
};

void MainScreen()
{
    short ButtonPress = NOPRESS, PirState = EepromTab[PIR_STATE].eeprom_par_value;
    bool ShowMsg = false, EnterSetup = false;
    if(Flags.Backlight)
    {
        ClearLCD();
        LCDPrintString(1,CENTER_ALIGN,"Press on Enter/Ok");
        LCDPrintString(2,CENTER_ALIGN,"for enter in");
        LCDPrintString(3,CENTER_ALIGN,"Main Setup");
        delay(DELAY_MESSAGE_MENU);
        ClearLCD();
        LCDPrintString(1,CENTER_ALIGN,"After the display");
        LCDPrintString(2,CENTER_ALIGN,"is turned off");
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
                LCDPrintString(1,CENTER_ALIGN,"Press on Enter/Ok");
                LCDPrintString(2,CENTER_ALIGN,"for enter in");
                LCDPrintString(3,CENTER_ALIGN,"Main Setup");
                delay(DELAY_MESSAGE_MENU);
                ClearLCD();
                LCDPrintString(1,CENTER_ALIGN,"After the display");
                LCDPrintString(2,CENTER_ALIGN,"is turned off");
                delay(DELAY_MESSAGE_MENU);
                if(Flags.Backlight)
                {
                    LCDDisplayOff();
                    Flags.Backlight = false;
                }
                ClearLCD();
                ShowMsg = false;
            }
            gestionePIR(PirState);
            delay(WHILE_LOOP_DELAY);
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
    }
}

void gestionePIR(short ActivePIR)
{
    short StatePir = LOW, TimeDelay = 0, numReg = 0;
    if(ActivePIR == ON)
    {
        StatePir = READ(PIR_INPUT);
        if(StatePir == HIGH)
        {
            ReadMemory(NUM_REG_ADDR , 1, &numReg);
            ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
            TimeDelay = EepromTab[DELAY_AMOUNT].eeprom_par_value;
            ON(LIGHT_SWITCH);
            LcdTimeWrite(TimeDelay);
            OFF(LIGHT_SWITCH);
        }
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
    short ButtonPress = NOPRESS, LightState = OFF, OldLightState = 0, TimerManualExit = 300, TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
    bool ExitManualState = false, OldValues = false;;
    ReadMemory(LIGHT_STATE_ADDR, 1, &OldLightState);
    ClearLCD();
    LCDDisplayOn();
    Flags.Backlight = true;
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
                    break;
                }
                else
                    TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
                if(LightState == OFF)
                    LightState = ON;
                else
                    LightState = OFF;
                ClearLCDLine(THREE);
                break;
            case DOWN:
                BlinkLed(YELLOW_LED);
                if(TimerDisplayOn == 0)
                {
                    TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
                    break;
                }
                else
                    TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
                if(LightState == OFF)
                    LightState = ON;
                else
                    LightState = OFF;
                ClearLCDLine(THREE);
                break;
            case OK_EXIT:
                BlinkLed(YELLOW_LED);
                if(TimerDisplayOn == 0)
                {
                    TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
                    break;
                }
                else
                    TimerDisplayOn = LCD_MANUAL_DELAY_TIMER;
                ReadMemory(LIGHT_STATE_ADDR, 1, &OldLightState);
                if(LightState != OldLightState)
                    GesLight(LightState);
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
                break;
        }
        if(ButtonPress == EXIT_MANUAL)
        {
            if(!Flags.Backlight)
            {
                LCDDisplayOn();
                Flags.Backlight = true;
            }
            TimerManualExit--;
            if(TimerManualExit == 0)
            {
                ClearLCD();
                LCDPrintString(TWO, CENTER_ALIGN, "Exit from");
                LCDPrintString(THREE, CENTER_ALIGN, "manual state");
                delay(DELAY_MESSAGE_MENU);
                ClearLCD();
                Flags.ManualState = false;
                ExitManualState = true;
                WriteMemory(MANUAL_STATE_ADDR, OFF);
            }
        }
        TimerDisplayOn--;
        if(TimerDisplayOn == 0 && ButtonPress != EXIT_MANUAL)
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
    }
    return true;
}


void MainSetup()
{
    bool ExitSetup = false;
    short MenuItem = LIGHT_DELAY;
    short ButtonPress;

    OFF(RED_LED);
    OFF(GREEN_LED);
    ON(BLUE_LED);
    LCDDisplayOn();
    Flags.Backlight = true;

    // Pulire LCD
    ClearLCD();
    LCDPrintString(ONE,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(TWO,CENTER_ALIGN,"to scroll the menu");
    LCDPrintString(FOUR,CENTER_ALIGN,"Press Ok to enter");

    delay(DELAY_MESSAGE_MENU);
    ClearLCD();
    while(!ExitSetup)
    {
        ButtonPress = CheckButtons();
        delay(BUTTON_PRESS_DELAY);
        // Pulire LCD

        LCDPrintString(TWO, CENTER_ALIGN, MainSetupItems[MenuItem].nameMenu);

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
    }
    if(ExitSetup)
    {
        MainSetupItems[MenuItem].MenuFunc();
    }
    OFF(BLUE_LED);
}

bool ManualState()
{
    short ButtonPress = NOPRESS, SwitchOnOff = OFF;
    bool ExitSetManual = false;
    ClearLCD();
    LCDPrintString(ONE, CENTER_ALIGN, "Set the");
    LCDPrintString(TWO, CENTER_ALIGN, "manual state:");
    while(!ExitSetManual)
    {
        LCDPrintString(THREE, CENTER_ALIGN, OnOff[SwitchOnOff]);
        ButtonPress = CheckButtons();
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
                BlinkLed(YELLOW_LED);
                ClearLCDLine(THREE);
                LCDPrintString(THREE, CENTER_ALIGN, "Setted!");
                ClearLCD();
                if(SwitchOnOff == ON)
                {
                    Flags.ManualState = true;
                    WriteMemory(MANUAL_STATE_ADDR, ON);
                }
                else
                {
                    Flags.ManualState = false;
                    WriteMemory(MANUAL_STATE_ADDR, OFF);
                }
                ExitSetManual = true;
                break;
            default:
                break;
        }
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
    LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(1,CENTER_ALIGN,"to change the value");
    LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");

    delay(DELAY_MESSAGE_MENU);
    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di
    // premere su setup per dare l'ok
    ClearLCD();
    while(!OkButton)
    {
        ButtonPress = CheckButtons();
        delay(BUTTON_PRESS_DELAY);
        // Pulire LCD

        if(ChangeDelayAmount <= 60)
        {
            LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
            LCDPrintString(1,CENTER_ALIGN,"to change the value:");
            ClearLCDLine(2);
            LCDPrintValue(2,8, ChangeDelayAmount);
            LCDPrintString(2,11,"sec");
            LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
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
            LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
            LCDPrintString(1,CENTER_ALIGN,"to change the value:");
            ClearLCDLine(2);
            LCDPrintString(2,CENTER_ALIGN, TimeOverSixty);
            LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
        }

        // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di
        // premere su setup per dare l'ok

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
            ClearLCD();
            break;
            case DOWN:
            BlinkLed(YELLOW_LED); // blink giallo
            ChangeDelayAmount--;
            if(ChangeDelayAmount < MIN_LIGHT_DELAY) // Raggiunti i secondi minimi per il delay nella gestione pir
            {
                ChangeDelayAmount = MAX_LIGHT_DELAY;
            }
            ClearLCD();
            break;
            case OK_EXIT:
            BlinkLed(YELLOW_LED); // blink giallo
            // Scrivere su LCD "Valori salvati"
            ClearLCD();
            if(oldDelayAmount != ChangeDelayAmount)
            {
                LCDPrintString(1,CENTER_ALIGN,"Value Saved!");
                WriteMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, ChangeDelayAmount);
                ReadMemory(NUM_REG_ADDR, 1, (short*)&numReg);
                ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &ChangeDelayAmount);
            }
            else
            {
                LCDPrintString(1,CENTER_ALIGN,"Value Unchanged");
            }
            OkButton = true;
            delay(DELAY_MESSAGE_MENU);
            break;
            default:
            break;

        }
    }
    OFF(BLUE_LED);
    ClearLCD();
    return OkButton;
}


bool SwichState()
{
    short ButtonPress;
    short EepromItem;
    bool OkSwitch = false;
    EepromItem = PIR_STATE;
    ReadMemory(EepromTab[EepromItem].eeprom_par_value, EepromTab[EepromItem].eeprom_par_numReg, &EepromTab[EepromItem].eeprom_par_value);
    short OldSwitch = EepromTab[EepromItem].eeprom_par_value;
    short SwitchOnOff = OFF;

    // Pulire LCD
    ClearLCD();
    LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(1,CENTER_ALIGN,"to change the state");
    LCDPrintString(2,CENTER_ALIGN,"On or Off");
    LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");

    delay(DELAY_MESSAGE_MENU);
    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di
    // premere su setup per dare l'ok
    ClearLCD();
    while(!OkSwitch)
    {
        ButtonPress = CheckButtons();
        delay(BUTTON_PRESS_DELAY);
        // Pulire LCD
        LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
        LCDPrintString(1,CENTER_ALIGN,"to change the state");
        ClearLCDLine(2);
        LCDPrintString(2, CENTER_ALIGN, OnOff[SwitchOnOff]);
        LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");
        // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di
        // premere su setup per dare l'ok

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
                // Scrivere su LCD "Valori salvati"
                ClearLCD();
                LCDPrintString(1, CENTER_ALIGN, "The state is");
                LCDPrintString(2, CENTER_ALIGN, OnOff[SwitchOnOff]);
                if(SwitchOnOff != OldSwitch)
                {
                    WriteMemory(EepromTab[EepromItem].eeprom_par_addr, SwitchOnOff);
                }
                OkSwitch = true;
                delay(DELAY_MESSAGE_MENU);
                break;
            default:
                break;
        }
    }
    OFF(BLUE_LED);
    ClearLCD();
    return OkSwitch;

}

bool InfoScroll()
{
    short TimerScrollInfo = INFO_SCROLL_DELAY_TIMER, ButtonPress = NOPRESS;
    bool ExitInfo = false;
    MENU_ITEM Page = LIGHT_DELAY;
    short numReg;
    String tmpEepromValue;
    short Minute = 0, Second = 0;
    String TimeStr, DateStr;
    String BandTime1, BandTime2;
    ON(BLUE_LED);
    // Pulire LCD
    ClearLCD();
    while(!ExitInfo)
    {
        switch(Page)
        {
            case LIGHT_DELAY:
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
                TimerScrollInfo--;
                if(TimerScrollInfo == 0)
                {
                    TimerScrollInfo = INFO_SCROLL_DELAY_TIMER;
                    Page = PIR_STATE_MENU;
                    ClearLCD();
                }
                break;
            case PIR_STATE_MENU:
                LCDPrintString(ONE, CENTER_ALIGN, "The state of");
                LCDPrintString(TWO, CENTER_ALIGN, "the PIR is:");
                ReadMemory(EepromTab[PIR_STATE].eeprom_par_value, EepromTab[PIR_STATE].eeprom_par_numReg, &EepromTab[PIR_STATE].eeprom_par_value);
                LCDPrintString(THREE, CENTER_ALIGN, OnOff[EepromTab[PIR_STATE].eeprom_par_value]);
                TimerScrollInfo--;
                if(TimerScrollInfo == 0)
                {
                    TimerScrollInfo = INFO_SCROLL_DELAY_TIMER;
                    Page = TIME_BANDS;
                    ClearLCD();
                }
                break;
            case TIME_BANDS:
                if(Flags.IsBandSetted)
                {
                    LCDPrintString(TWO, CENTER_ALIGN, "Setted");
                    LCDPrintString(THREE, CENTER_ALIGN, "The band is:");
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
                    LCDPrintString(FOUR, CENTER_ALIGN, BandTime1);
                }
                else
                {
                    LCDPrintString(TWO, CENTER_ALIGN, "Not setted");
                }
                TimerScrollInfo--;
                if(TimerScrollInfo == 0)
                {
                    TimerScrollInfo = INFO_SCROLL_DELAY_TIMER;
                    Page = LIGHT_DELAY;
                    ClearLCD();
                }
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
        delay(WHILE_LOOP_DELAY);
    }
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
            ClearLCD();
            LCDPrintString(ONE, CENTER_ALIGN, "Error!");
            LCDPrintString(ONE, CENTER_ALIGN, "Repeat the");
            LCDPrintString(ONE, CENTER_ALIGN, "procedure");
            delay(DELAY_MESSAGE_MENU);
            ClearLCD();
            Flags.IsBandSetted = false;
            BandSetted = false;
        }
    }

    return true;
}
