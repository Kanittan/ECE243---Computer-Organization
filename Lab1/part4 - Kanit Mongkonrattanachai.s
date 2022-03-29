/* Program that converts a binary number to decimal */
           
           .text               // executable code follows
           .global _start
_start:
            MOV    R4, #N       
            MOV    R5, #Digits  // R5 to the decimal digits storage location
            LDR    R4, [R4]     // R4 holds N
            MOV    R0, R4       // parameter for DIVIDE goes in R0
			
Sone:       MOV R2, #0 //R2 contains quotient

Loop:       CMP R0, #10 //comparing R0, 10
            BLT Stwo    //if R0 < 10, then branch to Stwo
            SUB R0, #10 //else R0 = R0-10
            ADD R2, #1  //R2 + 1
            B Loop     //to loop again

Stwo:       STRB R0, [R5] // store LSB of R0 to R5 address
            ADD R5, #1 // R5 +1 to the next location
            MOV R0, R2 // R2 to R0 to until quotientR2 is zero
            CMP R0, #10 //compare R0,10
            BLT Sthree  //if<10, brach to STHREE
            B Sone    //else back to sone agian

Sthree:     STRB R0, [R5]

END:        B END

N:          .word 9876
Digits:     .space 4

            .end
