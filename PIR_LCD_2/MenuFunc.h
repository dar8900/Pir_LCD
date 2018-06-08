#ifndef MENU_FUNC_H
#define MENU_FUNC_H

#include <Arduino.h>

enum
{
	CHANGE_HOUR = 0,
	CHANGE_MINUTE,
	CHANGE_DAY,
	CHANGE_MONTH,
	EXIT
};

enum
{
	CHANGE_BAND_1 = 0,
	CHANGE_BAND_2,
	CHEK_BAND,
	EXIT_CHANGE_BAND
};

enum
{
	BAND_1 = 1,
	BAND_2

};

void MainScreen(void);
void MainSetup(void);
void gestionePIR(short ActivePIR);
bool ChangeTimeBands(void);
bool InfoScroll(void);
bool SwichState(void);
bool ChangeValue(void);

#endif
