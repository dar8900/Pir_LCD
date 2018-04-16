#include <LiquidCrystal_I2C.h> // Libreria LCD I2C

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
