
//Function: Display: seconds, minutes, weeks, days, date, month, year. Can show :2000-2099.
//Note: DS1307 to read and write are the BCD code, the use of a swap between the decimal and BCD code.
//Platform£ºatmega64
//fuse bytes£º
//		  low byte£º0xc1
//		  high byte£º0xd9
//		  extend£º0xFF
//		  Lock byte£º0xff
//Crystal Oscillator: Internal RC oscillator 1MHz
//Time: July 27, 2012
#include<avr/io.h>
#include <macros.h>
#include <avr/wdt.h>
#define uint unsigned int 
#define uchar unsigned char
//sda=PD1;//;TWI Data transmission bit
//scl=PD0;//;TWI Clock control state flag
//RS=PC0;//define the I / O hardware interface of LCD1602
//RW=PC1;//define the I / O hardware interface 0f LCD1602 
//E=PC2; //define the I / O hardware interface 0f LCD1602 
uchar count;
uchar string3[]="Time:  :  :     ";
uchar string4[]="20  .  .  week: ";
uchar Hours=16;//Definition of hours
uchar Minutes=21;//Definition of minutes
uchar Seconds=0;//Definition of seconds

uint  Year=12;//Definition of year
uchar Month=7;//Definition of month
uchar Date=27;//Definition of date
uchar Weeks=5;//Definition of weeks 

uchar DispBuf[16]; //16 bytes of display buffer
uchar disp_tab[]={'0','1','2','3','4','5','6','7','8','9'};

#define START 		0X08   //start() has been successfully sent
#define RESTART 	0X10 //The restart() has been successfully sent
#define MT_SLA_ACK 	0X18 //Load data has been sent ack()
#define MT_SLA_NACK 0X20 //Load data has been sent nack()
#define MT_DAT_ACK 	0X28  //Data has been sent ack()
#define MT_DAT_NACK 0X30 //Data has been sent nack()
#define SL_SLA_ACK 	0X40 //Data has been loaded from the machine to send ack()
#define SL_SLA_NACK 0X48 //Data has been loaded from the machine to send nack()
#define SL_DAT_ACK 	0X50  //Slave data has been sent ack()
#define SL_DAT_NACK 0X58  //Slave data has been sent nack()

#define start() 	TWCR=((1<<TWINT)|(1<<TWSTA)|(1<<TWEN)) //Send start signal
#define restart() 	TWCR=((1<<TWINT)|(1<<TWEN)|(1<<TWSTA)) //Sent to re-start signal
#define wait()  	while(!(TWCR & (1<<TWINT))) //Waiting for the signal
#define state() 	(TWSR&0XF8) //The register TWSR state extraction
#define stop() 		TWCR=((1<<TWEN)|(1<<TWINT)|(1<<TWSTO)) //Stop signal
#define twi()		(TWCR=(1<<TWINT)|(1<<TWEN))	//Open the TWI function
#define setnoAck()	(TWCR&=~(1<<TWEA))				//Make Not Ack response

#define r_direction 0xD1 //Read the device address
#define w_direction 0xD0 //Write device address

void twi_init()  //TWI initialization
{
   TWCR |=(1<<TWEN); //Start twi function
   TWBR=0X20;  //Set the baud rate for the host mode
   TWSR=0;    //Clear Status Register
}

void writebyte(uchar temp)//A byte data write to the TWI
{
	TWDR=temp;
	TWCR=((1<<TWINT)|(1<<TWEN));
}
uchar TWI_Write(uchar addr,uchar dat) //Write data to TWI
{
   start();//start signal
   wait();//Start TWI function
   if(state()!=START)//Wait for the nack/ack() signal
   return 1;

   writebyte(w_direction);//write    Device address
   wait();
   if(state()!=MT_SLA_ACK)
   return 1;

   writebyte(addr);//write      Byte address
   wait();
   if(state()!=MT_DAT_ACK)
   return 1;

   writebyte(dat);//write     data
   wait();
   if(state()!=MT_DAT_ACK)
   return 1;

   stop();//stop signal
   return 0;
}

uchar TWI_Read(uchar addr) //Read data from TWI
{
   	uchar temp=0;
   	start();//
	wait();
	if(state()!=START) 
	return 1;			 		//ACK*/	   

	writebyte(w_direction);	//Write TWI slave address and write
	wait(); 
	if(state()!=MT_SLA_ACK) 
	return 1;				    //ACK*/

	writebyte(addr);		//Write the corresponding device register address
	wait();
	if(state()!=MT_DAT_ACK) 
	return 1;//*/

	start();	   				   	//TWI re-start
	wait();
	if (state()!=RESTART)  
	return 1;//*/

	writebyte(r_direction);	//Write TWI slave device address and read
	wait();
	if(state()!=SL_SLA_ACK)  
	return 1;				   //ACK*/

	twi();	 				   //Start the TWI read
	wait();
	if(state()!=SL_DAT_NACK) 
	return 1;					//ACK*/	

	temp=TWDR;//Read the TWI of the receive data
	stop();//
	return temp;


}
/////////////////////////////////////////
//////////////////////////////////////////
uchar Decimal_to_BCD(uchar temp)//Decimal converted to BCD code
{
	uchar a,b,c;
	a=temp;
	b=0x00;
	if(a>=10)
	{
		while(a>=10)
		{
			a=a-10;
			b=b+16;
			c=a+b;
			temp=c;
		}
	}
	return temp;
}

uchar BCD_to_Decimal(uchar temp)//BCD to decimal
{
	uchar a,b,c;
	a=temp;
	b=0x00;
	if(a>=16)
	{
		while(a>=16)
		{
			a=a-16;
			b=b+10;
			c=a+b;
			temp=c;
		}
	}
	return temp;
}
////////////////////////////////////////	
void delay_2(uint z)//0.1msDelay
{ 
	uchar x,x1;
	for(x1=0;x1<z;x1++)
	{
		for(x=0;x<114;x++);
	}
}

void delay_1()//0.1msDelay
{ 
	uchar x,x1;
	for(x1=0;x1<2;x1++)
	{
		for(x=0;x<100;x++);
	}
}
///////////LCD1602///////////////////
/////////////////////////////////////
void write_Directive(uchar command)//Write command
{
	PORTC &=~(1<<PC2);//E=0;
	PORTC &=~(1<<PC0);//RS=0;
	PORTC &=~(1<<PC1);//RW=0;	
	PORTA = command;
	PORTC |=(1<<PC2);//E=1;	
	delay_1();
	PORTC &=~(1<<PC2);//E=0;
}
void write_Data(uchar dat)//write data
{
	PORTC &=~(1<<PC2);//E=0;
	PORTC |=(1<<PC0);//RS=1;
	PORTC &=~(1<<PC1);//RW=0;
	PORTA = dat;
	PORTC |=(1<<PC2);//E=1;
	delay_1();
	PORTC &= ~(1<<PC2);//E=0;
}
void lcdinit(void)//Initialization
{
	delay_2(90);
	write_Directive(0x38);
	delay_2(30);
	write_Directive(0x38);
	delay_2(30);
	write_Directive(0x38);
	write_Directive(0x38);
	write_Directive(0x08);
	write_Directive(0x01);
	write_Directive(0x06);
	write_Directive(0x0c);		
}
void Show()//show
{ 
	write_Directive(0x80+5);//the first row of LCD1602 , The fifth address
	DispBuf[0]=disp_tab[(Hours/10)];//
	DispBuf[1]=disp_tab[(Hours%10)];//	
	write_Data(DispBuf[0]);//Ten of the Hours
	write_Data(DispBuf[1]);// Bits of the Hours

	write_Directive(0x80+8);
	DispBuf[2]=disp_tab[(Minutes/10)];//
	DispBuf[3]=disp_tab[(Minutes%10)];//	
	write_Data(DispBuf[2]);//Ten of the Minutes
	write_Data(DispBuf[3]);//Bits of the Minutes

	write_Directive(0x80+11);
	DispBuf[4]=disp_tab[(Seconds/10)];//
	DispBuf[5]=disp_tab[(Seconds%10)];//	
	write_Data(DispBuf[4]);//Ten of the Seconds
	write_Data(DispBuf[5]);//Bits of the Seconds
	
	write_Directive(0x80+0x40+2);//the second line of LCD1602, The second addresses
	DispBuf[7]=disp_tab[(Year/10)];//
	DispBuf[8]=disp_tab[(Year%10)];//
	write_Data(DispBuf[7]);//Ten of the Year
	write_Data(DispBuf[8]);//Bits of the Year
	
	write_Directive(0x80+0x40+5);//
	DispBuf[9]=disp_tab[(Month/10)];//
	DispBuf[10]=disp_tab[(Month%10)];//
	write_Data(DispBuf[9]);//Ten of the clock
	write_Data(DispBuf[10]);//Bits of the clock

	write_Directive(0x80+0x40+8);//
	DispBuf[11]=disp_tab[(Date/10)];//
	DispBuf[12]=disp_tab[(Date%10)];//
	write_Data(DispBuf[11]);//Ten of the Date
	write_Data(DispBuf[12]);//Bits of the Date

	write_Directive(0x80+0x40+15);//
	DispBuf[14]=disp_tab[Weeks];//
	write_Data(DispBuf[14]);//week
}

void write_ds1307()//write ds1307
{
	//Write a data to the device: (0xd0 is device write address; 0x01 ~ 06 are seconds to years register; Seconds ~ Year, data to be written)//
	Seconds=Decimal_to_BCD(Seconds);//Decimal converted to BCD code
	Minutes=Decimal_to_BCD(Minutes);//
	Hours=Decimal_to_BCD(Hours);//
	
	Date=Decimal_to_BCD(Date);//
	Year=Decimal_to_BCD(Year);//
	Month=Decimal_to_BCD(Month);//
	
	TWI_Write(0x00,Seconds);//write Seconds
	TWI_Write(0x01,Minutes);//write Minutes
	TWI_Write(0x02,Hours);//write Hours
	
	TWI_Write(0x03,Weeks);//write Weeks

	TWI_Write(0x04,Date);//write Data
	TWI_Write(0x05,Month);//write Month
	TWI_Write(0x06,Year);//write Year
}

void read_ds1307()//read ds1307
{
	//Read a data to the device: (0xd0 is the DS1307 device write address; 0x01 ~ 06 are seconds to years register; 0xd1 is DS1307 device read address) 
	Seconds=TWI_Read(0x00);
	Seconds=BCD_to_Decimal(Seconds);//BCD code converted to decimal
	
	Minutes=TWI_Read(0x01);
	Minutes=BCD_to_Decimal(Minutes);//
	WDR();

	Hours=TWI_Read(0x02);
	Hours=BCD_to_Decimal(Hours);//
	
	Weeks=TWI_Read(0x03);//
	WDR();	

	Date=TWI_Read(0x04);
	Date=BCD_to_Decimal(Date);//
	
	Month=TWI_Read(0x05);
	Month=BCD_to_Decimal(Month);//
	WDR();

	Year=TWI_Read(0x06);
	Year=BCD_to_Decimal(Year);//
}	

void display(uchar command,uchar dat)//Display data in the specified location of the LCD1602
{
	write_Directive(command);
	write_Data(dat);
}
int main(void)
{
	DDRA = 0XFF;//LCD1602 Data port
	DDRC = 0XFF;//LCD1602 Control port£ºRE,RW,E
	twi_init();//TWI Initialization
	lcdinit();	//LCD1602 Initialization
	
	write_Directive(0x80);//Display static data
	for(count=0;count<16;count++) write_Data(string3[count]);
	write_Directive(0x80+0x40);
	for(count=0;count<16;count++) write_Data(string4[count]);

	//write_ds1307();//Write the initial value on ds1307//(Modification time initial value when used)
	while(1)
	{ 
		read_ds1307();
		Show();//Display dynamic data
	}
}
