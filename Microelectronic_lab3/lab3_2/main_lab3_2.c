
#include "gd32vf103.h"
#include "drivers.h"
#include "dac.h"
#include "pwm.h"

int main(void){
    int ms=0, key, pKey=-1, idle=0, i=0;    
    //int lookUpTbl[16]={1,4,7,14,2,5,8,0,3,6,9,15,10,11,12,13};  // does not match my card
    // new 4x4 keyboard scan codes for GD32VF103 Rev.B
    // Non-numeric keys '#'=14, '*'=15
	int mykeytbl[16]={13,14,0,15,12,9,8,7,11,6,5,4,10,3,2,1};
    int speed=-100, set=4;
    
    // images for numbers 0-4 in 8x8 LED
    int pattern[5][8]={{0x00,0x3c,0x42,0x42,0x42,0x42,0x42,0x3c},   //0
                       {0x00,0x18,0x18,0x18,0x18,0x18,0x18,0x18},   //1
                       {0x00,0x3c,0x42,0x04,0x08,0x10,0x20,0x7e},   //2
                       {0x00,0x3c,0x42,0x04,0x1c,0x04,0x42,0x3c},   //3
                       {0x00,0x42,0x42,0x42,0x3e,0x02,0x02,0x02}};  //4

    t5omsi();                               // Initialize timer5 1kHz
    colinit();                              // Initialize column toolbox
    l88init();                              // Initialize 8*8 led toolbox
    keyinit();                              // Initialize keyboard toolbox
    DAC0powerUpInit();                      // Initialize DAC0/PA4 toolbox
    
    ///////////// code added for ch0 and ch1 (forward and backward speed)
    T1powerUpInitPWM(0x3);                  // Timer #1, enable Ch0 and ch1 PWM
    
    while (1) {
        idle++;                             // Manage Async events
 
        if (t5expq()) {                     // Manage periodic tasks
            l88row(colset());               // ...8*8LED and Keyboard
            ms++;                           
            if ((key=keyscan()) >= 0) {       // ...Any key pressed?
              pKey=key;                     // store Key status
            } 

            if (ms==1000){                // process with keys every second
              ms=0;              
              
              if(mykeytbl[pKey]==1){      // Is key =1 ?
               speed=speed+25;             // increase motor speed
               if(set<8) set++;            // increase pattern counter
               if (speed > 100) speed = 100;     // positive overflow?
			   pKey=-1;
              }  

              if(mykeytbl[pKey]==2){     // Is key =2 ?
               speed = 0;                // stop motor
               set = 4;                  // reset pattern counter
              }
              
              if(mykeytbl[pKey]==3){      // Is key =3 ?
                speed=speed-25;            // decrease motor speed
                if(set>0) set--;            // decrease pattern counter
                if (speed < -100) speed = -100;   // negitive overflow?
				pKey=-1;
              } 
              
              T1setPWMmotorA(speed);        // change motor speed in PA0 and PA1 LEDs
              
              /// set   : 8 7 6 5   4    3  2  1  0
              ///speed  : + + + +  Stop  -  -  -  -
              ///pattern: 4 3 2 1   0   ~1 ~2 ~3 ~4

              if(set>=4 && set<=8){         // if forward motor speed
                for(i=0;i<8;i++) {               
                l88mem(i,pattern[set-4][i]);  // show the pattern on 8x8 LED
                }
              }

              if(set>=0 && set<=3){         // if backward motor speed
                for(i=0;i<8;i++) {               
                l88mem(i,~(pattern[4-set][i]));    // show the reverse pattern on 8x8 LED
                }
              } 
            }             
            }            
        }
    }
