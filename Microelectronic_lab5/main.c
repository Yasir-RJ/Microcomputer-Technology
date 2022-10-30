#include "gd32vf103.h"
#include "drivers.h"
#include "lcd.h"

int main(void){
    int ms=0, s=0, key, pKey=-1, c=0, idle=0;
    //int lookUpTbl[16]={1,4,7,14,2,5,8,0,3,6,9,15,10,11,12,13};  // does not match my card
    // new 4x4 keyboard scan codes for GD32VF103 Rev.B
    // Non-numeric keys '#'=14, '*'=15
	int mykeytbl[16]={13,14,0,15,12,9,8,7,11,6,5,4,10,3,2,1};
    int dac=0, speed=-100;
    int adcr, tmpr;
    char digits[10][10]={"Zero ","One  ","Two  ","Three","Four ","Five ","Six  ","Seven","Eight","Nine "};

    t5omsi();                               // Initialize timer5 1kHz
    colinit();                              // Initialize column toolbox
    l88init();                              // Initialize 8*8 led toolbox
    keyinit();                              // Initialize keyboard toolbox
    Lcd_SetType(LCD_NORMAL);                // or use LCD_INVERTED!
    Lcd_Init();
    LCD_Clear(RED);
    LCD_ShowStr(10, 10, "POLL BY YASIR", WHITE, TRANSPARENT); // show string on LCD

    while (1) {
        idle++;                             // Manage Async events
        LCD_WR_Queue();                   // Manage LCD com queue!

      if (t5expq()) {                     // Manage periodic tasks
            l88row(colset());               // ...8*8LED and Keyboard
            ms++;                           // ...One second heart beat
      if (ms==1000){
              ms=0;
              l88mem(0,s++);			// show the time on 1st row in 8x8 LED
              LCD_ShowStr(10, 30, digits[s%10], WHITE, OPAQUE);		// show numbers 0-9 each second on LCD
            }
      if ((key=keyscan()) >= 0) {       // ...Any key pressed?
              pKey=key;                     // store Key status
			  l88mem(1,mykeytbl[key]);	 // show the key pressed on 2nd row in 8x8 LED
            }
      
      if(mykeytbl[pKey]>=0 && mykeytbl[pKey]<=4){      // Is key beetwen 0 and 4 ?
         LCD_Clear(RED);
         LCD_ShowStr(10, 10, "POLL BY YASIR", WHITE, TRANSPARENT); // show string on LCD
         LCD_ShowChar(10, 50,126+mykeytbl[pKey],TRANSPARENT,GREEN); // show 100% battery icon on LCD
         pKey=-1;			// reset key status
        }
			
			}
            l88mem(2,idle>>8);              // ...Performance monitor
            l88mem(3,idle); idle=0;
        
    }
}