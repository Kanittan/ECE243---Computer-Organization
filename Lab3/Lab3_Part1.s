.global _start
_start:
// Initialization
		MOV	R1, #0
		MOV R2, #0
		MOV R3, #0
		LDR	R2, =0xFF200000
		MOV	R1,	#Num_List
		LDR	R3, [R1]
		MOV	R4, #0
		


LOOP:
		STR	R3, [R2, #0x20]				// Store R3 to 7 Segment Display

// Check IF Previously, KEY3 is Pressed Or Not
		CMP	R4, #1						// CHECK IF R4=1 -> Mean Previously KEY3 is Pressed to TURN OFF 7 Segment Display
		BEQ	KEY3_Cond1					// IF SO: Branch to KEY3_Cond1 since Pressing any KEY0-2 afterwards will set the Screen to 0

// Check for KEY Press
		LDR	R0, [R2, #0x50]				// Load Content of Push Buttons into R0
		CMP	R0, #0
		BEQ	NO_KEY						// IF:No Key is Pressed, THEN:Branch to tag "NO_KEY"

// Check if KEY0 is Press
		CMP	R0, #0x1					// IF: R0=0x1=0001 (KEY0 is Pressed)
		BEQ KEY0_PRESSED				// THEN: Branch to tag KEY0_PRESSED
CONTINUE0:	

// Check if KEY1 is Press
		CMP	R0, #0x2					// IF: R0=0x2=0010 (KEY1 is Pressed)
		BEQ KEY1_PRESSED				// THEN: Branch to tag KEY1_PRESSED
CONTINUE1:	

// Check if KEY2 is Press
		CMP	R0, #0x4					// IF: R0=0x4=0100 (KEY2 is Pressed)
		BEQ KEY2_PRESSED				// THEN: Branch to tag KEY2_PRESSED
CONTINUE2:	

// Check if KEY3 is Press
		CMP	R0, #0x8					// IF: R0=0x8=1000 (KEY3 is Pressed)
		BEQ KEY3_PRESSED				// THEN: Branch to tag KEY3_PRESSED
CONTINUE3:


// Wait until KEY Pressed is Released
WAIT:	LDR	R0, [R2, #0x50]
		CMP	R0, #0
		BNE	WAIT
		
NO_KEY: B LOOP							// Branch back to tag "LOOP" on TOP



// WHEN KEY0-3  is Pressed:
KEY0_PRESSED:
		MOV	R1,	#Num_List
		LDR	R3, [R1]
		B	CONTINUE0
		
KEY1_PRESSED:
		CMP	R3, #0x67					// CHECK if CURRENTLY R3 is 0x67 OR digit "9" in 7 Segment Display (MAX)
		BEQ	KEY1_Cond1					// IF is, (CANNOT GO ABOVE 9) --> THEN go to tag "KEY1_Cond1" to decrement by 1 word. (So that incrementing by 1 word on the next line will make R3 remain at "9" on 7 Segment Display)
BACK1:		
		ADD	R1, #4						// Increment by 1 word
		LDR	R3, [R1]					// Load the new word to R3
		B	CONTINUE1
		
		
KEY2_PRESSED:
		CMP	R3, #0x3F					// CHECK if CURRENTLY R3 is 0x3F OR digit "0" in 7 Segment Display (MIN)
		BEQ	KEY2_Cond1					// IF is, (CANNOT GO BELOW 0) --> THEN go to tag "KEY2_Cond1" to increment by 1 word. (So that decremending by 1 word on the next line will make R3 remain at "0" on 7 Segment Display)
BACK2:		
		SUB	R1, #4						// Decrement by 1 word
		LDR	R3, [R1]					// Load the new word to R3
		B	CONTINUE2


KEY3_PRESSED:
		MOV	R1,	#Num_List
		LDR	R3, [R1,#40]
		MOV	R4, #1						// Set R4 to "1" When KEY3 is Pressed (R4 = Tracker that previously KEY3 is Pressed to TURN OFF 7 Segment Display)
		B	CONTINUE3



// KEY(1-3) Conditions: Special Cases
KEY1_Cond1:
		SUB R1, #4
		B	BACK1


KEY2_Cond1:
		ADD R1, #4
		B	BACK2


KEY3_Cond1:
		LDR	R0, [R2, #0x50]				// Load Content of Push Buttons into R0
		MOV	R4, #0						// Reser R4 (KEY3 Pressed/Not Pressed Tracker) to "0"
		
		// Determine if ANY KEY0-2 is pressed
		CMP	R0, #0x1					
		BEQ KEY3_Cond2
		CMP	R0, #0x2
		BEQ KEY3_Cond2
		CMP	R0, #0x4
		BEQ KEY3_Cond2
		
		// Keep branching to tag "KEY3_Cond1" IF NO KEY0-2 is pressed
		B KEY3_Cond1	

KEY3_Cond2:
		MOV	R1,	#Num_List
		LDR	R3, [R1]
		B	WAIT



// Number List	
Num_List: .word 0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x7, 0x7F, 0x67, 0x0
		  .end
		  