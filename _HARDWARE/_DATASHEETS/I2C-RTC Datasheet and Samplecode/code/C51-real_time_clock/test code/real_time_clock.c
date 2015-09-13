//This procedure is used to display the current time, and can modify the initial time.
//AT89C51 The crystal frequency is 12MHz
//Function: DS1307 display: seconds, minutes, weeks, days, date, month, year. Can show :2000-2909.Automatic leap year adjustment.
//Note: DS1307 to read and write are BCD, the exchange between the decimal and BCD.
//Time: July 27, 2012
/********************************************************************************************/
#include "reg51.h"//
#include "intrins.h"//
#define uchar unsigned char//
#define uint unsigned int//

sbit sda=P1^1;//;Simulate the I2C data transmit bit
sbit scl=P1^0;//;Simulate the I2C clock control state flag

sbit RS=P2^0;//LCD1602 define the I / O hardware interface
sbit RW=P2^1;
sbit E=P2^2; //

uchar count;
uchar string3[]="Time:  :  :     ";
uchar string4[]="20  .  .  week: ";
uchar Hours=13;//Definition of hours
uchar Minutes=34;//Definition of minutes
uchar Seconds=30;//Definition of seconds

uint  Year=12;//Definition of year
uchar Month=7;//Definition of month
uchar Date=30;//Definition of date
uchar Weeks=1;//Definition of weeks 

uchar DispBuf[16]; //16 bytes of display buffer
uchar disp_tab[]={'0','1','2','3','4','5','6','7','8','9'};

void delay_2(uint z)//1ms delay
{ 
	uchar x,x1;
	for(x1=0;x1<z;x1++)
	{
		for(x=0;x<114;x++);
	}
}

void delay()//5us delay
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}
void delay_1()//1ms delay
{ 
	uchar x,x1;
	for(x1=0;x1<2;x1++)
	{
		for(x=0;x<114;x++);
	}
}

void star()//I2C star
{
	sda=1;
	delay();//5us delay
	scl=1;
	delay();//5us delay
	sda=0;
	delay();//5us delay

}

void stop()//I2C stop
{
	sda=0;
	delay();//5us
	scl=1;
	delay();//5us
	sda=1;
	delay();//5us
}
void ack()//
{ 
	uchar z=0;
	while((sda==1)&&(z<50))z++;//Conditional, sda = 1, there is no answer. If no answer is delay: z <50, z + +; return to
	scl=0;
	delay();//5us
}
/////////////////////////////////////////////////////////////////////////////////////////
void write(uchar slave_write_address,uchar byte_address,uchar data_data)//Write a data
{
	uchar temp,temp1,i,ii;
	
	star();//
	
	for(ii=0;ii<3;ii++)//In order to send: the device write address, the byte address, data
	{ 
		if(ii==0)
		{
			temp=slave_write_address;//he device write address
			temp1=slave_write_address;
		}
		else if(ii==1)
		{
			temp=byte_address;//the byte address
			temp1=byte_address;
		}
		else if(ii==2)
		{
			temp=data_data;//data
			temp1=data_data;
		}
	
		for(i=0;i<8;i++)
		{
			scl=0;
			delay();//5us
			temp=temp1;
			temp=temp&0x80;// Take the highest bit
			
			if(temp==0x80)//			
			sda=1; 
			else
			sda=0;
			
			delay();//5us
			scl=1;
			delay();//5us
			scl=0;
			delay();//5us
			temp1=temp1<<1;//Prepare the next bit
	
		}
		sda=1;
		delay();//5us
		scl=1;
		delay();//5us
		ack();
	}
	stop();//
}

uchar read(uchar slave_write_address,uchar byte_address,uchar slave_read_address)//Read a data
{
	uchar temp,temp1,i,ii,x,data_data;
	
	star();//
	
	for(ii=0;ii<3;ii++)//In order to send: the device write address, the byte address, data
	{ 
		if(ii==0)
		{
			temp=slave_write_address;//
			temp1=slave_write_address;
		}
		else if(ii==1)
		{
			temp=byte_address;//
			temp1=byte_address;
		}
		else if(ii==2)
		{ 
			star();//		
			temp=slave_read_address;//
			temp1=slave_read_address;
		}
	
	
		for(i=0;i<8;i++)//Began to read data
		{
			scl=0;
			delay();//5us
			temp=temp1;
			temp=temp&0x80;// 
			
			if(temp==0x80)//		
			sda=1; 
			else
			sda=0;
			
			delay();//5us
			scl=1;
			delay();//5us
			scl=0;
			delay();//5us
			temp1=temp1<<1;//
		}
		sda=1;
		delay();//5us
		scl=1;
		delay();//5us
		ack();//
	}
	
	for(x=0;x<8;x++)
	{
		data_data=data_data<<1;//
		
		sda=1;
		delay();//5us
		scl=0;
		delay();//5us
		scl=1;
		delay();//5us
		
		if(sda==1)//
		data_data|=0x01;//
		//else 
		//data_data|=0x00;
	}
	ack();//
	stop();//
	return data_data;//

}
/////////////////////////////////////////////////////////////////////////////////////////
void write_Directive(uchar command)//LCD1602 write command
{
	E=0;
	RS=0;
	RW=0;	
	P0=command;
	_nop_();	
	E=1;	
	delay_1();
	E=0;
}
void write_Data(uchar dat)//LCD1602 read data
{
	E=0;
	RS=1;
	RW=0;
	P0=dat;
	_nop_();
	E=1;
	delay_1();
	E=0;
}
void lcdinit(void)//LCD1602 Initialization
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
/////////////////////////////////////////////////////////////////////////////////////////

void init()//ds1307 Initialization
{
	sda=1;
	scl=1;
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

uchar Decimal_to_BCD(uchar temp)//decimal to BCD
{
	uchar a,b,c;
	a=temp;
	b=0;
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
	b=0;
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

void write_ds1307()
{
	//Write a data to the device: (0xd0 is device write address; 0x01 ~ 06 are seconds to years register; Seconds ~ Year, data to be written)//
	Seconds=Decimal_to_BCD(Seconds);//Decimal converted to BCD code
	Minutes=Decimal_to_BCD(Minutes);//
	Hours=Decimal_to_BCD(Hours);//
	
	Date=Decimal_to_BCD(Date);//
	Year=Decimal_to_BCD(Year);//
	Month=Decimal_to_BCD(Month);//
	
	write(0xd0,0x00,Seconds);//write seconds
	write(0xd0,0x01,Minutes);//write minutes
	write(0xd0,0x02,Hours);//hours
	
	write(0xd0,0x03,Weeks);//
	write(0xd0,0x04,Date);//
	write(0xd0,0x05,Month);//
	write(0xd0,0x06,Year);//
}
void read_ds13074()
{
	//Read a data to the device: (0xd0 is the DS1307 device write address; 0x01 ~ 06 are seconds to years register; 0xd1 is DS1307 device read address) 	
	Seconds=read(0xd0,0x00,0xd1);//read second
	Seconds=BCD_to_Decimal(Seconds);//CD code converted to decimal
	
	Minutes=read(0xd0,0x01,0xd1);
	Minutes=BCD_to_Decimal(Minutes);//
	
	Hours=read(0xd0,0x02,0xd1);
	Hours=BCD_to_Decimal(Hours);//
	
	Weeks=read(0xd0,0x03,0xd1);//
	
	Date=read(0xd0,0x04,0xd1);
	Date=BCD_to_Decimal(Date);//
	
	Month=read(0xd0,0x05,0xd1);
	Month=BCD_to_Decimal(Month);//
	
	Year=read(0xd0,0x06,0xd1);
	Year=BCD_to_Decimal(Year);//
}
void main()
{

	init();
	lcdinit();
	write_Directive(0x80);
	for(count=0;count<16;count++) write_Data(string3[count]);
	write_Directive(0x80+0x40);
	for(count=0;count<16;count++) write_Data(string4[count]);
	//write_ds1307();//Write the initial value on ds1307//(Modification time initial value when used)
	while(1)
	{ 

		read_ds13074();
		Show();
	}
}

