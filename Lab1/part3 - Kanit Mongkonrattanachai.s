/* Program that finds the largest number in a list of integers	*/
            
            .text                   // executable code follows
            .global _start                  
_start:                             
            MOV     R4, #RESULT     // R4 points to result location
            LDR     R0, [R4, #4]    // R0 holds the number of elements in the list
            MOV     R1, #NUMBERS    // R1 points to the start of the list
            BL      LARGE           
            STR     R0, [R4]        // R0 holds the subroutine return value

END:        B       END             

/* Subroutine to find the largest integer in a list
 * Parameters: R0 has the number of elements in the list
 *             R1 has the address of the start of the list
 * Returns: R0 returns the largest item in the list */

// Based on LOOP code from part2.s
LARGE:      SUBS    R0, #1        // Decrements the loop counter by 1
            BEQ     FINISH        // IF: R0-1 = 0 (Count ALL Numbers in List), THEN: Branch to FINISH        
            LDR     R2, [R1], #4  // Post-Index: 1.)R2<- R1 AND 2.)R1<-R1+4
            CMP     R3, R2        // Check if a larger number is found
            BGE     LARGE		  // Branch to LARGE IF: R3>R2
            MOV     R3, R2        // (WHEN: R3<R2) Update the Larger number(R2) to R3
            B       LARGE		  // Branch to LARGE (Repeat in Loop)
            
FINISH:     MOV     R0, R3      // Stores Largest Number into result location R0
            MOV     PC, LR      // PC<-LR (RETURN FROM SUBROUTINE to the STR instruction in line 10 above)

RESULT:     .word   0           
N:          .word   7           // number of entries in the list
NUMBERS:    .word   4, 5, 3, 6  // the data
            .word   1, 8, 2                 

            .end