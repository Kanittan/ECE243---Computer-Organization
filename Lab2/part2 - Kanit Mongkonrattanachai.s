.global _start
	_start:                             
          MOV     R1, #TEST_NUM   // load the data word ...
		  LDR	  R5, [R1]
		  LDR	  R6, [R1]
		  MOV     R4, R1 	  	  // R4<-R1
		  //MOV	  R8, R1		  // Use to check Address of R1
          LDR     R1, [R1]        // into R1
		  BL   	  ONES
		  MOV	  R5, R3		  // R5 Store Longest consecutive 1's of all words in list.
STOP:     B		  STOP    		  // To Stop the program (Endless Loop)
		  
		  
ONES:	  MOV     R0, #0          // R0 will hold the result
		  MOV	  R3, #0
		  MOV	  R9, #0
		  
LOOP1:    CMP     R1, #0          // Loop until the data contains no more 1's
          BEQ     LOOP2           // Branch to LOOP2 IF: No more 1's left
          LSR     R2, R1, #1      // Perform SHIFT, followed by AND
          AND     R1, R1, R2      
          ADD     R0, #1          // Count the string length so far of longest consecutive 1's in a word
          B       LOOP1
		  
LOOP2:    CMP     R5, #0		  // Compare R5 with "0"(Indicate last word/ End of program)
		  BEQ 	  END			  // Branch to END IF: R5=0
		  
		  //ADD	  R8, #4		  // Use to check Address of R1
		  LDR     R5, [R4,#4]	  // Shift to next word (Shift by 4 byte since 1 word has 4 byte)
		  ADD	  R4, #4		  // Increment to the next word in list. (R4<-R4+4)
		  MOV	  R9, R0	      // Store/Keep content of R0 into R9 (R9<-R0)
		  MOV     R0, #0		  // Reset R0<-0 Since we want to use it for counting the consecutive 1's of the next word.
		  
		  MOV     R1, R5		  // Let: Store content(word) of R5 into R1 (R1<-R5) So that R1 gets the next word in list to find the longest consecutive 1's in the next word in LOOP1
		  
		  CMP	  R3, R9		  // Compare R3 and R9(Hold longest number of consecutive 1's in the word. (R3-R9)
		  BGE	  LOOP1			  // Branch to LOOP1 IF: R3>=R9
		  MOV	  R3, R9		  // IF: R3<R9, THEN: Store content of R9 into R3 (R3<-R2)
		  B		  LOOP1			  // Branch to Loop1

END:      MOV	  PC, LR            

TEST_NUM: .word   0x103fe00f, 0x00000003, 0xfffffffa, 0x00000000
          .end   