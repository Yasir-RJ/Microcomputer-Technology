#include "gd32vf103.h"
#include "drivers.h"
#include "lcd.h"
#include "usart.h"
#define EI 1
#define DI 0


int main(void){
    int ms=0, s=0, key, pKey=-1, c=0, idle=0;
    //int lookUpTbl[16]={1,4,7,14,2,5,8,0,3,6,9,15,10,11,12,13};  // does not match my card
    // new 4x4 keyboard scan codes for GD32VF103 Rev.B
    // Non-numeric keys '#'=14, '*'=15
	int mykeytbl[16]={13,14,0,15,12,9,8,7,11,6,5,4,10,3,2,1};
    int twodigits=0, decvalue=0, col=0;
    //char digits[10][10]={"Zero ","One  ","Two  ","Three","Four ","Five ","Six  ","Seven","Eight","Nine "};
    //char msg[]="*";

    t5omsi();                               // Initialize timer5 1kHz
    colinit();                              // Initialize column toolbox
    l88init();                              // Initialize 8*8 led toolbox
    keyinit();                              // Initialize keyboard toolbox
    Lcd_SetType(LCD_NORMAL);                // or use LCD_INVERTED!
    Lcd_Init();
    LCD_Clear(BLACK);
    LCD_ShowStr(10, 10, "SMS BY YASIR", WHITE, TRANSPARENT);
    u0init(EI);                             // Initialize USART0 toolbox

    eclic_global_interrupt_enable();        // !!! INTERRUPT ENABLED !!!

    while (1) {
        idle++;                             // Manage Async events
        LCD_WR_Queue();                     // Manage LCD com queue!
        u0_TX_Queue();                      // Manage U(S)ART TX Queue!        

        if (t5expq()) {                     // Manage periodic tasks
            l88row(colset());               // ...8*8LED and Keyboard
            ms++;                           // ...One second heart beat
            
			if (ms==1000){
         ms=0;
         l88mem(0,s++);
         if (usart_flag_get(USART0,USART_FLAG_RBNE)){ 		// any received char USART0 RX?
				    LCD_ShowStr(10, 10, "SMS BY YASIR", WHITE, TRANSPARENT);
            LCD_ShowChar(col=col+10,50,usart_data_receive(USART0), OPAQUE, GREEN);    // show char on LCD
			    	if(col>=150){ col=0; LCD_Clear(BLACK);  // end of LCD row? clean it and start over
          		LCD_ShowStr(10, 10, "SMS BY YASIR", WHITE, TRANSPARENT);
            } 			
		  	  }	  
			}
            
			if ((key=keyscan()) >= 0) {       // ...Any key pressed? 
			  pKey=key;                     // store Key status
			  l88mem(1,mykeytbl[pKey]);	 // show the key pressed on 2nd row in 8x8 LED
			}
			
			if(pKey>=0){
				if(twodigits==0){
					decvalue=16*mykeytbl[pKey]; 	// Hex to Dec for first input digit
					twodigits=1;		// prepare for next hex digit
					pKey=-1;			// reset key status
			    }
			    else{
					decvalue=decvalue+mykeytbl[pKey];			// process the second input digit (ASCII code is ready)
					usart_data_transmit(USART0, decvalue); 		// send ASCII code to USRAT0 TX
					twodigits=0;		// reset hex digits for next input char
					pKey=-1;			// reset key status
				}
				
			}
			
            l88mem(2,idle>>8);              // ...Performance monitor
            l88mem(3,idle); idle=0;
        }
    }
}