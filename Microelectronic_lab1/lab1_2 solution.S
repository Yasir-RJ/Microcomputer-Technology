
//     Yasir Riyadh Jabbar 31/1/2021.section .text
/////////////////////////////////////////////////////////////////////////////////////
// GPIO General Purpose I/0 Subroutine package                         0.91 AC
/////////////////////////////////////////////////////////////////////////////////////
.equ GPIOA,  0x40010800     // GPIOB Base address.
.equ GPIOB,  0x40010C00     // GPIOB Base address.
.equ CTR0,   0x00           // Port control register 0 offset
.equ ISTAT,  0x08           // Port input status register
.equ OCTL,   0x0C           // Port output control register
.equ BOP,    0x10           // Port bit operate register offset
.equ BC,     0x14           // Port bit clear register offset
.equ GPIO_AM, 0x0           // Mode: Input, Analog Mode
.equ GPIO_50, 0x3           // Mode: Output, max speed 50MHz
.equ GPIO_FI, 0x4           // Conf: (Input) Floating input
.equ GPIO_UD, 0x8           // Conf: (Input) Pull-up/-down
.equ GPIO_PP, 0x0           // Conf: (Output) Push-pull
.equ GPIO_OD, 0x4           // Conf: (Output) Open-drain

// GPIOI Initialize ////////////////////////////////////////////////// 1.0 / AC ////
// a0 Targeted GPIO-module
// a1 Configuration bits (<<2)
// a2 Mode bits
// a3 Targeted pin
gpioi:  mv t0, a0           // t0 points to GPIOX...
        mv t4, a3           //    t4 marks selected pin...
        li t1,0x8           //    Use port control register 0 or 1,
        blt t4,t1, 1f       //    thus is pin > 7?
        addi t0, t0, 4      //       Yes, adjust base address,
        addi t4,t4,-8       //            and adjust selected pin.

1:      slli t4,t4,2        //    Multiply pin nb by 4 to get steps to shift!
        li t1, 0xF          //    Create inverted configuration mask...
        sll t1, t1, t4      //    ...and move it into right position.
        not t1, t1          //    ...invert it back, then...
        mv t2, a1           //    Create configuration bits...
        or t2, t2, a2       //    
        sll t2, t2, t4      //    ...and move it into right position.

        lw t3,CTR0(t0)      //    retreive port control register 0(/1)...
        and t3,t3,t1        //    ...clear out selected pin's config/mode bits...
        or t3, t3, t2       //    ...configure new config/mode bits...
        sw t3,CTR0(t0)      //    ...and write it back.
        ret                 //  then go home!

// GPIOBO Bit Operations ///////////////////////////////////////////// 1.0 / AC ///
// a0 Targeted GPIO-module
// a1 Targeted pattern
// a2 Targeted pin(s)
gpiobo: xor a2, a2, a1      // Find zeros in targeted bits...
        slli a2, a2, 16     // ...position clear requests...
        or a1, a1, a2       // ...add set requests...
        sw a1, BOP(a0)      // ...then execute...
        ret                 // ...and return to caller!

// GPIBC Bit Clear /////////////////////////////////////////////////// 1.0 / AC ///
// a0 Targeted GPIO-module
// a1 Targeted pin(s)
gpiobc: sw a1, BC(a0)       // Execute request...
        ret                 // ...and return to caller!

// GPIOC Output Control ////////////////////////////////////////////// 1.0 / AC ///
// a0 Targeted GPIO-module
// a1 value
gpiooc: sw a1, OCTL(a0)      // Execute request...
        ret                 // ...and return to caller!

// GPIOIS Input Status //////////////////////////////////////////////// 1.0 / AC ///
// a0 Targeted GPIO-module
// a1 read value
gpiois: lw a1, ISTAT(a0)    // Execute request...
        ret                 // ...and return to caller!

// GPIO General Purpose I/0 Subroutine package ////////////////////////// END ///////

/////////////////////////////////////////////////////////////////////////////////////
// RCU Reset and Clock Unit Subroutine package                         0.91 AC
/////////////////////////////////////////////////////////////////////////////////////
.equ RCU,    0x40021000
.equ APB1EN, 0x1C
.equ T5EN,   0x10
.equ APB2EN, 0x18
.equ PAEN,   0x4
.equ PBEN,   0x8
// RCU1EN Enabel APB1 bus devices //////////////////////////////////// 1.0 / AC ///
rcu1en: li t0,RCU           // t0 points to RCU...
        lw t1,APB1EN(t0)    // ...retrieve current configuration...
        or t1,t1,a0         // ...and update the configuration...
        sw t1,APB1EN(t0)    // ...then save back the updated config...
        ret                 // and go home!

// RCU2EN Enabel APB2 bus devices //////////////////////////////////// 1.0 / AC ///
rcu2en: li t0,RCU           // t0 points to RCU...
        lw t1,APB2EN(t0)    // ...retrieve current configuration...
        or t1,t1,a0         // ...and update the configuration...
        sw t1,APB2EN(t0)    // ...then save back the updated config...
        ret                 // and go home!
        
// RCU Reset and Clock Unit Subroutine package ////////////////////////// END /////// 

// Lab 1_2 ... Yasir Riyadh Jabbar 31/1/2021
// ((Alkotest))
.global main
main:   li a0, PBEN+PAEN    // Prepare to turn on GPIOB and GPIOA ...
        call rcu2en         // ...do the set-up!

        li a0, GPIOB        // Prepare to init GPIOB...
        li a1, GPIO_PP      // ...as Push-Pull Output...
        li a2, GPIO_50      // ...with max 50MHz rate...
        li a3, 0            // ...for pin 0...
        li a4, 3            // ...to pin 2...
1:      call gpioi          // ...do the set-up!
        addi a3,a3,1        // ...advance to next pin...
        bne a3,a4, 1b       // ...continue, if not done!  

        li a0, GPIOA        // Prepare to init GPIOA...
        li a1, GPIO_AM      // ...in Analoge mode...
        li a2, GPIO_FI      // ...with High-Z...
        li a3, 5            // ...for pin >>>>> 5...
        li a4, 9            // ...to pin >>>>>> 8...
1:      call gpioi          // ...do the set-up!
        addi a3,a3,1        // ...advance to next pin...
        bne a3,a4, 1b       // ...continue, if not done!   

 //     Code modified for Lab 1_2 ((Alkotest))
 //     Yasir Riyadh Jabbar 31/1/2021
 //           PB0  PB1  PB2  <<< LEDS  ... 3 correct patterns -->> All Leds ON
 //           PA5  PA6  PA7  <<< DIP SWITCHES
 ////////////////////////////////
 //     t0 = memory address
 //     t1 = temp reg to hold memory content
 //     t2 = 7 ... end of 3 patterns sequnce


start:  la t0,membuf    // load memory offset address        
        li t2,7         // pass the test
           
next:   li a0, GPIOB        // Prepare to write GPIOB...        
        lb a1, (t0)    // pattern in a1 

        xori a1,a1,1   // reverse pattern
        li t4,4
        beqz a1,ok
        lb a1, (t0)
        xori a1,a1,4
        li t4,1
        beqz a1,ok
        lb a1, (t0)
        xori a1,a1,3
        li t4,6
        beqz a1,ok
        lb a1, (t0)
        xori a1,a1,6
        li t4,3
        beqz a1,ok
        lb t4,(t0)

ok:     mv a1,t4
        li a2, 0x07         // ...the 3 lsb... 
        call gpiobo         // ...and write the new value!
        lb t1, (t0)
        bne t1,t2,testin    // is it 7 ?

teston: li a0, GPIOA        // Prepare to read GPIOA...
        call gpiois         // ...and get the value in a1   
        srli a1,a1,6        // ...shift it down 6 steps.        
        andi a1,a1,7    // mask only least 3 bits .. only PA7, PA6, and PA5
		xori a1,a1,7
		bnez teston
        j pass

testin: li a0, GPIOA        // Prepare to read GPIOA...
        call gpiois         // ...and get the value in a1   
        srli a1,a1,6        // ...shift it down 6 steps.        
        andi a1,a1,7    // mask only least 3 bits .. only PA7, PA6, and PA5
        lb t1, (t0)   // compare with stored pattern
        bne a1,t1,testin    // correct pattern ? No .. test again

pass:   addi t0,t0,1    // yes ... increament memeory pointer      
        lb t1, (t0)
        beq t1,zero,start //.. end of memory buffer ? yes start again
        j next          // No ... test next pattern
    
.section .data
        membuf: .byte 3,6,5,7
                .byte 2,1,4,7
                .byte 1,5,2,7
                .byte 3,6,4,7
                .byte 5,4,1,7
                .byte 4,3,6,7
                .byte 1,5,2,7
                .byte 4,3,6,7
                .byte 0  
.end
