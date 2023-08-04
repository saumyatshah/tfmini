/*
 * TFmini.cpp
 *
 * Created: 02-03-2019 20:28:24
 * Author : Ayush
 */ 

 #define  F_CPU 16000000UL
 #include <avr/io.h>
 #include "GPIO.h"
 #include "LCD.h"
 #include "UART.h"
 #include "SPI.h"
 #include <stdlib.h>
 #include <util/delay.h>
 #include <avr/interrupt.h>




 #define Rs PORTA7
 #define e  PORTA5
 #define d4 PORTA3
 #define d5 PORTA2
 #define d6 PORTA4
 #define d7 PORTA6

 #define MUXA PORTD6
 #define MUXB PORTD4

/*
#define Rs PORTB0
#define e  PORTB1
#define d4 PORTB2
#define d5 PORTB3
#define d6 PORTB4
#define d7 PORTB5
*/
//================================================FUNCTION VARIABLES DEFINATIONS======================================	
 int tf_dist=0,count=0;
 int A=0,B=0,C=0;
 int32_t lidarvalue=0,next=0,laser1=0,laser2=0,laser3=0,laser4=0;
  unsigned char uart_data[8];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char tfmini_dist(int channnel)
{ 
	   	    uart_data[1]=Inputbin(COM0);           // byte1 
            uart_data[2]=Inputbin(COM0);           // byte2 
		   
			char  t1 = Inputbin(COM0); // byte 3 = Dist_L
		    char  t2 = Inputbin(COM0); // byte 4 = Dist_H
		    A=t1;
			B=t2;
			B = (B << 8);
		    B += A;
			lidarvalue = B;
		    //   t1 =
			uart_data[5]= Inputbin(COM0); // byte 5 = Strength_L
	    	//    t2 =
			uart_data[5]= Inputbin(COM0); // byte 6 = Strength_H
		    //  t2 <<= 8;
		    //  t2 += t1;
		    for(int i=0; i<3; i++)
			{
				uart_data[5]=Inputbin(COM0);
			} // byte 7, 8, 9 are ignored
				
	return lidarvalue;      
/*
//========================================================UART SETTING FOR TFmini==============================================
ConfigUART(COM0,7,_TX,WORD_LENGTH_8,STOP_BIT_1,none);   //// BAUD RATE = 115200 is fixed for TF mini  DO NOT CHANGE THE UBBR
UBRRL = 7UL;
/////////////////////////////////////////////////////////// ///////////////////////////////////////////////////////
   Printbin(COM0,0x42);
   Printbin(COM0,0x57);
   Printbin(COM0,0x02);
   Printbin(COM0,0x00);
   Printbin(COM0,0xff);
   Printbin(COM0,0xff);
   Printbin(COM0,0xff);
   Printbin(COM0,0xff);
*/
}
 
void timer2_init()
{
	TCCR2 |= (1<<CS20); // (1<<CS02)|(1<<CS00); ///(1<<CS02)| (1<<CS00);
	TIMSK |=(1<<TOIE2);
	TCNT2  = 0;
}

ISR(TIMER2_OVF_vect)                                                               ///////////////  982ovf/sec
{
	cli();
	count++;
	if (count>=2510)                        ///////////////   count should be at least  ,
	{
		if( ((PIND & 0x01<<PIND3)<<0) && (!((PIND & 0x01<<PIND2)<<0)) )//PIND == 254)	
		{
			A=Inputbin(COM0);
			
		}

		if( ((PIND & 0x01<<PIND2)<<0) && (!((PIND & 0x01<<PIND3)<<0)) )//PIND == 253)(1<<pind2)&&(~(1<<pind3))
		{
			A=Inputbin(COM0);
			A=A+255;
		}
		count=0;
		TCNT2 =0;
	}
	sei();
}

ISR (INT1_vect)
{
cli();

sei();
}   

void laser_call(int number)
{
	if(number==1)
	{
		reset(MUXA);
		reset(MUXB);
		waitms(35);
		cls();
		lcd((char*)"l1=");
		lcd(A);
	}
	else if(number==2)
	{
		set(MUXA);
		reset(MUXB);
		cls();
		lcd((char*)"l2=");
		lcd(A);
	}
	else if(number==3)
	{
		cls();
		reset(MUXA);
		set(MUXB);
		lcd((char*)"l3=");
		lcd(A);
	}
	else if(number==4)
	{
		cls();
		set(MUXA);
		set(MUXB);
		
		lcd((char*)"l4=");
		lcd(A);
	}
	
	
}


int main(void)
{

//========================================================UART SETTING FOR TFmini==============================================	
//ConfigUART(COM0,8,_TX,WORD_LENGTH_8,STOP_BIT_1,none);   //// BAUD RATE = 115200 is fixed for TF mini  DO NOT CHANGE THE UBBR  
//UBRRL = 8UL;

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////
DDRA  = 0xFF;
//ex_interrupt(ExINT1,rising);
ConfigLcd(Rs,e,d4,d5,d6,d7);

lcd((char*)"POINT");
lowerline();
lcd((char*)"TEAM INDIA 2019");
// lcd(attempt);
waitms(500);

DDRC = 0x00;
DDRD = 0b01010000;
PORTD = 0b01011100;

ConfigUART(COM0,103,_RX,WORD_LENGTH_8,STOP_BIT_1,none);   //// BAUD RATE = 9600 is fixed for arduino communication  DO NOT CHANGE THE UBBR
UBRRL = 103UL;
UBRRH= 0UL;
waitms(100);
timer2_init();
sei(); 

//DDRA = 0x0F;
//set(PORTD4);
//set(PORTD6);

while (1) 
    {
		cls();
		lcd((char*)"inloop");
		waitms(1000);
		for(int i=0;i<500;i++)
		{
			laser_call(1);
		}
		for(int i=0;i<500;i++)
		{
			laser_call(2);
		}
		for(int i=0;i<500;i++)
		{
			laser_call(3);
		}
		
		for(int i=0;i<500;i++)
		{
			laser_call(4);
		}
  // cls();
   //lcd(A);
//   lowerline();
//   lcd(B);
    }
}
 
