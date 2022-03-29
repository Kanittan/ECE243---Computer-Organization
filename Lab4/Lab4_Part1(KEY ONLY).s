               .equ      EDGE_TRIGGERED,    0x1
               .equ      LEVEL_SENSITIVE,   0x0
               .equ      CPU0,              0x01    // bit-mask; bit 0 represents cpu0
               .equ      ENABLE,            0x1

               .equ      KEY0,              0b0001
               .equ      KEY1,              0b0010
               .equ      KEY2,              0b0100
               .equ      KEY3,              0b1000

               .equ      IRQ_MODE,          0b10010
               .equ      SVC_MODE,          0b10011

               .equ      INT_ENABLE,        0b01000000
               .equ      INT_DISABLE,       0b11000000
/*********************************************************************************
 * Initialize the exception vector table
 ********************************************************************************/
                .section .vectors, "ax"

                B        _start             // reset vector
                .word    0                  // undefined instruction vector
                .word    0                  // software interrrupt vector
                .word    0                  // aborted prefetch vector
                .word    0                  // aborted data vector
                .word    0                  // unused vector
                B        IRQ_HANDLER        // IRQ interrupt vector
                .word    0                  // FIQ interrupt vector

/*********************************************************************************
 * Main program
 ********************************************************************************/
                .text
                .global  _start
_start:        
                /* Set up stack pointers for IRQ and SVC processor modes */
                //... Code not shown (START)
				// Below: From Lecture Example Code interrupts.s //
				MOV      R1, #0b11010010                    // interrupts masked, MODE = IRQ
             	MSR      CPSR_c, R1                            // change to IRQ mode
				LDR      SP, =0x40000                       // set IRQ stack pointer
            	
				// Below: From Lecture Example Code interrupts.s //
				/* Change to SVC (supervisor) mode with interrupts disabled */
            	MOV      R1, #0b11010011                    // interrupts masked, MODE = SVC
           		MSR      CPSR, R1                                // change to supervisor mode
           	    LDR      SP, =0x20000                       // set SVC stack 
				//... Code not shown (END)
				
                BL       CONFIG_GIC              // configure the ARM generic interrupt controller

                // Configure the KEY pushbutton port to generate interrupts
                //... Code not shown
				LDR      R0, =0xFF200050                    // pushbutton KEY base address
             	MOV      R1, #0xf                               // set interrupt mask bits
             	STR      R1, [R0, #0x8]                        // interrupt mask register is (base + 8)


                // enable IRQ interrupts in the processor
                //... Code not shown
				MOV      R0, #0b01010011                    // IRQ unmasked, MODE = SVC
             	MSR      CPSR, R0
				
IDLE:
                B        IDLE                    // main program simply idles

IRQ_HANDLER:
                PUSH     {R0-R7, LR}
    
                /* Read the ICCIAR in the CPU interface */
                LDR      R4, =0xFFFEC100
                LDR      R5, [R4, #0x0C]         // read the interrupt ID

CHECK_KEYS:
                CMP      R5, #73
				BEQ      KEY_IRQ
				
UNEXPECTED:     B        UNEXPECTED              // if not recognized, stop here
    
KEY_IRQ:        BL       KEY_ISR

EXIT_IRQ:
                /* Write to the End of Interrupt Register (ICCEOIR) */
                STR      R5, [R4, #0x10]
    
                POP      {R0-R7, LR}
                SUBS     PC, LR, #4

/*****************************************************0xFF200050***********************************
 * Pushbutton - Interrupt Service Routine                                
 *                                                                          
 * This routine checks which KEY(s) have been pressed. It writes to HEX3-0
 ***************************************************************************************/
                .global  KEY_ISR
KEY_ISR:
                //... Code not shown
				LDR      R3, =0xff20005c // Key Edgecapture address
				LDR      R1, [R3] // Key Edgecapture state
				LDR      R3, =0xff200020 // LED ADDRESS
				LDR      R0, [R3] // LED State
				
				// Toggle 0 if 0 is interrupt
				AND      R2, R1, #0x1
				CMP      R2, #0x0
				BEQ      CHK_1
				EOR      R0, #0x3f
CHK_1:
				// Toggle 1 if 1 is interrupt
				AND      R2, R1, #0x2
				CMP      R2, #0x0
				BEQ      CHK_2
				EOR      R0, #0x0600
CHK_2:
				// Toggle 2 if 2 is interrupt
				AND      R2, R1, #0x4
				CMP      R2, #0x0
				BEQ      CHK_3
				EOR      R0, #0x5B0000
CHK_3:
				// Toggle 3 if 3 is interrupt
				AND      R2, R1, #0x8
				CMP      R2, #0x0
				BEQ      CHK_4
				EOR      R0, #0x4F000000
CHK_4:
				STR      R0, [R3] // Store new state into LED
				
				// To reset edgecapture
				MOV      R0, #0xf 
				LDR      R3, =0xff20005c // Key Edgecapture ADDRess
				STR      R0, [R3]
				
				// Return back to caller (IRQ_Handler)
                MOV      PC, LR

/* 
 * Configure the Generic Interrupt Controller (GIC)
*/
                .global  CONFIG_GIC
CONFIG_GIC:
                PUSH     {LR}
                /* Enable the KEYs interrupts */
                MOV      R0, #73
                MOV      R1, #CPU0
                /* CONFIG_INTERRUPT (int_ID (R0), CPU_target (R1)); */
                BL       CONFIG_INTERRUPT

                /* configure the GIC CPU interface */
                LDR      R0, =0xFFFEC100        // base address of CPU interface
                /* Set Interrupt Priority Mask Register (ICCPMR) */
                LDR      R1, =0xFFFF            // enable interrupts of all priorities levels
                STR      R1, [R0, #0x04]
                /* Set the enable bit in the CPU Interface Control Register (ICCICR). This bit
                 * allows interrupts to be forwarded to the CPU(s) */
                MOV      R1, #1
                STR      R1, [R0]
    
                /* Set the enable bit in the Distributor Control Register (ICDDCR). This bit
                 * allows the distributor to forward interrupts to the CPU interface(s) */
                LDR      R0, =0xFFFED000
                STR      R1, [R0]    
    
                POP      {PC}
/* 
 * Configure registers in the GIC for an individual interrupt ID
 * We configure only the Interrupt Set Enable Registers (ICDISERn) and Interrupt 
 * Processor Target Registers (ICDIPTRn). The default (reset) values are used for 
 * other registers in the GIC
 * Arguments: R0 = interrupt ID, N
 *            R1 = CPU target
*/
CONFIG_INTERRUPT:
                PUSH     {R4-R5, LR}
    
                /* Configure Interrupt Set-Enable Registers (ICDISERn). 
                 * reg_offset = (integer_div(N / 32) * 4
                 * value = 1 << (N mod 32) */
                LSR      R4, R0, #3               // calculate reg_offset
                BIC      R4, R4, #3               // R4 = reg_offset
                LDR      R2, =0xFFFED100
                ADD      R4, R2, R4               // R4 = address of ICDISER
    
                AND      R2, R0, #0x1F            // N mod 32
                MOV      R5, #1                   // enable
                LSL      R2, R5, R2               // R2 = value

                /* now that we have the register address (R4) and value (R2), we need to set the
                 * correct bit in the GIC register */
                LDR      R3, [R4]                 // read current register value
                ORR      R3, R3, R2               // set the enable bit
                STR      R3, [R4]                 // store the new register value

                /* Configure Interrupt Processor Targets Register (ICDIPTRn)
                  * reg_offset = integer_div(N / 4) * 4
                  * index = N mod 4 */
                BIC      R4, R0, #3               // R4 = reg_offset
                LDR      R2, =0xFFFED800
                ADD      R4, R2, R4               // R4 = word address of ICDIPTR
                AND      R2, R0, #0x3             // N mod 4
                ADD      R4, R2, R4               // R4 = byte address in ICDIPTR

                /* now that we have the register address (R4) and value (R2), write to (only)
                 * the appropriate byte */
                STRB     R1, [R4]
    
                POP      {R4-R5, PC}

                .end   