#include <LiquidCrystal_I2C.h> // Libreria LCD I2C

#define MAX_LCD_ROW    3
#define MAX_LCD_COL   19

#define CENTER_ALIGN  10
#define RIGHT_ALIGN    19
#define LEFT_ALIGN     0

void LCDInit(void);
void ClearLCD(void);
void LcdTimeWrite(int Time2Write);
void LCDPrintString(short row, short col, String string);
void LCDPrintValue(short row, short col, short value);
void LCDPrintLineVoid(short row);
void LCDDisplayOn(void);
void LCDDisplayOff(void);
void LCDBlink(void);
void LCDNoBlink(void);
