/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define BOX_LEN 2
#define NUM_BOXES 8

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

////////////// CHANGE #Pixel to increment to MOVE PLayer HERE!!! //////////////
// Constants for Player
#define PLAYER_MOVE_INCR 1
BALL_MOVE_INCR_value = 1;

#define BALL_MOVE_INCR 1
/////////////////////////////////////////////////
	

// Function Declaration
volatile int pixel_buffer_start; // global variable
void wait_for_vsync();
void draw_line(int x0, int y0, int x1, int y1, short int colour);
void plot_pixel(int x, int y, short int line_color);
void clear_screen();


// Function Declaration/Prototype for Player:
void draw_Player();
void erase_Player();
void draw_Head(int x0, int y0, int colour);
void draw_Neck(int x0, int y0, int colour);
void draw_Body(int x0, int y0, int colour);
void draw_Arm(int x0, int y0, int colour);	//Right Arm and Left Arm
void draw_Leg(int x0, int y0, int colour);	//Right Leg and Left Leg

// Function Declaration/Prototype for Ball:
void draw_Ball(int player_loc_x, int player_loc_y);
void erase_Ball(int player_loc_x, int player_loc_y);
void shoot_Ball(int x0, int y0, int x1, int y1, int colour);
bool move_Ball_valid(int current_element);
bool pixel_is_Black(int x0, int y0);

// Function Declaration/Prototype for Player's Movement:
void move_player_Top();
void move_player_Bot();
bool player_Position_valid(char x);	//Check whether Player's movement TOP AND BOTTOM is valid to move or not. IF: Is valid, THEN: Move TOP/BOTTOM. IF: NOT valid, THEN: STAY AS Player is (DON'T MOVE TOP/BOTTOM).
bool player_hits_Object();

// Function Declaration/Prototype for Draw Wall:
void draw_wall(int x0, int y0, int x1, int y1,int colour);
void draw_pattern1(int x0, int y0, int colour,int goal_colour, int inside_goal_colour);
void draw_pattern2(int x0, int y0, int colour,int goal_colour, int inside_goal_colour);
void draw_pattern3(int x0, int y0, int colour,int goal_colour, int inside_goal_colour);
void draw_pattern4(int x0, int y0, int colour,int goal_colour, int inside_goal_colour);
void draw_goal(int x0, int y0, int x1, int y1, int colour, int colour_inside_goal);
// Function Declaration/Prototype for Displaying Score on HEX Display:
void display_Score(int current_Score);
void display_Win();

// Function Declaration/Prototype for PS2 Keyboard Key Interrupt:
void config_PS1();
void config_PS2();
void config_KEYs();
//void __attribute__ ((interrupt)) __cs3_isr_irq (void);
//void __attribute__ ((interrupt)) __cs3_isr_undef (void);
//void __attribute__ ((interrupt)) __cs3_isr_swi (void);
//void __attribute__ ((interrupt)) __cs3_isr_pabort (void);
//void __attribute__ ((interrupt)) __cs3_isr_dabort (void);
//void __attribute__ ((interrupt)) __cs3_isr_fiq (void);
void __attribute__ ((interrupt)) __cs3_isr_undef (void);
void __attribute__ ((interrupt)) __cs3_isr_swi (void);
void __attribute__ ((interrupt)) __cs3_isr_pabort (void);
void __attribute__ ((interrupt)) __cs3_isr_dabort (void);
void __attribute__ ((interrupt)) __cs3_isr_fiq (void);

void disable_A9_interrupts(void);
void enable_A9_interrupts(void);
void config_GIC(void);
void config_interrupt (int N, int CPU_target);
void set_A9_IRQ_stack(void);
void PS1_ISR();
void PS2_ISR();
void pushbutton_ISR();


////////// CHANGE STARTING POSITION HERE!!! //////////
// Global Variable:
// Pointer storing Player's Current Position (x,y)
int player_loc_x = 100;	// KEY: STARTING X-POSITION OF PLAYER
int player_loc_y = 100; // KEY: STARTING Y-POSITION OF PLAYER - TEST TOP SCREEN LIMIT: Set y0=18 | TEST BOTTOM SCREEN LIMIT: Set y0=221
int *player_curr_loc_x = &player_loc_x;
int *player_curr_loc_y = &player_loc_y;

// Pointer storing Player Directon (TOP/BOTTOM)
int player_direction_Top = 0;	// KEY: STARTING X-POSITION OF PLAYER
int player_direction_Bot = 0; // KEY: STARTING Y-POSITION OF PLAYER - TEST TOP SCREEN LIMIT: Set y0=18 | TEST BOTTOM SCREEN LIMIT: Set y0=221
int *player_curr_direction_Top = &player_direction_Top;
int *player_curr_direction_Bot = &player_direction_Bot;


// Global Array to keep track of the SHOOTED Ball Location (x,y)
int pos_x_Ball_arr[10];
int pos_y_Ball_arr[10];
int current_element_Ball_arr = 0; // Current Element in array pos_x_Ball_arr[10]
int current_num_element_in_arr = 0;

bool shoot = false;

// Global Pointer to Keep Track of Current Score When scores:
int current_Score = 0;
int *current_Score_ptr = &current_Score;

// Global Pointer to Keep Track of Wall:
int incr_Wall = 0;
int *incr_Wall_ptr = &incr_Wall;
int incr_Wall_2 = 0;
int *incr_Wall_ptr_2 = &incr_Wall_2;
int incr_Wall_3 = 0;
int *incr_Wall_ptr_3 = &incr_Wall_3;
int incr_Wall_4 = 0;
int *incr_Wall_ptr_4 = &incr_Wall_4;

bool pattern1_is_called = true;	// Set 1st Pattern to True to display only the first pattern
bool pattern2_is_called = false;
bool pattern3_is_called = false;
bool pattern4_is_called = false;

// Global Pointer for Game Over
bool game_Over = false;
/////////////////////////////////////////////////////////////



int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;
	
	// BEGIN: 1st: Make Background Screen black
	clear_screen();
	
	// START OF GAME: Draw Player + Ball at the STARTING Position of the Game:
	draw_Player();
	draw_Ball(*player_curr_loc_x+4, *player_curr_loc_y+9);
	
	disable_A9_interrupts();
 	set_A9_IRQ_stack();
 	config_GIC();
 	config_PS1();
 	config_PS2();
	config_KEYs();
 
 	enable_A9_interrupts();
	
	while(game_Over == false)
	{		
			//Erase Wall:
			//Write Code Here
			draw_pattern1(308- *incr_Wall_ptr, 0, 0x0, 0x0, 0x0);
			draw_pattern2(308 - *incr_Wall_ptr_2, 0, 0x0, 0x0, 0x0);
			draw_pattern3(308- *incr_Wall_ptr_3, 0, 0x0, 0x0, 0x0);
			draw_pattern4(308 - *incr_Wall_ptr_4, 0, 0x0, 0x0, 0x0);
			
			//Erase Player+Ball at Player's RIght Foot
			if((*player_curr_loc_y-15 >=0) || (*player_curr_loc_y+15 <=239))
			{
				erase_Player();	//Colour Player black color like background
				erase_Ball(*player_curr_loc_x+4, *player_curr_loc_y+9); //Colour Ball black color like background
			}
			
			//Erase Shooting/Moving Ball
			int i;
			for(i=0+current_element_Ball_arr; i<current_num_element_in_arr; i++)	//pos_x_Ball_arr[] and pos_y_Ball_arr[] has same size.
			{
				erase_Ball(pos_x_Ball_arr[i], pos_y_Ball_arr[i]);
			}
			
			
			//Draw Wall:
			if(pattern1_is_called == true)
			{
				if(*incr_Wall_ptr < 308)
				{
					if(*incr_Wall_ptr == 150) //Initial: Set to 100
					{
						pattern2_is_called = true;
					}
					
					*incr_Wall_ptr = *incr_Wall_ptr + 1;
					draw_pattern1(308 - *incr_Wall_ptr, 0, 0xf1ff, 0x1fff, 0x1ffe);
				}
				else
				{
					*incr_Wall_ptr = 0;
					pattern1_is_called = false;
				}
			}
			
			
			if(pattern2_is_called == true)
			{
				if(*incr_Wall_ptr_2 < 308)
				{
					if(*incr_Wall_ptr_2 == 150) //Initial: Set to 100
					{
						pattern3_is_called = true;
					}
					
					*incr_Wall_ptr_2 = *incr_Wall_ptr_2 + 1;
					draw_pattern2(308 - *incr_Wall_ptr_2, 0, 0xf1ff, 0x1fff, 0x1ffe);
				}
				else
				{
					*incr_Wall_ptr_2 = 0;
					pattern2_is_called = false;
				}
			}
			
			
			if(pattern3_is_called == true)
			{
				if(*incr_Wall_ptr_3 < 308)
				{
					if(*incr_Wall_ptr_3 == 150) //Initial: Set to 100
					{
						pattern4_is_called = true;
					}
					
					*incr_Wall_ptr_3 = *incr_Wall_ptr_3 + 1;
					draw_pattern3(308 - *incr_Wall_ptr_3, 0, 0xf1ff, 0x1fff, 0x1ffe);
				}
				else
				{
					*incr_Wall_ptr_3 = 0;
					pattern3_is_called = false;
				}
			}
			
		
			if(pattern4_is_called == true)
			{
				if(*incr_Wall_ptr_4 < 308)
				{
					if(*incr_Wall_ptr_4 == 150) //Initial: Set to 100
					{
						pattern1_is_called = true;
					}
					
					*incr_Wall_ptr_4 = *incr_Wall_ptr_4 + 1;
					draw_pattern4(308 - *incr_Wall_ptr_4, 0, 0xf1ff, 0x1fff, 0x1ffe);
				}
				else
				{
					*incr_Wall_ptr_4 = 0;
					pattern4_is_called = false;
				}
			}
		
			////////////////////////////////////
			
		
			// Increment the Ball to the right:
			int k;
			for(k=0+current_element_Ball_arr; k<current_num_element_in_arr; k++)	//pos_x_Ball_arr[] and pos_y_Ball_arr[] has same size.
			{
				if(move_Ball_valid(k) == true)
				{
					//printf("Move Valid!\n");
					pos_x_Ball_arr[k] = pos_x_Ball_arr[k] + BALL_MOVE_INCR;
					draw_Ball(pos_x_Ball_arr[k], pos_y_Ball_arr[k]);
				}
				else
				{
					//if((*(short int *)(pixel_buffer_start + (pos_y_Ball_arr[k] << 10) + ((pos_x_Ball_arr[k]+6+1) << 1))) == 0xf1ff)
					//{
					//	printf("Hits Wall!\n");
					//}
					//printf("Move NOT Valid!\n");
					current_element_Ball_arr = current_element_Ball_arr + 1;
				}
				//else	//if move_Ball_valid(k) == false (Hits Wall or Goal)
				//{
					
				//}
			}
			
			
			if(*player_curr_direction_Top != 0)
			{
				*player_curr_loc_y = (*player_curr_loc_y) + (*player_curr_direction_Top);
			}
			
			if(*player_curr_direction_Bot != 0)
			{
				*player_curr_loc_y = (*player_curr_loc_y) + (*player_curr_direction_Bot);
			}
			
			// Update HEX Display of Score:
			display_Score(*current_Score_ptr);
			
			//Game is Over When Player hits Wall OR Goal
			if(player_hits_Object() == true)
			{
				game_Over = true;
			}
			
			if(*current_Score_ptr == 10)
			{
				display_Win();
				game_Over = true;
			}
			
			
			
			//Draw Player and Ball at Player's Right Foot
			draw_Player();
			draw_Ball(*player_curr_loc_x+4, *player_curr_loc_y+9);
			
			// Draw Shooted Ball - WHen Pushbutton KEY0 is Pressed
			//for(int j=0; j<current_num_element_in_arr; j++)	//pos_x_Ball_arr[] and pos_y_Ball_arr[] has same size.
			//{
			//	draw_Ball(pos_x_Ball_arr[j], pos_y_Ball_arr[j]);
			//}
			
			wait_for_vsync();
			*player_curr_direction_Top = 0;
			*player_curr_direction_Bot = 0;
			//*incr_Wall_ptr = *incr_Wall_ptr + 1;
	}
	
}



/////////	BELOW: Functions for Player	/////////

// Draw Player
void draw_Player()
{
	draw_Body(*player_curr_loc_x-3, *player_curr_loc_y-5, 0xFFFF);
	draw_Head(*player_curr_loc_x-3, *player_curr_loc_y-15, 0xFFFF);
	draw_Neck(*player_curr_loc_x-1, *player_curr_loc_y-8, 0xFFFF);
	draw_Arm(*player_curr_loc_x+4, *player_curr_loc_y-5, 0xFFFF);	//Right Arm
	draw_Arm(*player_curr_loc_x-8, *player_curr_loc_y-5, 0xFFFF);	//Left Arm
	draw_Leg(*player_curr_loc_x+1, *player_curr_loc_y+6, 0xFFFF);	//Right Leg
	draw_Leg(*player_curr_loc_x-3, *player_curr_loc_y+6, 0xFFFF);	//Left Leg
}

// Erase Player
void erase_Player()
{
	draw_Body(*player_curr_loc_x-3, *player_curr_loc_y-5, 0x0);
	draw_Head(*player_curr_loc_x-3, *player_curr_loc_y-15, 0x0);
	draw_Neck(*player_curr_loc_x-1, *player_curr_loc_y-8, 0x0);
	draw_Arm(*player_curr_loc_x+4, *player_curr_loc_y-5, 0x0);	//Right Arm
	draw_Arm(*player_curr_loc_x-8, *player_curr_loc_y-5, 0x0);	//Left Arm
	draw_Leg(*player_curr_loc_x+1, *player_curr_loc_y+6, 0x0);	//Right Leg
	draw_Leg(*player_curr_loc_x-3, *player_curr_loc_y+6, 0x0);	//Left Leg
}

// Draw Head LxW = 7x7 pixel
void draw_Head(int player_loc_x, int player_loc_y, int colour)
{
	int j;
	for(j=0; j<7; j++)
	{
		draw_line(player_loc_x, player_loc_y+j, player_loc_x+7, player_loc_y+j, colour);
	}
}

// Draw Body LxH = 7x11 pixel
void draw_Body(int player_loc_x, int player_loc_y, int colour)
{
	int j;
	for(j=0; j<11; j++)
	{
		draw_line(player_loc_x, player_loc_y+j, player_loc_x+7, player_loc_y+j, colour);
	}
}

// Draw Neck LxH = 3x3 pixel
void draw_Neck(int player_loc_x, int player_loc_y, int colour)
{
	int j;
	for(j=0; j<3; j++)
	{
		draw_line(player_loc_x, player_loc_y+j, player_loc_x+3, player_loc_y+j, colour);
	}
}

// Draw Arm LxH = 5x3
void draw_Arm(int player_loc_x, int player_loc_y, int colour)
{
	int j;
	for(j=0; j<3; j++)
	{
		draw_line(player_loc_x, player_loc_y+j, player_loc_x+5, player_loc_y+j, colour);
	}
}

// Draw Leg LxH = 3x10 (Changed from 3x5 to 3x10)
void draw_Leg(int player_loc_x, int player_loc_y, int colour)
{
	int j;
	for(j=0; j<10; j++)
	{
		draw_line(player_loc_x, player_loc_y+j, player_loc_x+3, player_loc_y+j, colour);
	}
}

/////////////////////////////////////////////////




/////////	BELOW: Functions for Ball	/////////

// Draw Ball LxH = 7x7
void draw_Ball(int player_loc_x, int player_loc_y)
{
	int j;
	for(j=0; j<7; j++)
	{
		draw_line(player_loc_x, player_loc_y+j, player_loc_x+7, player_loc_y+j, 0xFF00);
	}
}

// Erase Ball
void erase_Ball(int player_loc_x, int player_loc_y)
{
	int j;
	for(j=0; j<7; j++)
	{
		draw_line(player_loc_x, player_loc_y+j, player_loc_x+7, player_loc_y+j, 0x0);
	}
}

/////////////////////////////////////////////////




/////////	BELOW: Functions for Player Movement: TOP and BOT	/////////

// Move Player TOP WHEN "Arrow UP" Keyboard Key is Pressed.
void move_player_Top()
{
		draw_Player();
		draw_Ball(*player_curr_loc_x+4, *player_curr_loc_y+9);
		wait_for_vsync();
		erase_Player();	//Colour Player black color like background
		erase_Ball(*player_curr_loc_x+4, *player_curr_loc_y+9); //Colour Ball black color like background	
		
		if(player_Position_valid('T') == true)	//
		{
			*player_curr_loc_y = *player_curr_loc_y - PLAYER_MOVE_INCR; //Decrement y ONLY SINCE MOVE TOP, where PLAYER_MOVE_INCR = Global Constant = #Pixel used to increment the Player's movement UP/DOWN when Keyboard Key is pressed.
		}
}

// Move Player BOT WHEN "Arrow UP" Keyboard Key is Pressed.
void move_player_Bot()
{
	draw_Player();
	draw_Ball(*player_curr_loc_x+4, *player_curr_loc_y+9);
	wait_for_vsync();
	erase_Player();
	erase_Ball(*player_curr_loc_x+4, *player_curr_loc_y+9);
	
	if(player_Position_valid('B') == true)	//
	{
		*player_curr_loc_y = *player_curr_loc_y + PLAYER_MOVE_INCR; //Increment y ONLY SINCE MOVE BOT, where PLAYER_MOVE_INCR = Global Constant = #Pixel used to increment the Player's movement UP/DOWN when Keyboard Key is pressed.
	}
}


bool player_Position_valid(char x)
{
	// Check the next incoming movement after press Arrow UP OR Arrow DOWN Key
	// IF: *player_curr_loc_y-15-20 >=0, where:
	// 	   1) *player_curr_loc_y-15 = Top Border of Player's Head
	//	   2) "PLAYER_MOVE_INCR" = GLOBAL CONSTANT = #Pixel used to increment the Player's movement UP/DOWN when Keyboard Key is pressed.
	if((x == 'T') && ((*player_curr_loc_y-15-PLAYER_MOVE_INCR) >= 0))
	{
		
		return true;
	}
	
	if((x == 'B') && ((*player_curr_loc_y+15+PLAYER_MOVE_INCR) <= 239))
	{
		
		return true;
	}
	
	return false;
}

bool player_hits_Object()
{
	int i;
	for(i=0; i<31; i++)
	{
		if(pixel_is_Black(*player_curr_loc_x+10, *player_curr_loc_y+i) == true)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}
/////////////////////////////////////////////////


/////////	BELOW: Functions for BALL Movement: HORIZONTAL TO RIGHT	/////////
void shoot_Ball(int x0, int y0, int x1, int y1, int colour)//repalce short int colour with int colour
{
	// FIRST: STORE the INITIAL Position of the ball where it is shooted by Player into Global Array shooted_Ball_array
	//shooted_Ball_Arr_pos_x[current_Pos_x_in_Ball_Arr] = x0-1; //x0 = Initial x-Position of TOP RIGHT CORNER OF BALL
	//current_Pos_x_in_Ball_Arr = current_Pos_x_in_Ball_Arr + 1;	//Increment to Next Position in array
	
	// SECOND: Move the Ball Horizontally to the RIGHT
	int i;
	int j;
	int k;
	for(i=0; i<320; i++)
	{
		for(j=0; j<7; j++)	// Draw a 7x7 Block moving
		{
			draw_line(x0, y0+j, x1, y1+j, colour);	//Draw a White Horiziontal Line starting from TOP
		}
		
		//draw_Ball(*player_curr_loc_x+4, *player_curr_loc_y+9);	
		wait_for_vsync();	// Request to draw to screen(VGA Pixel Buffer) + Check when drawing to screen is complete
		//erase_Ball(*player_curr_loc_x+4, *player_curr_loc_y+9);
		
		
		for(k=0; k<7; k++)	// Draw a 7x7 Block moving
		{
			draw_line(x0, y0+k, x1, y1+k, 0x0);	//Draw a White Horiziontal Line starting from TOP
		}
		// Shift to next line (DOWNWARDS)
		//y0 = y0+1;	// Increment y0 (Coordinate#1) by 1 to next bit
		//y1 = y1+1;  // Increment y1 (Coordinate#2) by 1 to next bit
		
		// Ball's Speed: Increment 1 pixel per frame
		x0 = x0+1;
		x1 = x1+1;
	}
}



//// NOTE: FINISH CODE BELOW AND INCLUDE IT TO CHECK WHEN BALL HITS OBJECTS:

bool move_Ball_valid(int current_element)
{
	//if((pixel_is_Black(pos_x_Ball_arr[current_element]+6, pos_y_Ball_arr[current_element]) == true) && (pixel_is_Black(pos_x_Ball_arr[current_element]+6, pos_y_Ball_arr[current_element]+1) == true) && (pixel_is_Black(pos_x_Ball_arr[current_element]+6, pos_y_Ball_arr[current_element]+2) == true) && (pixel_is_Black(pos_x_Ball_arr[current_element]+6, pos_y_Ball_arr[current_element]+3) == true) && (pixel_is_Black(pos_x_Ball_arr[current_element]+6, pos_y_Ball_arr[current_element]+4) == true) && (pixel_is_Black(pos_x_Ball_arr[current_element]+6, pos_y_Ball_arr[current_element]+5) == true) && (pixel_is_Black(pos_x_Ball_arr[current_element]+6, pos_y_Ball_arr[current_element]+6) == true))
	if((pixel_is_Black(pos_x_Ball_arr[current_element]+6+1, pos_y_Ball_arr[current_element]) == true) && (pixel_is_Black(pos_x_Ball_arr[current_element]+6+1, pos_y_Ball_arr[current_element]+1) == true) && (pixel_is_Black(pos_x_Ball_arr[current_element]+6+1, pos_y_Ball_arr[current_element]+2) == true) && (pixel_is_Black(pos_x_Ball_arr[current_element]+6+1, pos_y_Ball_arr[current_element]+3) == true) && (pixel_is_Black(pos_x_Ball_arr[current_element]+6+1, pos_y_Ball_arr[current_element]+4) == true) && (pixel_is_Black(pos_x_Ball_arr[current_element]+6+1, pos_y_Ball_arr[current_element]+5) == true) && (pixel_is_Black(pos_x_Ball_arr[current_element]+6+1, pos_y_Ball_arr[current_element]+6) == true))
	{
		return true;
	}
	else
	{
		//for(int i=0; i<7 ;i++)
		//{
		//	if((*(short int *)(pixel_buffer_start + ((pos_y_Ball_arr[current_element]+i) << 10) + ((pos_x_Ball_arr[current_element]+6+1) << 1))) == 0xFFFF)
		//	{
		//		printf("Hits Wall\n");
		//		return false;
		//	}
		//}
		return false;
	}
}


bool pixel_is_Black(int x0, int y0)
{
	
	//Return true IF: The pixel is Black Colour (Background Colour)
	if((*(short int *)(pixel_buffer_start + (y0 << 10) + (x0 << 1))) == 0x0)
	{
		//printf("Hits Nothing1\n");
		//printf("Hits Nothing2\n");
		return true;	//Return True IF Pixel is Black
	}
	
	//printf("Pixel Color: %d\n", (*(short int *)(pixel_buffer_start + (y0 << 10) + (x0 << 1))));
	if((*(short int *)(pixel_buffer_start + (y0 << 10) + (x0 << 1))) == (short int)0xf1ff)
	{
		printf("Hits Wall\n");
		return false;
	}
	
	//IF: Hits Inside Goal (Blue Color), THEN: Count Score AND remove the ball from screen
	if((*(short int *)(pixel_buffer_start + (y0 << 10) + (x0 << 1))) == (short int)0x1ffe)
	{
		printf("Hits Goal\n");
		*current_Score_ptr = *current_Score_ptr + 1;
		printf("Score: %d\n", *current_Score_ptr);
		return false;
	}
	
	return false;
	//else//All other condditions like ht border of screen.
	//{
	//	return false;
	//}
	
}


//void plot_pixel(int x, int y, short int line_color)
//{
//    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
//}

/////////////////////////////////////////////////




/////////	BELOW: Functions for DRAWING PIXEL/LINE	/////////
void wait_for_vsync()
{
	volatile int *pixel_ctrl_ptr = (int*) 0xFF203020; //Pixel Controller
	register int status;
	
	*pixel_ctrl_ptr = 1; //Start the Sunchronization Process by writing 1 to it Front Buffer
	
	status = *(pixel_ctrl_ptr+3); // Check "S" bit (Indicate drawing on screen is done/completed)
	
	// Polled Status Register: Keep checking until the status register will be bit "1"
	while((status&0x001)!=0)
	{
		status = *(pixel_ctrl_ptr+3);
	}
}

void draw_line(int x0, int y0, int x1, int y1, short int colour)
{
	int is_steep = 0;
	if(abs(y1-y0) > abs(x1-x0))
	{
		is_steep = 1; //Set=1 when delta_y>delta_x
	}
	
	if(is_steep == 1)
	{
		int temp = x0;
		x0 = y0;
		y0 = temp;
		
		temp = x1;
		x1 = y1;
		y1 = temp;
	}
	
	if(x0>x1)// if(boolean_is_steep == 0)
	{
		int temp = x0;
		x0 = x1;
		x1 = temp;
		
		temp = y0;
		y0 = y1;
		y1 = temp;
	}
	
	////////////////////////
	int deltax = x1-x0;
	int deltay = abs(y1-y0);
	int error = -(deltax/2);
	int y_step;
	
	////////////////////////
	if(y0<y1)
	{
		y_step = 1;
	}
	else
	{
		y_step=-1;
	}
	
	////////////////////////
	int x=x0;
	int y=y0;
	
	while(x<x1)
	{
		if(is_steep == 1)
		{
			plot_pixel(y,x,colour);
		}
		else
		{
			plot_pixel(x,y,colour);
		}
		error = error+deltay;
		
		if(error > 0)
		{
			y = y+y_step;
			error = error-deltax;
		}
		x = x + 1; //Increment x
	}
	
}

void clear_screen()
{
	int j;
	int i;
	for(j=0; j<240; j++)
	{
		for(i=0; i<320; i++)
		{
			plot_pixel(i,j,0x0);	//Clear screen (Make Screen Black)
		}
	}
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

/////////////////////////////////////////////////




/////////	BELOW: Functions for Wall	/////////

void draw_wall(int x0, int y0, int x1, int y1, int colour)
{
	int i;
	for(i=0; i<11; i++)
	{
		draw_line(x0+i, y0, x1+i, y1, (short int)colour);
	}
}

void draw_goal(int x0, int y0, int x1, int y1, int colour, int colour_inside_goal)
{
	int i;
	int j;
	int k;
	for(i=0; i<11; i++)
	{
		if(i<=1)
		{
			draw_line(x0+i, y0, x1+i, y1, (short int)colour_inside_goal);
		}
		else
		{
			draw_line(x0+i, y0, x1+i, y1, (short int)colour);
		}
		
	}
	
	for(j=0; j<11; j++)
	{
		draw_line(x0-5+j, y0, x1-5+j, y0+8, (short int)colour);
	}
	
	for(k=0; k<11; k++)
	{
		draw_line(x0-5+k, y1-8, x1-5+k, y1, (short int)colour);
	}
	
}

// Set: x0= 308 AND y0 = 0
void draw_pattern1(int x0, int y0, int colour, int goal_colour, int inside_goal_colour)
{
	draw_wall(x0, y0, x0, y0+60, colour);
	draw_wall(x0, y0+60+60, x0, y0+60+60+70, colour);
}

void draw_pattern2(int x0, int y0, int colour, int goal_colour, int inside_goal_colour)
{
	draw_wall(x0, y0, x0, y0+35, colour);
	draw_goal(x0, y0+35+80, x0, y0+35+80+50, goal_colour, inside_goal_colour);
	draw_wall(x0, y0+35+80+50+50, x0, y0+35+80+50+50+24, colour);
}

void draw_pattern3(int x0, int y0, int colour, int goal_colour, int inside_goal_colour)
{
	draw_wall(x0, y0+40, x0, y0+40+30, colour);
	draw_wall(x0, y0+40+30+50, x0, y0+40+30+50+30, colour);
	draw_goal(x0, y0+40+30+50+30+50, x0, y0+40+30+50+30+50+30, goal_colour, inside_goal_colour);
}

void draw_pattern4(int x0, int y0, int colour, int goal_colour, int inside_goal_colour)
{
	draw_wall(x0, y0+10, x0, y0+10+20, colour);
	draw_goal(x0, y0+10+20+45, x0, y0+10+20+45+30, goal_colour, inside_goal_colour);
	draw_wall(x0, y0+10+20+45+30+45, x0, y0+10+20+45+30+45+20, colour);
	draw_wall(x0, y0+10+20+45+30+45+20+45, x0, y0+10+20+45+30+45+20+45+20, colour);
	
}

/////////////////////////////////////////////////




/////////	BELOW: Functions for Interrupt from PS2 Keyboard Key	/////////

void display_Score(int current_Score)
{
	volatile int * HEX0_3 = (int *) 0xFF200020;
	volatile int * HEX4_5 = (int *) 0xFF200030;
	*HEX0_3 = 0x33000000;
	*HEX4_5 = 0x6D39;
	
	switch(current_Score)
	{
		case 0:
			*HEX0_3 = *HEX0_3 | 0x3F;
			break;
		case 1:
			*HEX0_3 = *HEX0_3 | 0x06;
			break;
		case 2:
			*HEX0_3 = *HEX0_3 | 0x5B;
			break;
		case 3:
			*HEX0_3 = *HEX0_3 | 0x4F;
			break;
		case 4:
			*HEX0_3 = *HEX0_3 | 0x66;
			break;
		case 5:
			*HEX0_3 = *HEX0_3 | 0x6D;
			break;
		case 6:
			*HEX0_3 = *HEX0_3 | 0x7D;
			break;
		case 7:
			*HEX0_3 = *HEX0_3 | 0x07;
			break;
		case 8:
			*HEX0_3 = *HEX0_3 | 0x7F;
			break;
		case 9:
			*HEX0_3 = *HEX0_3 | 0x6F;
			break;
		case 10:
			*HEX0_3 = *HEX0_3 | 0x063F;
			break;
		
		default:
			;
	}
		
}

void display_Win()
{
	volatile int * HEX0_3 = (int *) 0xFF200020;
	volatile int * HEX4_5 = (int *) 0xFF200030;
	*HEX0_3 = 0x1C7E1054;
	*HEX4_5 = 0x6E5C;
		
}

/////////////////////////////////////////////////




/////////	BELOW: Functions for Interrupt from PS2 Keyboard Key	/////////

void config_PS1()
{
	volatile int * PS1_Control = (int *) 0xFF200104;
	*PS1_Control = 0x00000001;  //enable interrupts for PS2
	printf("%#010x\n", *PS1_Control);
}

//Added Below
void config_PS2()
{
	volatile int * PS2_Control = (int *) 0xFF20010C;
	*PS2_Control = 0x00000001;  //enable interrupts for PS2
	printf("%#010x\n", *PS2_Control);
}

void config_KEYs()
{
	volatile int * KEY_ptr = (int *) 0xFF200050; // KEY base address
	*(KEY_ptr + 2) = 0xF; // enable interrupts for all four KEYs
}

void __attribute__ ((interrupt)) __cs3_isr_irq (void)
{
	printf("INTERRUPT DETECTED!\n");
	//volatile int * PS2_ptr = (int *) 0xFF200100;

	// Read the ICCIAR from the processor interface

	int int_ID = *((int *) 0xFFFEC10C);

	if (int_ID == 89)// check if interrupt is from the PS1
 	{
		PS2_ISR();
 	}
	
	else if(int_ID == 79)// check if interrupt is from the PS2
 	{
		PS1_ISR();
 	}
	
	else if (int_ID == 73) // check if interrupt is from the KEYs
	{
		pushbutton_ISR();
	}
  	else
  	{
		while (1){} // if unexpected, then stay here
  	}
 
 	// Write to the End of Interrupt Register (ICCEOIR)
 	*((int *) 0xFFFEC110) = int_ID;
 	return;
}

	
// Define the remaining exception handlers */
void __attribute__ ((interrupt)) __cs3_isr_undef (void)
{
	while (1);
}
void __attribute__ ((interrupt)) __cs3_isr_swi (void)
{
	while (1);
}
void __attribute__ ((interrupt)) __cs3_isr_pabort (void)
{
	while (1);
}
void __attribute__ ((interrupt)) __cs3_isr_dabort (void)
{
	while (1);
}
void __attribute__ ((interrupt)) __cs3_isr_fiq (void)
{
	while (1);
}


void disable_A9_interrupts(void)
{
	int status = 0b11010011;
	asm("msr cpsr, %[ps]" : : [ps]"r"(status));
}

void enable_A9_interrupts(void)
{
	int status = 0b01010011;
	asm("msr cpsr, %[ps]" : : [ps]"r"(status));
}

void config_GIC(void)
{
	config_interrupt (79, 1); // configure the KEYs parallel port (Interrupt ID = 73)
	config_interrupt (89, 1); // configure the KEYs parallel port (Interrupt ID = 73)
	config_interrupt (73, 1); // configure the FPGA KEYs interrupt (73)
	// Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all priorities
	*((int *) 0xFFFEC104) = 0xFFFF;
	// Set CPU Interface Control Register (ICCICR). Enable signaling of interrupts
	*((int *) 0xFFFEC100) = 1;
	// Configure the Distributor Control Register (ICDDCR) to send pending interrupts to CPUs
	*((int *) 0xFFFED000) = 1; 
}

void config_interrupt (int N, int CPU_target)
{
	int reg_offset, index, value, address;
	
	/* Configure the Interrupt Set-Enable Registers (ICDISERn). 
	* reg_offset = (integer_div(N / 32) * 4
	* value = 1 << (N mod 32) */

	reg_offset = (N >> 3) & 0xFFFFFFFC;
	index = N & 0x1F;
	value = 0x1 << index;
	address = 0xFFFED100 + reg_offset;

	/* Now that we know the register address and value, set the appropriate bit */ *(int *)address |= value;
	*(int *)address |= value;

	/* Configure the Interrupt Processor Targets Register (ICDIPTRn) 
	* reg_offset = integer_div(N / 4) * 4
	* index = N mod 4 */

	reg_offset = (N & 0xFFFFFFFC);
	index = N & 0x3;
	address = 0xFFFED800 + reg_offset + index;

	/* Now that we know the register address and value, write to (only) the appropriate byte */ 
	*(char *)address = (char) CPU_target;
}

void set_A9_IRQ_stack(void)
{
	int stack, mode;
	stack = 0xFFFFFFFF - 7;// top of A9 on-chip memory, aligned to 8 bytes 

	/* change processor to IRQ mode with interrupts disabled */
	mode = 0b11010010;
	asm("msr cpsr, %[ps]" : : [ps] "r" (mode));
	/* set banked stack pointer */
	asm("mov sp, %[ps]" : : [ps] "r" (stack));

	/* go back to SVC mode before executing subroutine return! */
	mode = 0b11010011;
	asm("msr cpsr, %[ps]" : : [ps] "r" (mode));

	//printf("Done setting up stack\n");
}


void PS1_ISR()
{
	printf("IN PS1_ISR\n");
	//LEDR For Testng
	volatile int * LEDR = (int *) 0xFF200000;
	volatile int * PS1_Control = (int *) 0xFF200104;
	*PS1_Control = 1;  // clear RI bit

	volatile int * PS1_ptr = (int *) 0xFF200100; // PS/2 port address
	int PS1_data, RVALID;
	char byte1, byte2, byte3 = 0;

	PS1_data = *(PS1_ptr); // read the Data register in the PS/2 port
	RVALID = PS1_data & 0x8000; // extract the RVALID field
	if (RVALID)
	{
		byte1 = byte2;
		byte2 = byte3;
		byte3 = PS1_data & 0xFF;
		//printf("%#010x\n", byte3);
		*LEDR = 0x1;
	}
	printf("Value of KEY: %d\n", *PS1_ptr);
	
	if((*PS1_ptr) == 117)
	{
		// Check if Player Hits TOP Edge of Wall
		if(player_Position_valid('T') == true)	//
		{
			// Decrement Direction Top by 1 to move TOP:
			*player_curr_direction_Top = *player_curr_direction_Top - 1;
			//*player_curr_loc_y = *player_curr_loc_y - PLAYER_MOVE_INCR; //Decrement y ONLY SINCE MOVE TOP, where PLAYER_MOVE_INCR = Global Constant = #Pixel used to increment the Player's movement UP/DOWN when Keyboard Key is pressed.
		}
		else
		{
			;	//Do nothing: DO NOT Increment *player_curr_direction_Top
		}
	}
}

void PS2_ISR()
{
	printf("IN PS2_ISR\n");
	
	//LEDR For Testng
	volatile int * LEDR = (int *) 0xFF200000;
	volatile int * PS2_Control = (int *) 0xFF20010C;
	*PS2_Control = 1;  // clear RI bit

	volatile int * PS2_ptr = (int *) 0xFF200108; // PS/2 port address
	int PS2_data, RVALID;
	char byte1, byte2, byte3 = 0;

	PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
	RVALID = PS2_data & 0x8000; // extract the RVALID field
	if (RVALID)
	{
		byte1 = byte2;
		byte2 = byte3;
		byte3 = PS2_data & 0xFF;
		//printf("%#010x\n", byte3);
		//*LEDR = 0xff;
	}
	
	printf("Value of KEY: %d\n", *PS2_ptr);
	
	if((*PS2_ptr) == 114)
	{
		// Check if Player Hits Bottom Edge of Wall
		if(player_Position_valid('B') == true)	//
		{
			// Decrement Direction Top by 1 to move TOP:
			*player_curr_direction_Bot = *player_curr_direction_Bot + 1;
			//*player_curr_loc_y = *player_curr_loc_y - PLAYER_MOVE_INCR; //Decrement y ONLY SINCE MOVE TOP, where PLAYER_MOVE_INCR = Global Constant = #Pixel used to increment the Player's movement UP/DOWN when Keyboard Key is pressed.
	}
		else
		{
			;	//Do nothing: DO NOT Increment *player_curr_direction_Bot
		}
	}
}


void pushbutton_ISR()
{
	printf("IN pushbutton_ISR\n");
	/* KEY base address */
	volatile int *KEY_ptr = (int *) 0xFF200050;
	/* HEX display base address */
	//volatile int *HEX3_HEX0_ptr = (int *) 0xFF200020;
	int press;
	press = *(KEY_ptr + 3); // read the pushbutton interrupt register
	*(KEY_ptr + 3) = press; // Clear the interrupt
	if (press & 0x2) // KEY0
	{
		//HEX_bits = 0b00111111;
		pos_x_Ball_arr[current_num_element_in_arr] = *player_curr_loc_x+4;
		pos_y_Ball_arr[current_num_element_in_arr] = *player_curr_loc_y+9;
		
		current_num_element_in_arr = current_num_element_in_arr + 1;
	}
	//return;
}

/////////////////////////////////////////////////
