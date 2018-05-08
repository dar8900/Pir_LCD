#include "LCDLib.h"
#include "PIR_LCD_2.h"
LiquidCrystal_I2C lcd_main(0x27, 20,4);


void LCDInit()
{
	lcd_main.begin();	
}

void ClearLCD()
{
    //delay(10);
    lcd_main.clear();
    //lcd_main.home();
}

void LcdTimeWrite(int Time2Write)
{
  int cnt = Time2Write;
  while(cnt != 0)
  {
    LCDPrintValue(1, CENTER_ALIGN, cnt);
    delay(1000);
    ClearLCD();
    cnt--;
  }  

  LCDPrintString(1, CENTER_ALIGN, "The light is going");
  LCDPrintString(2, CENTER_ALIGN, "to turn off");
  delay(3000);
  ClearLCD();
}

// Utilizzano un oggetto di tipo LCD
void LCDPrintString(short row, short col, String string) 
{
  if(col > MAX_LCD_COL || row > MAX_LCD_ROW || string.length() > 20) // ???
  {
	lcd_main.clear();
	if(string.length() > 20)
	{
		col = CENTER_ALIGN;
		string = "STRING TOO BIG";  		
	}
	else
	{
		col = CENTER_ALIGN;
		row = 3;
		string = "OVER DIMENSION"; 		
	}
  
  }
  switch(col)
  {
    case CENTER_ALIGN:
      col = ((MAX_LCD_COL+1) - string.length()) / 2;
      break;
    case RIGHT_ALIGN:
      col = (MAX_LCD_COL+1) - string.length();
      break;
    case LEFT_ALIGN:
      col = 0;
      break;
    default:
      lcd_main.home();
      break;
  }
  lcd_main.setCursor(col, row);
  lcd_main.print(string);

}

void LCDPrintValue(short row, short col, short value)
{
  String ValStr = String(value);
  if(col > MAX_LCD_COL || row > MAX_LCD_ROW || ValStr.length() > 20)
  {
	lcd_main.clear();
	col = CENTER_ALIGN;
	row = 3;
    ValStr = "OVER DIMENSION"; 
    return;
  }
  
  switch(col)
  {
    case CENTER_ALIGN:
      col = ((MAX_LCD_COL+1) - ValStr.length()) / 2;
      break;
    case RIGHT_ALIGN:
      col = (MAX_LCD_COL+1) - ValStr.length();
      break;
    case LEFT_ALIGN:
      col = 0;
      break;
    default:
      lcd_main.home();
      break;
  }
  lcd_main.setCursor(col, row);
  lcd_main.print(ValStr);
}

void LCDPrintLineVoid(short row)
{
  lcd_main.setCursor(0, row);
  lcd_main.print("                    ");
}

void LCDDisplayOn()
{
	lcd_main.backlight();
}

void LCDDisplayOff()
{
	lcd_main.noBacklight();
}

void LCDBlink()
{
	lcd_main.blink();
}

void LCDNoBlink()
{
	lcd_main.noBlink();
}

void LCDPrintPhrase(String Frase)
{
	short PhraseLen = Frase.length(), TotRow = 0, TotChar = 0, row = 0, Char = 0, cursor = 0;
	char Phrase[PhraseLen];
	
	if(PhraseLen > 80)
	{
		lcd.clear();
		lcd.setCursor(0,2);
		lcd.print("Phrase too BIG");
		delay(2000);
		return;
	}
	Frase.toCharArray(Phrase, PhraseLen);
	TotChar = PhraseLen;
	if(PhraseLen <= 20)
	{
		TotRow = 1;
	}
	else if(PhraseLen > 20 && PhraseLen <= 40)
	{
		TotRow = 2;
	}
	else if(PhraseLen > 40 && PhraseLen <= 60)
	{
		TotRow = 3;
	}
	else if(PhraseLen > 60 && PhraseLen <= 80)
	{
		TotRow = 4;
	}	
	for(row = 0; row < TotRow; row++)
	{
		while(Char < TotChar)
		{
			lcd.setCursor(cursor, row);
			lcd.print(Phrase[Char]);
			Char++;
			cursor++;
			if(Char % 20 == 0)
			{
				Char++;
				cursor = 0;
				break;
			}
				
		}
	}
}



