#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SPI.h>


//#include "TeensyThreads.h"
// Using supplied cable:
// - Black = GND (connected to GND)
// - Red = 5V (4.5 - 6.0V) (connected to Vin on Teensy 3.5, or 5V on Arduino)
// - White = TFmini RX (aka. connect to microcontroller TX, pin1 on Teensy 3.5)
// - Green = TFmini TX (aka. connect to microcontroller RX, pin0 on Teensy 3.5)

// NOTE: for this sketch you need a microcontroller with additional serial ports beyond the one connected to the USB cable
// This includes Arduino MEGA (use Serial1), Teensy (3.x) (use one of the available HW Serial connections)

 //LCD CONNECTION  
  // ===============================
/*
The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3) 
*/


volatile int liDARval = 0;
int count1=0,count2=0,val1=0,val2=0,lidarno=0,lidar0_done=0,lidar1_done=0,next=0,val1_l=0,val1_h=0,x=0;
char a=0;

//ISR(TIMER0_OVF_vect); 
void readLiDAR(){
  // Data Format for Benewake TFmini
  // ===============================
  // 9 bytes total per message:
  // 1) 0x59 
  // 2) 0x59
  // 3) Dist_L (low 8bit)
  // 4) Dist_H (high 8bit)
  // 5) Strength_L (low 8bit)
  // 6) Strength_H (high 8bit)
  // 7) Reserved bytes
  // 8) Original signal quality degree
  // 9) Checksum parity bit (low 8bit), Checksum = Byte1 + Byte2 +...+Byte8. This is only a low 8bit though

 // while(1)
 { // Keep going for ever
    while(Serial1.available()>=9) // When at least 9 bytes of data available (expected number of bytes for 1 signal), then read
    {
      if((0x59 == Serial1.read()) && (0x59 == Serial1.read())) // byte 1 and byte 2
      {
        unsigned int t1 = Serial1.read(); // byte 3 = Dist_L
        unsigned int t2 = Serial1.read(); // byte 4 = Dist_H
        t2 <<= 8;
        t2 += t1;
        liDARval = t2;
        t1 = Serial1.read(); // byte 5 = Strength_L
        t2 = Serial1.read(); // byte 6 = Strength_H
        t2 <<= 8;
        t2 += t1;
        for(int i=0; i<3; i++)Serial1.read(); // byte 7, 8, 9 are ignored
       } 
    }
  }
}

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{  
   // pinMode(8,OUTPUT);  
    pinMode(29,OUTPUT);  
    pinMode(7,OUTPUT);
    lcd.begin(16,2);
    Serial1.begin(115200); // HW Serial for TFmini
    Serial3.begin(9600); //to atmega32
    Serial.begin(9600); // Serial output through USB to computer
    delay (100); // Give a little time for things to start
    // Set to Standard Output mode
    
    Serial1.write(0x42);
    Serial1.write(0x57);
    Serial1.write(0x02);
    Serial1.write(0x00);
    Serial1.write(0x00);   
    Serial1.write(0x00);
    Serial1.write(0x01);
    Serial1.write(0x06);
    // Setup thread for reading serial input from TFmini
    //threads.addThread(readLiDAR);
    lcd.begin(16,2);                                   //Setup the LCD's number of columns and rows.
    lcd.clear();                //clear lcd screen
    lcd.setCursor(0,0);         //initial position of cursor

/*
noInterrupts();  // interrupts disabled
TCCR0B |= (1<<CS01); // (1<<CS02)|(1<<CS00); ///(1<<CS02)| (1<<CS00);
TIMSK0 |=(1<<TOIE0);
TCNT0  = 0;
interrupts(); 
*/

lidarno=0;
//digitalWrite(7,LOW);    //LED On
//digitalWrite(8,LOW);   //LED On
noInterrupts();  // interrupts disabled
TCNT1 = 0;            // preload timer 65536-16MHz/256/2Hz
TCCR1B |= (1 << CS11);    // 256 prescaler 
TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt

TCNT3 = 0;            // preload timer 65536-16MHz/256/2Hz
TCCR3B |= (1 << CS31);    // 256 prescaler 
TIMSK3 |= (1 << TOIE3);   // enable timer overflow interrupt

interrupts();             // enable all interrupts

}

////=====================//////////////////////============


ISR(TIMER3_OVF_vect)                                                               ///////////////  982ovf/sec
{
count2++;
if(count2==5)// && lidarno==0 )                        ///////////////   count should be at least  ,
  {
    
    
       if(val1<512)
        { 
          
          if(val1 <= 255)
          {
           //delayMicroseconds(10); 
           digitalWrite(29,LOW);
           digitalWrite(7,HIGH);
           delayMicroseconds(10); 
           Serial3.write(val1_l);
           
          }
          
          else if(val1 > 255)
          {
          // delayMicroseconds(10); 
           digitalWrite(29,HIGH);
           digitalWrite(7,LOW);
           delayMicroseconds(10); 
           Serial3.write(val1_l-255);  
          }
        } 
           count2=0;
 	   TCNT1=0;
  }      

}

ISR(TIMER1_OVF_vect)                                                               ///////////////  982ovf/sec
{ 
        count1++;
        if (count1==2)// && lidarno==0 )                        ///////////////   count should be at least  ,
         {
           readLiDAR();
           val1=liDARval;
           val1_l = val1&0x00FF ;
           val1_h = ((val1&0x0F00)>>8); 
           count1=0;
   	       TCNT1=0;
        }
}


void loop()
{

            Serial.println(liDARval);


}
