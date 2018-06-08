#include "PIR_LCD_2.h"
#include "EEPROM_Ard.h"

#include "MenuFunc.h"
#include "LCDLib.h"
#include "TimeLib.h"

#define DELAY_MESSAGE_MENU  1200

extern EEPROM_ITEM EepromTab[];
extern short MainMenuPage;
extern FLAGS  Flags;

static const String OnOff[2] = {"On", "Off"};

static const short TabDays4Month[] =
{
    31,
    28,
    31,
    30,
    31,
    30,
    31,
    31,
    30,
    31,
    30,
    31,
};

CREATE_MENU MainSetupItems[] =
{
  {"Change light delay", ChangeValue		},
  {"Change PIR state"  , SwichState 		},
  {"Show Info"         , InfoScroll         },
  {"Manual State"	   , SwichState 		},
  {"Change Time Bands" , ChangeTimeBands    },
};

void MainScreen()
{
    short ButtonPress = NOPRESS, PirState = EepromTab[PIR_STATE].eeprom_par_value;
    bool ShowMsg = false, EnterSetup = false;
    LCDDisplayOff();
    ClearLCD();
    while(!EnterSetup)
    {
        ButtonPress = CheckButtons();
        switch (ButtonPress)
        {
            case BUTTONUP:
            case BUTTONDOWN:
            ShowMsg = true;
            break;
            case BUTTONSET:
            EnterSetup = true;
            break;
        }
        ButtonPress = NOPRESS;
        if(ShowMsg)
        {
            ClearLCD();
            LCDDisplayOn();
            LCDPrintString(1,CENTER_ALIGN,"Press on Enter/Ok");
            LCDPrintString(2,CENTER_ALIGN,"for enter in");
            LCDPrintString(3,CENTER_ALIGN,"Main Setup");
            delay(DELAY_MESSAGE_MENU);
            ClearLCD();
            LCDPrintString(1,CENTER_ALIGN,"After the display");
            LCDPrintString(2,CENTER_ALIGN,"is turned off");
            delay(DELAY_MESSAGE_MENU);
            LCDDisplayOff();
            ClearLCD();
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

void gestionePIR(short ActivePIR)
{
    short StatePir = LOW, TimeDelay = 0, numReg = 0;
    if(ActivePIR == TURN_ON)
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


void MainSetup()
{
    bool ExitSetup = false;
    short Page = MIN_MENU_PAGES;
    short ButtonPress;

    OFF(RED_LED);
    OFF(GREEN_LED);
    ON(BLUE_LED);
    LCDDisplayOn();


    // Pulire LCD
    ClearLCD();
    LCDPrintString(ONE,CENTER_ALIGN,"Press Up or Down");
    LCDPrintString(TWO,CENTER_ALIGN,"to scroll the menu");
    LCDPrintString(FOUR,CENTER_ALIGN,"Press Ok to enter");

    delay(DELAY_MESSAGE_MENU);
    ClearLCD();
    while(!ExitSetup)
    {
        ButtonPress = ChekButtons();
        delay(BUTTON_PRESS_DELAY);
        // Pulire LCD

        LCDPrintString(TWO, CENTER_ALIGN, MainSetupItems[Page].nameMenu);

        switch(ButtonPress)
        {
            case UP:
                BlinkLed(YELLOW_LED); // blink giallo
                Page++;
                if(Page >= MAX_MENU_PAGES)
                {
                    Page = MIN_MENU_PAGES;
                }
                ClearLCDLine(TWO);
                break;
            case DOWN:
            BlinkLed(YELLOW_LED); // blink giallo
                Page--;
                if(Page < MIN_MENU_PAGES)
                {
                    Page = MAX_MENU_PAGES-1;
                }
                ClearLCDLine(TWO);
                break;
            case OK_EXIT:
                BlinkLed(YELLOW_LED); // blink giallo
                MainMenuPage = Page;
                ExitSetup = MainSetupItems[Page].MenuFunc();
                ClearLCD();
                break;
            default:
                break;
        }
    }
    OFF(BLUE_LED);
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
        ButtonPress = ChekButtons();
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
    switch(MainMenuPage)
    {
        case SWITCH_STATE:
        EepromItem = PIR_STATE;
        break;
        case MANUAL_STATE:
        EepromItem = MANUAL_LIGHT;
        break;
        default:
        EepromItem = PIR_STATE;
        break;
    }
    ReadMemory(EepromTab[EepromItem].eeprom_par_value, EepromTab[EepromItem].eeprom_par_numReg, &EepromTab[EepromItem].eeprom_par_value);
    short OldSwitch = EepromTab[EepromItem].eeprom_par_value;
    short SwitchOnOff = TURN_OFF;

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
        ButtonPress = ChekButtons();
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
            BlinkLed(YELLOW_LED); // blink giallo
            SwitchOnOff++;
            if(SwitchOnOff > 1)
            SwitchOnOff = TURN_ON;
            ClearLCD();
            break;
            case DOWN:
            BlinkLed(YELLOW_LED); // blink giallo
            SwitchOnOff--;
            if(SwitchOnOff < 0)
            SwitchOnOff = TURN_OFF;
            ClearLCD();
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
            if(MainMenuPage == MANUAL_STATE && SwitchOnOff == TURN_ON)
            {
                ON(LIGHT_SWITCH);
                Flags.ManualState = true;
            }
            else if(MainMenuPage == MANUAL_STATE && SwitchOnOff == TURN_OFF)
            {
                OFF(LIGHT_SWITCH);
                Flags.ManualState = false;
            }
            else
            OFF(LIGHT_SWITCH);
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



bool ChangeDateTime(short WichBand)
{
    short ButtonPress;
    bool ExitDateTime = false;
    TIME_DATE_FORMAT ChangedTime;
    // DATE_FORMAT ChangedDate;
    short   ChangeState = CHANGE_HOUR;
    ChangedTime = PresentTime;
    // ChangedDate = PresentDate;
    ON(BLUE_LED);
    ON(RED_LED);
    // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di
    // premere su setup per dare l'ok
    ClearLCD();
    // CAMBIA ORE
    while(!ExitDateTime)
    {
        ButtonPress = ChekButtons();
        delay(BUTTON_PRESS_DELAY);
        switch(ChangeState)
        {
            case CHANGE_HOUR:

            // Pulire LCD
            LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
            LCDPrintString(1,CENTER_ALIGN,"to change the hour");
            LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
            LCDPrintValue(3, CENTER_ALIGN, ChangedTime.hour);

            // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di
            // premere su setup per dare l'ok
            switch(ButtonPress)
            {
                case UP:
                BlinkLed(YELLOW_LED); // blink giallo
                if(ChangedTime.hour == 23)
                ChangedTime.hour = BAND_INVALID_VALUE;
                else if(ChangedTime.hour == BAND_INVALID_VALUE)
                ChangedTime.hour = 0;
                else
                ChangedTime.hour++;
                ClearLCD();
                break;
                case DOWN:
                BlinkLed(YELLOW_LED); // blink giallo
                if(ChangedTime.hour == 0)
                ChangedTime.hour = BAND_INVALID_VALUE;
                else if(ChangedTime.hour == BAND_INVALID_VALUE)
                ChangedTime.hour = 23;
                else
                ChangedTime.hour--;
                ClearLCD();
                break;
                case OK_EXIT:
                BlinkLed(YELLOW_LED); // blink giallo
                // Scrivere su LCD "Valori salvati"
                ClearLCD();
                if(ChangedTime.hour != PresentTime.hour && ChangedTime.hour != BAND_INVALID_VALUE)
                {
                    LCDPrintString(1, CENTER_ALIGN, "Saved!");
                    // scrivere il nuovo orario
                    if(WichBand == 1)
                    Band_1.hour = ChangedTime.hour;
                    else
                    Band_2.hour = ChangedTime.hour;
                }
                else if(ChangedTime.hour != PresentTime.hour && ChangedTime.hour == BAND_INVALID_VALUE)
                {
                    LCDPrintString(0, CENTER_ALIGN, "Saved!");
                    LCDPrintString(1, CENTER_ALIGN, "Invalidated value");
                    LCDPrintString(2, CENTER_ALIGN, "This will turn OFF");
                    LCDPrintString(3, CENTER_ALIGN, "the sensor");
                    delay(DELAY_MESSAGE_MENU);
                    // scrivere il nuovo orario
                    if(WichBand == 1)
                    Band_1.hour = ChangedTime.hour;
                    else
                    Band_2.hour = ChangedTime.hour;
                }
                else
                {
                    LCDPrintString(1, CENTER_ALIGN, "Unchanged");
                    if(WichBand == 1)
                    Band_1.hour = ChangedTime.hour;
                    else
                    Band_2.hour = ChangedTime.hour;
                }
                ChangeState = CHANGE_MINUTE;
                delay(DELAY_MESSAGE_MENU);
                ClearLCD();
                break;
                default:
                break;
            }
            break;

            //CAMBIA MINUTI
            case CHANGE_MINUTE:
            // Pulire LCD
            LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
            LCDPrintString(1,CENTER_ALIGN,"to change minutes");
            LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
            LCDPrintValue(3, CENTER_ALIGN, ChangedTime.minute);
            // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di
            // premere su setup per dare l'ok
            switch(ButtonPress)
            {
                case UP:
                BlinkLed(YELLOW_LED); // blink giallo
                if(ChangedTime.minute == 59)
                ChangedTime.minute = BAND_INVALID_VALUE;
                else if(ChangedTime.minute == BAND_INVALID_VALUE)
                ChangedTime.minute = 0;
                else
                ChangedTime.minute++;
                ClearLCD();
                break;
                case DOWN:
                BlinkLed(YELLOW_LED); // blink giallo
                if(ChangedTime.minute == 0)
                ChangedTime.minute = BAND_INVALID_VALUE;
                else if(ChangedTime.minute == BAND_INVALID_VALUE)
                ChangedTime.minute = 59;
                else
                ChangedTime.minute--;
                ClearLCD();
                break;
                case OK_EXIT:
                BlinkLed(YELLOW_LED); // blink giallo
                // Scrivere su LCD "Valori salvati"
                ClearLCD();
                if(ChangedTime.minute != PresentTime.minute && ChangedTime.minute != BAND_INVALID_VALUE)
                {
                    LCDPrintString(1, CENTER_ALIGN, "Saved!");
                    // scrivere il nuovo orario
                    if(WichBand == 1)
                    Band_1.minute = ChangedTime.minute;
                    else
                    Band_2.minute = ChangedTime.minute;
                }
                else if(ChangedTime.minute != PresentTime.minute && ChangedTime.minute == BAND_INVALID_VALUE)
                {
                    LCDPrintString(0, CENTER_ALIGN, "Saved!");
                    LCDPrintString(1, CENTER_ALIGN, "Invalidated value");
                    LCDPrintString(2, CENTER_ALIGN, "This will turn OFF");
                    LCDPrintString(3, CENTER_ALIGN, "the sensor");
                    delay(DELAY_MESSAGE_MENU);
                    // scrivere il nuovo orario
                    if(WichBand == 1)
                    Band_1.minute = ChangedTime.minute;
                    else
                    Band_2.minute = ChangedTime.minute;
                }
                else
                {
                    LCDPrintString(1, CENTER_ALIGN, "Unchanged");
                    if(WichBand == 1)
                    Band_1.minute = ChangedTime.minute;
                    else
                    Band_2.minute = ChangedTime.minute;
                }
                delay(DELAY_MESSAGE_MENU);
                ChangeState = CHANGE_DAY;
                ClearLCD();
                break;

                default:
                break;
            }
            break;


            // CAMBIA GIORNO
            case CHANGE_DAY:

            // Pulire LCD
            LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
            LCDPrintString(1,CENTER_ALIGN,"to change the day");
            LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
            LCDPrintValue(3, CENTER_ALIGN, ChangedTime.day);
            // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di
            // premere su setup per dare l'ok
            switch(ButtonPress)
            {
                case UP:
                BlinkLed(YELLOW_LED); // blink giallo
                if(ChangedTime.day == TabDays4Month[PresentTime.month])
                ChangedTime.day  = BAND_INVALID_VALUE;
                else if(ChangedTime.day == BAND_INVALID_VALUE)
                ChangedTime.day  = 1;
                else
                ChangedTime.day++;
                ClearLCD();
                break;
                case DOWN:
                BlinkLed(YELLOW_LED); // blink giallo
                if(ChangedTime.day == 1)
                ChangedTime.day  = BAND_INVALID_VALUE;
                else if(ChangedTime.day == BAND_INVALID_VALUE)
                ChangedTime.day = TabDays4Month[PresentTime.month];
                else
                ChangedTime.day--;
                ClearLCD();
                break;
                case OK_EXIT:
                BlinkLed(YELLOW_LED); // blink giallo
                // Scrivere su LCD "Valori salvati"
                ClearLCD();
                if(ChangedTime.day != PresentTime.day && ChangedTime.day != BAND_INVALID_VALUE)
                {
                    LCDPrintString(1, CENTER_ALIGN, "Saved!");
                    // scrivere il nuovo orario
                    if(WichBand == 1)
                    Band_1.day = ChangedTime.day;
                    else
                    Band_2.day = ChangedTime.day;
                }
                else if(ChangedTime.day != PresentTime.day && ChangedTime.day == BAND_INVALID_VALUE)
                {
                    LCDPrintString(0, CENTER_ALIGN, "Saved!");
                    LCDPrintString(1, CENTER_ALIGN, "Invalidated value");
                    LCDPrintString(2, CENTER_ALIGN, "This will turn OFF");
                    LCDPrintString(3, CENTER_ALIGN, "the sensor");
                    delay(DELAY_MESSAGE_MENU);
                    if(WichBand == 1)
                    Band_1.day = ChangedTime.day;
                    else
                    Band_2.day = ChangedTime.day;
                }
                else
                {
                    LCDPrintString(1, CENTER_ALIGN, "Unchanged");
                    if(WichBand == 1)
                    Band_1.day = ChangedTime.day;
                    else
                    Band_2.day = ChangedTime.day;
                }
                delay(DELAY_MESSAGE_MENU);
                ChangeState = CHANGE_MONTH;
                ClearLCD();
                break;
                default:
                break;
            }
            break;


            // CAMBIA MESE
            case CHANGE_MONTH:
            // Pulire LCD
            LCDPrintString(0,CENTER_ALIGN,"Press Up or Down");
            LCDPrintString(1,CENTER_ALIGN,"to change the month");
            LCDPrintString(2,CENTER_ALIGN,"Press Ok to exit");
            LCDPrintValue(3, CENTER_ALIGN, ChangedTime.month);
            // Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di
            // premere su setup per dare l'ok
            switch(ButtonPress)
            {
                case UP:
                BlinkLed(YELLOW_LED); // blink giallo

                if(ChangedTime.month == 12)
                ChangedTime.month  = BAND_INVALID_VALUE;
                else if(ChangedTime.month == BAND_INVALID_VALUE)
                ChangedTime.month = 1;
                else
                ChangedTime.month++;
                ClearLCD();
                break;
                case DOWN:
                BlinkLed(YELLOW_LED); // blink giallo
                if(ChangedTime.month == 1)
                ChangedTime.month  = BAND_INVALID_VALUE;
                else if(ChangedTime.month == BAND_INVALID_VALUE)
                ChangedTime.month = 12;
                else
                ChangedTime.month--;
                ClearLCD();
                break;
                case OK_EXIT:
                BlinkLed(YELLOW_LED); // blink giallo
                // Scrivere su LCD "Valori salvati"
                ClearLCD();
                if(ChangedTime.month != PresentTime.month && ChangedTime.month != BAND_INVALID_VALUE)
                {
                    LCDPrintString(1, CENTER_ALIGN, "Saved!");
                    // scrivere il nuovo orario
                    if(WichBand == 1)
                    Band_1.month = ChangedTime.month;
                    else
                    Band_2.month = ChangedTime.month;
                }
                else if(ChangedTime.month != PresentTime.month && ChangedTime.month == BAND_INVALID_VALUE)
                {
                    LCDPrintString(0, CENTER_ALIGN, "Saved!");
                    LCDPrintString(1, CENTER_ALIGN, "Invalidated value");
                    LCDPrintString(2, CENTER_ALIGN, "This will turn OFF");
                    LCDPrintString(3, CENTER_ALIGN, "the sensor");
                    delay(DELAY_MESSAGE_MENU);
                    // scrivere il nuovo orario
                    if(WichBand == 1)
                    Band_1.month = ChangedTime.month;
                    else
                    Band_2.month = ChangedTime.month;
                }
                else
                {
                    LCDPrintString(1, CENTER_ALIGN, "Unchanged");
                    if(WichBand == 1)
                    Band_1.month = ChangedTime.month;
                    else
                    Band_2.month = ChangedTime.month;
                }

                ChangeState = EXIT;
                ClearLCD();
                break;
                default:
                break;
            }
            break;
            case EXIT:
            ExitDateTime = true;
            break;
            default:
            ChangeState = CHANGE_HOUR;
            break;
        }
    }
    return ExitDateTime;
}


bool InfoScroll()
{
    short ButtonPress;
    bool ExitInfo = false;
    short Page = MIN_INFO_PAGES;
    short numReg;
    String tmpEepromValue;
    short Minute = 0, Second = 0;
    #ifdef RTC_INSERTED
    String TimeStr, DateStr;
    #endif

    OFF(RED_LED);
    ON(GREEN_LED);
    ON(BLUE_LED);
    LCDDisplayOn();

    // Pulire LCD
    ClearLCD();
    LCDPrintString(1,CENTER_ALIGN,"Press Up, Down");
    LCDPrintString(2,CENTER_ALIGN,"to scroll the info");
    LCDPrintString(3,CENTER_ALIGN,"Press Ok to exit");

    delay(DELAY_MESSAGE_MENU);
    ClearLCD();

    return !ExitInfo;
}

bool ChangeTimeBands()
{
    ClearLCD();
    LCDDisplayOn();

    return true;
}
