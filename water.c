#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
	
//global variable declaration
volatile int pixel_buffer_start;

//helpper function declaration

void wait_screen(volatile int* pixel_ctrl_ptr);
void draw_water();
void draw_water_one(int x_position, int y_position);
void clear_water_one(int x_position, int y_position);


//waiting for the screen to finish drawing
void wait_screen(volatile int* pixel_ctrl_ptr){
    *pixel_ctrl_ptr = 1;
    register int status;
	status = *(pixel_ctrl_ptr + 3);
    while((status & 0x01) == 1){
        status = *(pixel_ctrl_ptr + 3);
    }
}

//helper function to plot one pixel with a color


//helper function to clear the screen to black color
	

int water[120] = {
// 'water', 8x15px
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x02f6, 0x02f6, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x03f9, 0x03f9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03b8, 0x351b, 0x2d1b, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x351b, 0x5d9c, 0x5d9c, 0x351b, 0x0000, 0x0000, 0x0000, 0x1c18, 0x65bc, 0x8e3d, 0x8e3d, 0x65bc, 0x0000, 0x0000, 
0x0000, 0x9ebe, 0x8e5d, 0xb6de, 0xb6de, 0x965d, 0x247a, 0x0000, 0x0000, 0xaebe, 0xcf1e, 0xcf1e, 0xcf1e, 0xc71e, 0xaebe, 0x0000, 
0xae9d, 0xc71e, 0xd75f, 0xd75f, 0xd75f, 0xdf5f, 0xd73e, 0x4479, 0xbf1f, 0xcf3e, 0xe77f, 0xe79f, 0xefbf, 0xefbf, 0xdf7f, 0xd73f, 
0xc73f, 0xdf5f, 0xefbf, 0xefbf, 0xf7df, 0xf7df, 0xefbf, 0xd75f, 0xd77f, 0xbf1f, 0xdf5f, 0xffff, 0xffff, 0xefbf, 0xe77f, 0xe79f, 
0x0253, 0xdf9f, 0xb71e, 0xdf7f, 0xe77f, 0xe77f, 0xd75f, 0x0293, 0x0000, 0x965d, 0xefdf, 0xc71e, 0xa6de, 0xf7df, 0xcf1e, 0x0000, 
0x0000, 0x0000, 0x0274, 0x12b4, 0x1b35, 0x0273, 0x0000, 0x0000
};

//the main function
void draw_water(){
    volatile int * pixel_ctrl_ptr = (int *) 0xFF203020;
	
	int N=6;
	int initial_y_position_left[N];
	int initial_x_position_left[N];
	int initial_y_position_right[N];
	int initial_x_position_right[N];
	
	
    // initialize location and direction of rectangles
    for(int i=0; i<N; i++){
        initial_y_position_left[i] = rand()%20*10;
		initial_x_position_left[i] = 2+i*8;
		initial_y_position_right[i] = rand()%20*10;
		initial_x_position_right[i] = 270+i*8;
    }
	
	while(1){
		for(int i=0; i<N; i++){
			draw_water_one(initial_x_position_left[i],initial_y_position_left[i]);
			draw_water_one(initial_x_position_right[i],initial_y_position_right[i]);
		}
		wait_screen(pixel_ctrl_ptr);
		wait_screen(pixel_ctrl_ptr);
		for(int i=0; i<N; i++){
			clear_water_one(initial_x_position_left[i],initial_y_position_left[i]);
			clear_water_one(initial_x_position_right[i],initial_y_position_right[i]);
		}
		
		for(int i=0; i<N; i++){
			initial_y_position_left[i] +=5;
			initial_y_position_right[i] +=5;
			
			if(initial_y_position_left[i]>=220){
				initial_y_position_left[i] =0;
			}
			if(initial_y_position_right[i]>=220){
				initial_y_position_right[i] =0;
			}
		}
		

	}
}

void draw_water_one(int x_position, int y_position){
	for(int x = 0; x<8; x++){
		for(int y=0; y<15; y++){
			int index = y*8+x;
			plot_pixel(x+x_position, y+y_position, water[index]);
		}
	}
}

void clear_water_one(int x_position, int y_position){
	for(int x = 0; x<8; x++){
		for(int y=0; y<15; y++){
			plot_pixel(x+x_position, y+y_position, 0x0000);
		}
	}
}

