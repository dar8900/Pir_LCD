
#define MAX_LCD_ROW		3
#define MAX_LCD_COL		19

#define	MAX_LIGHT_DELAY		600 // In secondi
#define MIN_LIGHT_DELAY		 20 // In secondi

#define  OFF(pin)  digitalWrite(pin, LOW)
#define  ON(pin)   digitalWrite(pin, HIGH)

#define CENTER_ALIGN	(MAX_LCD_COL/2)
#define RIGHT_ALIGN		MAX_LCD_COL
#define LEFT_ALIGN		 0

enum
{
	BUTTON_UP = 2,
	BUTTON_DOWN,
	BUTTON_SETUP,
	RED_LED,
	GREEN_LED,
	BLUE_LED,
	YELLOW_LED
} PIN;






int SetupOk = 0;
int Increase = MIN_LIGHT_DELAY; // Imposto il valore minimo per default

//int ButtonUP = BUTTON_UP, ButtonDOWN = BUTTON_DOWN, ButtonSETUP = BUTTON_SETUP;
//int redLed = RED_LED, greenLed = GREEN_LED, blueLed = BLUE_LED, yellowLed = YELLOW_LED;

bool FlagSetup = false;

/************************ SETUP INIZIALE ******************************/

void SetupDelayInit()
{
	int timer = 80; // tempo di attesa di 5s con delay di 62ms
	int buttonUp = 0, buttonDown = 0;
	bool OkButton = false;
	int oldIncrease = Increase;
	
	ClearLCD();
	LCDPrintString(0,0,"Press on Enter/Ok");
	LCDPrintString(2,1,"for enter in");
	LCDPrintString(1,2,"initial Setup");
	LCDPrintString(1,3,"You have 5 seconds");
	// Pulire LCD
	// Scrivere su LCD che si deve premere su SetupOk per accedere entro 5s
	
	while(timer > 0)
	{
		SetupOk = digitalRead(BUTTON_SETUP);
		if(SetupOk == HIGH)
		{
			// blink giallo
			
			BlinkLed(YELLOW_LED);
			ON(BLUE_LED); // Accendo il led blu di setup			
			// Pulire LCD
			ClearLCD();
			LCDPrintString(2,0,"Press Up or Down");
			LCDPrintString(0,1,"to change the delay");
			LCDPrintString(0,3,"Press Ok to exit");
			
			// 	Resetto SetupOk
			SetupOk = LOW;
			
			delay(1000);
			// Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
			// premere su setup per dare l'ok
			
			while(1)
			{
				buttonUp = digitalRead(BUTTON_UP);
				buttonDown = digitalRead(BUTTON_DOWN);
				SetupOk = digitalRead(BUTTON_SETUP);
				
				// Pulire LCD
				ClearLCD();
				LCDPrintString(2,0,"Press Up or Down");
				LCDPrintString(0,1,"to change the delay");
				LCDPrintValue(8,2, Increase);
				LCDPrintString(0,3,"Press Ok to exit");
				// Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
				// premere su setup per dare l'ok
				
				if(buttonUp == HIGH)
				{
					BlinkLed(YELLOW_LED); // blink giallo
					if(Increase == 0)
					{
						Increase == MIN_LIGHT_DELAY;
					}
					Increase++;
					if(Increase == MAX_LIGHT_DELAY) // Raggiunti i secondi massimi per il delay nella gestione pir
					{
						Increase = MAX_LIGHT_DELAY;
					}
				}
				if(buttonDown == HIGH)
				{
					BlinkLed(YELLOW_LED); // blink giallo
					Increase--;
					if(Increase == 0 || Increase == MIN_LIGHT_DELAY) // Raggiunti i secondi minimi per il delay nella gestione pir
					{
						Increase = MIN_LIGHT_DELAY;
					}
				}
				if(SetupOk == HIGH)
				{
					BlinkLed(YELLOW_LED); // blink giallo
					// Scrivere su LCD Valori salvati
					ClearLCD();
					if(oldIncrease != Increase)
					{
						LCDPrintString(0,0,"Value Saved!");					
					}
					else
					{
						LCDPrintString(0,0,"Value Unchanged");
					}
					OkButton = true;
					delay(2000);
					lcd.noBacklight();
					// Spegnere Schermo
					break;
				}
			}
		}
		if(OkButton)
		{
			break; // Esci da SetupInit
			OFF(BLUE_LED);
		}
		delay(62);		
	}
}

/************************ SETUP RUNTIME ******************************/

void SetupDelay()
{
	int buttonUp = 0, buttonDown = 0;
	bool OkButton = false;
	int oldIncrease = Increase;
	
	OFF(RED_LED);
	OFF(GREEN_LED);
	ON(BLUE_LED);
	
	while(1)
	{
		SetupOk = digitalRead(BUTTON_SETUP);
		if(SetupOk == HIGH)
		{
			BlinkLed(YELLOW_LED); // blink giallo
			// Pulire LCD
			ClearLCD();
			LCDPrintString(2,0,"Press Up or Down");
			LCDPrintString(0,1,"to change the delay");
			LCDPrintString(0,3,"Press Ok to exit");
			
			// 	Resetto SetupOk
			SetupOk = LOW;
			
			delay(1000);
			// Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
			// premere su setup per dare l'ok
			
			while(1)
			{
				buttonUp = digitalRead(BUTTON_UP);
				buttonDown = digitalRead(BUTTON_DOWN);
				SetupOk = digitalRead(BUTTON_SETUP);
				
				// Pulire LCD
				ClearLCD();
				LCDPrintString(2,0,"Press Up or Down");
				LCDPrintString(0,1,"to change the delay:");
				LCDPrintValue(8,2, Increase);
				LCDPrintString(0,3,"Press Ok to exit");
				// Scrivere su LCD che cosa si sta aumentando e scrivere nella riga sotto centrale il valore, scriver anche di 
				// premere su setup per dare l'ok
				
				if(buttonUp == HIGH)
				{
					BlinkLed(YELLOW_LED); // blink giallo
					if(Increase == 0)
					{
						Increase == MIN_LIGHT_DELAY;
					}
					Increase++;
					if(Increase == MAX_LIGHT_DELAY) // Raggiunti i secondi massimi per il delay nella gestione pir
					{
						Increase = MAX_LIGHT_DELAY;
					}
				}
				if(buttonDown == HIGH)
				{
					BlinkLed(YELLOW_LED); // blink giallo
					Increase--;
					if(Increase == 0 || Increase == MIN_LIGHT_DELAY) // Raggiunti i secondi minimi per il delay nella gestione pir
					{
						Increase = MIN_LIGHT_DELAY;
					}
				}
				if(SetupOk == HIGH)
				{
					BlinkLed(YELLOW_LED); // blink giallo
					
					// Scrivere su LCD Valori salvati
					ClearLCD();
					if(oldIncrease != Increase)
					{
						LCDPrintString(0,0,"Value Saved!");					
					}
					else
					{
						LCDPrintString(0,0,"Value Unchanged");
					}
					OkButton = true;
					// Spegnere Schermo
					lcd.noBacklight();
					delay(2000);
					break;
				}
			}
		}
		if(OkButton)
		{
			break; // Esci dal Setup
			OFF(BLUE_LED);
		}		
	}
}





void EnterSetupButton()
{
	SetupOk = digitalRead(BUTTON_SETUP);
	if(SetupOk == HIGH)
	{
		BlinkLed(YELLOW_LED);
		SetupOk = LOW;
		FlagSetup = true;
	}
	else
	{
		FlagSetup = false;
	}
}

void InterruptFunc()
{
	int timer = 7; // 0.1s c.a (105ms)
	while(timer > 0)
	{
		timer--;
		EnterSetupButton();
		delay(15);
	}
}


void WriteHomeMsg()
{

	ClearLCD();
	lcd.backlight();
	LCDPrintString(0,0,"Press on Enter/Ok");
	LCDPrintString(2,1,"for enter in");
	LCDPrintString(5,2,"Delay Setup");
	// Pulire LCD
	// Scrivere su LCD che si deve premere su SetupOk per accedere 

}


void setup()
{
	Serial.begin(9600);
	pinMode(BUTTON_UP, INPUT);
	pinMode(BUTTON_DOWN, INPUT);
	pinMode(BUTTON_SETUP, INPUT);
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	
	SetupDelayInit();
	
}

void loop()
{
	WriteHomeMsg();
	InterruptFunc();
	if(FlagSetup)
	{
		SetupDelay();
	}
	else
	{
		gestionePir();
	}
}


void BlinkLed(int pin) // Blink del led per 5ms
{
	digitalWrite(pin, HIGH);
	delay(5);
	digitalWrite(pin, LOW);
}

// NOTA PER ME!! PER CENTRARE LA SCRITTA BISOGNA CONOSCERE LA LUNGHEZZA DELLA STRINGA, POI 
// METTERE COME VALORE INIZIALE DELLA COLONNA: (LUNGHEZZA MASSIMA LCD - LUNGHEZZA STRINGA)/2

void LCDPrintString(int row, int col, String string) // Utilizzano un oggetto di tipo LCD
{
	if(col > MAX_LCD_COL || row > MAX_LCD_ROW || string.lenght() > 20) // ???
	{
		lcd.clear();
		lcd.home();
		lcd.print("OVER DIMENSION");
		delay(2000);
		return;
	}
	switch(col)
	{
		case CENTER_ALIGN:
			col = (MAX_LCD_COL - string.lenght()) / 2;
			break;
		case RIGHT_ALIGN:
			col = MAX_LCD_COL - string.lenght();
			break;
		case LEFT_ALIGN:
			col = 0;
			break;
		default:
			lcd.home();
			break;
	}
	lcd.setCursor(col, row);
	lcd.print(string);

}

void LCDPrintValue(int row, int col, int value)
{
	String ValStr = String(value);
	if(col > MAX_LCD_COL || row > MAX_LCD_ROW || ValStr.lenght() > 20)
	{
		lcd.clear();
		lcd.home();
		lcd.print("OVER DIMENSION");
		delay(2000);
		return;
	}
	
	switch(col)
	{
		case CENTER_ALIGN:
			col = (MAX_LCD_COL - ValStr.lenght()) / 2;
			break;
		case RIGHT_ALIGN:
			col = MAX_LCD_COL - ValStr.lenght();
			break;
		case LEFT_ALIGN:
			col = 0;
			break;
		default:
			lcd.home();
			break;
	}
	lcd.setCursor(col, row);
	lcd.print(ValStr);
}

void LCDPrintLineVoid(int row)
{
	lcd.setCursor(0, row);
	lcd.print("                    ");
}



