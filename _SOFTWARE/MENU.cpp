#include <LiquidCrystal.h>
#include <RotaryEncoder.h>

LiquidCrystal lcd(12,11,5,4,3,2);
RotaryEncoder encoder;

char MENU[5][5][8];
volatile uint8_t m;

ISR(TIMER2_OVF_vect)
{
	TCNT -= 250; // 1000 Hz
	encoder.service();
}

void drawText(void)
{
	//clearLCD()
	lcd.print(MENU[((m-m%10)/10)][m%10]);
	lcd.print(MENU[((m-m%10)/10)][(m%10)+1]);
}

void buildMenu(void)
{
	for (int i=0;i<5;i++)
		strcpy(MENU[i][0], "<< back");

	strcpy(MENU[0][1], "Status");
	strcpy(MENU[0][2], "Light");
	strcpy(MENU[0][3], "Water");
	strcpy(MENU[0][4], "System");

	strcpy(MENU[1][1], "PH-Value");
	strcpy(MENU[1][2], "EL-Value");
	strcpy(MENU[1][3], "Tempera.");
	strcpy(MENU[1][4], "Humidit.");

	strcpy(MENU[2][1], "Interval");

	strcpy(MENU[3][1], "Amount");
	strcpy(MENU[3][2], "Interval");
	strcpy(MENU[3][3], "Fert.Amt");
	strcpy(MENU[3][4], "Humidit.");	

	strcpy(MENU[4][1], "Time");
	strcpy(MENU[4][2], "ID");
	strcpy(MENU[4][3], "Channel");
	strcpy(MENU[4][4], "Reset");	
}

void setup()
{
	buildMenu();
	lcd.begin(8,2);
	
	encoder.init();
	
	//init Timer2
	TCCR2B = (1<<CS22); //clk=F_CPU/64
	TCNT2  = 0x00;
	TIMSK2 |= (1<<TOIE2); //enable overflow interupt
	sei(); //enable interupts

	drawText();
}

void loop()
{
	int8_t move, press;

	if(move||press)
	{
		if(press == SW_PRESSED)
		{
			if ((m%10)==0 && m>0) m-=10; //if item 0 back is selected go back
			else m=m*10;
		}
		else if(press == SW_PRESSEDLONG)
		{
			m=m-10-(m%10);
		}

		if(move)
		{
			m+= move; //Move Menu Item
		}
		drawText();
	}
}
		}
	}
}