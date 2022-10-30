
#include "gd32vf103.h"
#include "drivers.h"
#include "dac.h"
#include "pwm.h"

int main(void){
    int ms=0, s=0, key, pKey=-1, idle=0;
    //int lookUpTbl[16]={1,4,7,14,2,5,8,0,3,6,9,15,10,11,12,13};  // does not match my card
    // new 4x4 keyboard scan codes for GD32VF103 Rev.B
    // Non-numeric keys '#'=14, '*'=15
	int mykeytbl[16]={13,14,0,15,12,9,8,7,11,6,5,4,10,3,2,1};
    int set=0,vdac=0,vpwm=0;

    t5omsi();                               // Initialize timer5 1kHz
    colinit();                              // Initialize column toolbox
    l88init();                              // Initialize 8*8 led toolbox
    keyinit();                              // Initialize keyboard toolbox
    DAC0powerUpInit();                      // Initialize DAC0/PA4 toolbox
    
    ///////////// code added for ch0
    T1powerUpInitPWM(0x1);                  // Timer #1, Ch #0 PWM
    
    while (1) {
        idle++;                             // Manage Async events
 
        if (t5expq()) {                     // Manage periodic tasks
            l88row(colset());               // ...8*8LED and Keyboard
            ms++;                           
            if (ms==1000){					
              ms=0; l88mem(0,s++);       // show time (sec) on 1st row in 8x8 LED  
            }
            if ((key=keyscan())>=0) {       // ...Any key pressed?
              if (pKey!=key) pKey=key;		// store it
              l88mem(1,mykeytbl[key]);	 // show the key pressed on 2nd row in 8x8 LED

              if(mykeytbl[pKey]>=0 && mykeytbl[pKey]<=9){  // process with numbers 0-9
                set=set*10 + mykeytbl[pKey];
              }
                            
              if(mykeytbl[pKey]==15){     // key = '*' ? 
                set=set/10;				  // delete last digit of entry
              }
              if(mykeytbl[pKey]==14){     // key = '#' ?  
                set=0;					  // reset entry
              }
              if(mykeytbl[pKey]==0xD){        // key = 'D' ? --> set DAC output value
                if(set>=0 && set<=100){     // entry must be (0-100)
                vdac=set*4095/100;         // make it percentage from DAC 
                                          // 12-bit full value (0xFFF)
                DAC0set(vdac);            //DAC "voltage method" DAC0 Vout 
                vpwm=set*15999/100;       // make it % duty cycle from full 
                                          // period time (16000)
                T1setPWMch0(vpwm);      //DAC "effect method" PWM T1/Ch0
                }
              }
            l88mem(4,set);      // display the current % entry on 5th row in 8x8 LED
            
            }
            l88mem(2,idle>>8);              // ...Performance monitor
            l88mem(3,idle); idle=0;  
            }
            
        }
    }
