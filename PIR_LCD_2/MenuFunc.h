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
	MAX_MENU_ITEMS
}MENU_ITEM;

enum
{
	ON = 0,
	OFF
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

#endif
