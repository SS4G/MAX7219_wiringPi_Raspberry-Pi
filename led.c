#include <wiringPi.h>
/*
 *
 *
 void pinMode (int pin, int mode); //INPUT, OUTPUT
 void pullUpDnControl (int pin, int pud) ;//PUD_OFF PUD_UP PUD_DOWN
 void digitalWrite (int pin, int value) ;//HIGH  LOW 
 int digitalRead (int pin) ;
 void delayMicroseconds (unsigned int howLong)
 void delay (unsigned int howLong)
 wiringPiSetup (void) ;
 wiringPiSetupGpio (void) ;
 */

int main (void)
{
  wiringPiSetup () ;
  pinMode (1, OUTPUT) ;
  for (;;)
  {
    digitalWrite (1, HIGH) ; delay (500) ;
    digitalWrite (1,  LOW) ; delay (500) ;
  }
  return 0 ;
}
