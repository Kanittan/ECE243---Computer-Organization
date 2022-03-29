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

// Begin part1.s for Lab 7

// Function Declaration
volatile int pixel_buffer_start; // global variable
void draw_line(int x0, int y0, int x1, int y1, short int colour);
void plot_pixel(int x, int y, short int line_color);
void clear_screen();

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;

    clear_screen();
	move_line(0, 0, 319, 0, 0xFFFF); //Move White Color Horizontal Line Up and Down
}

// code not shown for clear_screen() and draw_line() subroutines

void move_line(int x0, int y0, int x1, int y1, short int colour)
{
	while(1) //For Infinite Loop
	{
		for(int j=0; j<240; j++)
		{
			draw_line(x0, y0, x1, y1, colour);	//Draw a White Horiziontal Line starting from TOP
			wait_for_vsync();	// Request to draw to screen(VGA Pixel Buffer) + Check when drawing to screen is complete
			draw_line(x0, y0, x1, y1, 0x0);	// For Next Iteration: Draw Black to initial line)
			// Shift to next line (DOWNWARDS)
			y0 = y0+1;	// Increment y0 (Coordinate#1) by 1 to next bit
			y1 = y1+1;  // Increment y1 (Coordinate#2) by 1 to next bit
		}
	
	for(int j=240; j>0; j--)
		{
			draw_line(x0, y0, x1, y1, colour);	//Draw a White Horiziontal Line starting from BOTTOM (Since last horizontal line is @ Bottom of screen)
			wait_for_vsync();	// // Request to draw to screen(VGA Pixel Buffer) + Check when drawing to screen is complete
			draw_line(x0, y0, x1, y1, 0x0); // For Next Iteration: Draw Black to initial line)
			// Shift to next line (UPWARDS)
			y0 = y0-1;	// Increment y0 (Coordinate#1) by 1 to next bit
			y1 = y1-1;	// Increment y1 (Coordinate#2) by 1 to next bit
		}
	}
}

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

void clear_screen()
{
	for(int j=0; j<240; j++)
	{
		for(int i=0; i<320; i++)
		{
			plot_pixel(i,j,0x0);	//Clear screen (Make Screen Black)
		}
	}
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}
