          .text                   // executable code follows
          .global _start                  
_start:   // ONES:                          
          MOV     R1, #TEST_NUM   // load the data word ...
          LDR     R1, [R1]        // into R1
		  BL	  ONES
		  MOV	  R5, R0
		  
		  //ZEROS:
		  MOV     R1, #TEST_NUM   // load the data word ...
		  LDR     R1, [R1] 
		  LDR	  R4, =#0xffffffff
		  EOR	  R3, R1, R4  // Invert by XOR R1 with ...11111...(32 bits of 1's)
		  MOV	  R1, R3
		  BL	  ZEROS
		  MOV	  R6, R0
		  
		  // Alternating Zeros and Ones:
		  MOV     R1, #TEST_NUM   // load the data word ...
		  LDR     R1, [R1]
		  BL	  ALTERNATE
		  MOV	  R7, R12
		  
STOP:	  B		  STOP


// ONES Subroutine
ONES:     MOV     R0, #0          // R0 will hold the result
		  
LOOP1:    CMP     R1, #0          // loop until the data contains no more 1's
          BEQ     END             
          LSR     R2, R1, #1      // perform SHIFT, followed by AND
          AND     R1, R1, R2      
          ADD     R0, #1          // count the string length so far
          B       LOOP1            
// END tag is at the bottom


ZEROS:    MOV     R0, #0          // R0 will hold the result

LOOP2:    CMP     R1, #0          // loop until the data contains no more 1's
          BEQ     END             
          LSR     R2, R1, #1      // perform SHIFT, followed by AND
          AND     R1, R1, R2      
          ADD     R0, #1          // count the string length so far
          B       LOOP2    



ALTERNATE: MOV    R0, #0
		   MOV    R10, #1    // R10 = Counter
		   MOV	  R11, #0	 // R11 Stores LARGEST consecutive 1's or 0's
		   MOV	  R12, #0	 
		   LDR	  R4, =#0xAAAAAAAA
		   LDR	  R8, =#0x55555555
		   EOR	  R3, R1, R4 //R3 holds R1 XOR with 101010...  
		   //CMP    R10, #1;
		   B	  LOOP4
		   
LOOP3:	   CMP	  R10, #3
		   BEQ	  END
		   
		   CMP	  R12, R11	// KEY: Final Result is stored in R11
		   BLT	  NEXT  
		   
BACK:	   ADD	  R10, #1
		   MOV    R0, #0
		   EOR	  R3, R1, R8 //R3 holds R1 XOR with 101010... 
		   B	  LOOP4
		   
		   
NEXT:	   MOV	  R12, R11
		   B 	  BACK

		   
		   
LOOP4:     MOV	  R11, R0		  
		   CMP    R3, #0          // loop until the data contains no more 1's
		   //ADD	  R10, #1		  // Increment R10 by 1
           BEQ    LOOP3             
           LSR    R2, R3, #1      // perform SHIFT, followed by AND
           AND    R3, R3, R2      
           ADD    R0, #1          // count the string length so far
           B      LOOP4     
 	  


END:      MOV	  PC, LR

TEST_NUM: .word   0x123455bc   //0x123455bc -> 8 OR: 0x1234555c --> 12 

          .end    