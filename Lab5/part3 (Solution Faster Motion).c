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
#include <math.h>	//Included so that I can used sqrt

// Begin part3.c code for Lab 7

// Function Declaration
volatile int pixel_buffer_start; // global variable
void wait_for_vsync();
void draw_line(int x0, int y0, int x1, int y1, short int colour);
void plot_pixel(int x, int y, short int line_color);
void clear_screen();

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
	int x[8];
	int y[8];
	short int colour[8];
	int dx[8];
	int dy[8];
	
	//int colour = 0xFFFF; //Init colour = whote for now --> CHANGE using rand()
	for(int i=0; i<8; i++)
	{
		colour[i] = rand() | 0x4208; // colour[i] = rand() | 0x4208; to ensure that the 2nd most significant bit of the each color R,G,B is always "1" (ON) to make color brighter
		//OR: color[i] = rand();
	}
	
    // initialize location and direction of rectangles(not shown)
	// Initialize the first 2 points.
	for (int i = 0; i < 8; i++){
		x[i] = rand() % 320; // Set x=[0,320]
		y[i] = rand() % 240; // Set x=[0,240]
		dx[i] = rand() % 9 + 1;	// Make rand value in range = [0+1, 1+8] = [0,9]
		dy[i] = rand() % 9 + 1; // Make rand value in range = [0+1, 1+8] = [0,9]
		
		//To get Negative direction of dx & dy
		// And rand() with 0x...0001 to determine based on the 1st/least signiciant bit.
		// Since the if we write the bit from 0000 to 0001 to 0010 to ... 1111, we see that
		// each bit gets EQUAL number of "0" bit and "1" bit.
		// Therefore: Assuming that rand will give us a 50/50 chance of getting either a "1"
		// OR a "0" for each bit --> We use this fact that set dx & dy to "Negative Direction" if
		// the 1st/least significant bit is set to "1".
		if (rand() & 1) dx[i] = -dx[i];	// Check
		if (rand() & 1) dy[i] = -dy[i];
	}
	
    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
	// After wait_for_vsync() is called, the contents(which is the Base Address of
	// 0xC8000000 & 0xC0000000 is swapped.
	// In Line 92: 	   content of ptr to front buffer = *(pixel_ctrl_ptr) = "blank"
	//			   	   content of ptr to back buffer  = *(pixel_ctrl_ptr + 1) = 0xC8000000
	
	// After Line 95:  content of ptr to front buffer = *(pixel_ctrl_ptr) = "0xC8000000"
	//			   	   content of ptr to back buffer  = *(pixel_ctrl_ptr + 1) = "blank"
	
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    //pixel_buffer_start = *pixel_ctrl_ptr;
    //clear_screen(); // pixel_buffer_start points to the pixel buffer
	
	
    /* set back pixel buffer to start of SDRAM memory at address 0xC0000000 */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
	// Let pixel_buffer_start = Essentially acting like a pointer, pointing to the "Back Buffer", EXCEPT that it doesn't store the updated value of the content of that pointer if the content of the pointer was changed.
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	// pixel_buffer_start <-gets content "0xC0000000" (Since *(pixel_ctrl_ptr + 1) = Content of ptr *(pixel_ctrl_ptr + 1) = 0xC0000000)
	
    while (1)
    {
        /* Erase any boxes and lines that were drawn in the last iteration */
        clear_screen();

        // code for drawing the boxes and lines (not shown)
		for(int i=0; i<8; i++)
		{
			if(i!=7)
			{
				draw_line(x[i], y[i],x[i+1], y[i+1], colour[i]); 
			}
			else
			{
				draw_line(x[i], y[i],x[0], y[0], colour[i]); 
			}
		}
		
		
		// Code for updating the locations of boxes (BELOW)
		for(int i=0; i<8; i++ )
		{
			// Increment x[i] by dx[i]
			x[i] = x[i] +dx[i];
			
			// Check LEFT & RIGHT Boundary
			// When hit LEFT/RIGHT Surface: dx = CHANGES, dy = Unchanged
			if(x[i]<0)
			{
				x[i]=0;
				dx[i] = -dx[i];
			}
			if(x[i]>319)
			{
				x[i]=319;
				dx[i] = -dx[i];
			}
		}
		
		for(int i=0; i<8; i++ )
		{
			// Increment y[i] by dy[i]
			y[i] = y[i] +dy[i];
			
			// Check TOP & BOTTOM Boundary
			// When hit LEFT/RIGHT Surface: dx = Unchanged, dy = CHANGES
			if(y[i]<0)
			{
				y[i]=0;
				dy[i] = -dy[i];
			}
			if(y[i]>239)
			{
				y[i]=239;
				dy[i] = -dy[i];
			}
		}
		
		
		// Switch Front and Back Buffer
		// Place in what is drawn into Back Buffer into the Front Buffer for displaying into the screen.
		// SINCE we can ONLY Display the screen THROUGH (via) Front Buffer ONLY (Cannot by Back Buffer)
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		
		// NOW: pixel_buffer_start <-STILL gets/holds the previously stored content, that is "0xC0000000" in Line 112.
		// *(pixel_ctrl_ptr + 1), that is the content at
		
		// From previous(Line 177), the pixel_buffer_start still holds the content of 0xC0000000, BUT after "wait_for_vsync()"
		// is called in Line 177, the content of front and back buffer swaps:
		// 1. Write content FROM 0xC0000000 into Front Buffer --> Display yo VGA Screen
		// 2. Back Buffer has content 0xC8000000 NOW (Instead of 0xC0000000)
		
		// SINCE we want the pixel_buffer_start to AWLAYS point/holds the value of Back Buffer, we have to reset it:
		// NOTE: *(pixel_ctrl_ptr + 1) = Content of Back Buffer (Which is now: *(pixel_ctrl_ptr + 1) = 0xC8000000)
		// 		 THEREFORE: pixel_buffer_start = 0xC8000000
		//       Why do we need to reset? Because draw_pixel() function BELOW USES "pixel_buffer_start" which we are ALWAYS "Writing" to.
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
		
		// Repeat Process/ Reiterate Loop:
		// - Keep swapping 0xC0000000 AND 0xC8000000.
		// - Keep writing to the Back Buffer indicated by address that "pixel_buffer_start" holds (draw_pixel() function uses pixel_buffer_start)
		// 	 --> NOTE: Back Buffer = Content pixel_buffer_start holds = Can be EITHER "0xC0000000" OR "0xC8000000")
		// - Keep "swapping" Front Buffer with Back Buffer after every iteration (every loop a screen is drawn completely)
    }
}

// code for subroutines (not shown)
void wait_for_vsync()
{
	volatile int *pixel_ctrl_ptr = 0xFF203020; //Pixel Controller
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


void plot_pixel(int x, int y, short int line_color)
{
	//Code Below
	// 1. Convert coordinate to address --> Essentially: A specific coordiante of 1 pixel in screen is given by a specific Address which uses the BELOW FORMULA to compute for this specific Address.
	// 2. Write Pixel Color "line_color" to that coordinate --> That's how we are able to see the line visisble on the VGA
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}


void clear_screen()
{
	// int = 32 bits
	// short int = 16 bits since color-pixel has only 16 bits
	
	short int *base = (short int*)pixel_buffer_start; // pixel_buffer_start ALWAYS hold the Address of the BACK BUFFER, regardless of whether that addres if "0xC0000000" OR "0XC800000".
	
	// Draw Black color "...0000" (16 Bits of 0) to EVERY single pixel on the Screen
	for(int j=0; j< 240 * (1<<9); j++)	// NOTE: 240 * 2^9 = #Row Bits x #Column Bits = Total Bits on Buffer Screen
	{
		base[j] = 0x0;
	}
}
