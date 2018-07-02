#ifndef MENU_FUNC_H
#define MENU_FUNC_H

#include <Arduino.h>

#define DELAY_MESSAGE_MENU  1200

typedef enum
{
	LIGHT_DELAY = 0,
	PIR_STATE_MENU,
	SHOW_INFO,
	MANUAL_STATE,
	TIME_BANDS,
	RESET_MENU,
	MAX_MENU_ITEMS
}MENU_ITEM;

typedef enum
{
	LIGHT_DELAY_INFO = 0,
	PIR_STATE_MENU_INFO,
	TIME_BANDS_INFO,
	VERSION_DATA_INFO,
	MAX_INFO_ITEMS
}INFO_ITEM;

enum
{
	ON = 0,
	OFF
};

typedef enum
{
	YES = 0,
	NO
} ANSWER_TYPE;

enum
{
	RESET = 0,
	NO_RESET
};

void MainScreen(void);
void MainSetup(void);
void gestionePIR(short ActivePIR);
void GesLight(short LightState);
void ManualScreen(void);
bool ManualState(void);
bool ChangeTimeBands(void);
bool InfoScroll(void);
bool SwichState(void);
bool ChangeValue(void);
bool ResetAll(void);
bool CheckYesNo(void);

#endif
