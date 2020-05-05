#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "interrupt_ID.h"
#include "defines.h"
#include "address_map_arm.h"
volatile int * PS2_ptr = (int *) 0xFF200100;  // PS/2 port address
unsigned char byte1 = 0;
unsigned char byte2 = 0;
unsigned char byte3 = 0;
void plot_pixel(int x, int y, short int line_color); 
void VGA_text(int x, int y, char *text_ptr);
void clear_screen();
void wait_for_vsync();
void VGA_text_clean();

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


void wait_screen(volatile int* pixel_ctrl_ptr);

void draw_water_one(int x_position, int y_position);
void clear_water_one(int x_position, int y_position);


//waiting for the screen to finish drawing


volatile int pixel_buffer_start; // global variable

volatile char *character_buffer = (char *) 0xC9000000;
volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

//functions for PS2 keyboard

void config_interrupt (int int_ID, int CPU_target);

void hw_write_bits(volatile int *, volatile int, volatile int);

void set_A9_IRQ_stack(void);

void enable_A9_interrupts(void);

void disable_A9_interrupts(void);

void config_GIC(void);

void PS2_ISR(void);

void config_PS2() {
    volatile int * PS2_ptr = (int *)PS2_BASE; // PS/2 port address

    *(PS2_ptr) = 0xFF; /* reset */
    *(PS2_ptr + 1) =
            0x1; /* write to the PS/2 Control register to enable interrupts */
}

void plot_pixel(int x, int y, short int line_color){
	*(short int*)(pixel_buffer_start + (y<<10) + (x<<1)) = line_color;
}

// information for each node
// number_#_node[5]
// [0]=row, [1]=column, [2]=water, [3]=type, [4]=item[0]
// type: 1=start, 2=end, 3=straight, 4=curve, 5=tri
// item[]: 0=A, 1=B, 2=C, 3=D
// water: 0=no_water, 1=have_water

//function declaration
void print_set_game(int array[][5], int size);
void set_game_one(int array[][5]);
bool update_database_one(int array[][5], int input);
void set_game_two(int array[][5]);
bool update_database_two(int array[][5], int input);
void set_game_three(int array[][5]);
bool update_database_three(int array[][5], int input);
void set_game_four(int array[][5]);
bool update_database_four(int array[][5], int input);
void set_game_five(int array[][5]);
bool update_database_five(int array[][5], int input);
void set_game_six(int array[][5]);
bool update_database_six(int array[][5], int input);
void set_game_seven(int array[][5]);
bool update_database_seven(int array[][5], int input);
void set_game_eight(int array[][5]);
bool update_database_eight(int array[][5], int input);
void set_game_nine(int array[][5]);
bool update_database_nine(int array[][5], int input);
void set_game_ten(int array[][5]);
bool update_database_ten(int array[][5], int input);
void set_game_eleven(int array[][5]);
bool update_database_eleven(int array[][5], int input);
void set_game_twelve(int array[][5]);
bool update_database_twelve(int array[][5], int input);

char check_diffculty();
char check_enter();

//main function
int main() {
    bool run = true;
 
    set_A9_IRQ_stack();
    config_GIC();
    //config_PS2();
    enable_A9_interrupts();
   // *(pixel_ctrl_ptr + 1) = 0xC8000000;
   // wait_for_vsync();
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
   // *(pixel_ctrl_ptr + 1) = 0xC0000000;
   //pixel_buffer_start = *(pixel_ctrl_ptr + 1);
    draw_start();

    char choose_to_enter=check_enter();
    if(choose_to_enter=='q'){
       run=false;
        clear_screen();
        VGA_text(10,5,"You choose to not to play.Bye Bye!!!!");
    }
    else{
       run=true;
    }

    while(run){
         
        //game start page
        clear_screen();
        draw_home();
        
        char level_of_difficulty=check_enter();
     
        if(level_of_difficulty == 'q'){
            run = false;
            clear_screen();
            return 0;
        }
        else if(level_of_difficulty!='1' && level_of_difficulty!='2' && level_of_difficulty!='3' && level_of_difficulty!='4' && level_of_difficulty!='5'
                    && level_of_difficulty!='6' && level_of_difficulty!='7' && level_of_difficulty!='8' && level_of_difficulty!='9' && level_of_difficulty!='a'
                    && level_of_difficulty!='b' && level_of_difficulty!='c'){
        }
        else{
            bool return_home = false;
            bool reset = true;
            while(reset) {
                reset = false;
                volatile int * HEX3_HEX0_ptr = (int *) 0xFF200020;
                int seg7[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
                //level one game
                if (level_of_difficulty == '1') {
                    //initialize
                
                    int number_of_turn = 20;
                   
                  
                    *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                    int game_one_array[6][5];
                    set_game_one(game_one_array);
                    clear_screen();
                    draw_one_initial();
                    draw_level_one();
                    if(number_of_turn==20){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                    }
                    
                    //updating
                    while (number_of_turn != 0) {
                        
                        if(number_of_turn==19){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[1]<<8;
                        }
                        else if(number_of_turn==18){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[1]<<8;
                        }
                        else if(number_of_turn==17){
                            *HEX3_HEX0_ptr = seg7[7]|seg7[1]<<8;
                        }
                        else if(number_of_turn==16){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[1]<<8;
                        }
                        else if(number_of_turn==15){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[1]<<8;
                        }
                        else if(number_of_turn==14){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[1]<<8;
                        }
                        else if(number_of_turn==13){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[1]<<8;
                        }
                        else if(number_of_turn==12){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[1]<<8;
                        }
                        else if(number_of_turn==11){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[1]<<8;
                        }
                        else if(number_of_turn==10){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[1]<<8;
                        }
                        else if(number_of_turn==9){
                       *HEX3_HEX0_ptr = seg7[9]|seg7[0]<<8;
                        }
                        else if(number_of_turn==8){
                        *HEX3_HEX0_ptr = seg7[8]|seg7[0]<<8;
                        }
                        else if(number_of_turn==7){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[0]<<8;
                        }
                        else if(number_of_turn==6){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[0]<<8;
                        }
                        else if(number_of_turn==5){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[0]<<8;
                        }
                        else if(number_of_turn==4){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[0]<<8;
                        }
                        else if(number_of_turn==3){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                        }
                        else if(number_of_turn==2){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[0]<<8;
                        }
                        else if(number_of_turn==1){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[0]<<8;
                        }
                         else if(number_of_turn==0){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8;    
                        }
                         
                        //get the input
                        char row=check_diffculty();
                        
                        if (row == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (row == 'h') {
                            return_home = true;
                            break;
                        } else if (row == 'r') {
                            reset = true;
                            break;
                        }
                                             
                      
                        char column=check_diffculty();
                        
                         number_of_turn--;
                        if (column == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (column == 'h') {
                            return_home = true;
                            break;
                        } else if (column == 'r') {
                            reset = true;
                            break;
                        }
                        

                        //update the database
                        if ((row != '1' && row != '2' && row != '3') || (column != '1' && column != '2')) {
                            number_of_turn++;
                        } else {
                            //get input
                            int row_int = row - '0';
                            int column_int = column - '0';
                            int input = (row_int - 1) * 2 + column_int;
                     

                            //update the database
                            bool status;
                            status = update_database_one(game_one_array, input);

                            if (status == true) {
                                break;
                            }
                            else{
                                update_one(game_one_array[input-1][0], game_one_array[input-1][1], game_one_array[input-1][3], game_one_array[input-1][4]);
                            }
                        }
                    }

                    //win or lose or loop
                    if (return_home) {
                        break;
                    }
                    else if (!reset) {
                        //win or lose
                        if (number_of_turn != 0) {
                            
                       
    
                            draw_one_final();
                            volatile int * pixel_ctrl_ptr2 = (int *) 0xFF203020;
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
	                        wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            clear_screen();
                            draw_win();
                         
                             
                            volatile int *SW_ptr = (int *) 0xFF200040;
                            volatile int *LEDR_ptr = (int *) 0xFF200000;
                             int value=*SW_ptr;
                             *LEDR_ptr=value;
                       
                            bool valid_select = true;
                            while(valid_select) {
                                char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    *LEDR_ptr=0;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }

                        } else {
                            clear_screen();
                            draw_fail();
                            *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8; 
                            bool valid_select = true;
                            while(valid_select) {
                                
                         
                              char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                                
                            }
                        }
                    }
                }
                    //level two game
                else if (level_of_difficulty=='2') {
                    //initialize the for game one
                    int number_of_turn = 20;
                    int game_two_array[9][5];
                    set_game_two(game_two_array);
                    clear_screen();
                    draw_two_initial();
                    draw_level_two();
                    *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                    
                    
                    //updating
                    while (number_of_turn != 0) {
                        //printf("\nthe remaining chances is: %d/20\n", number_of_turn);
                         if(number_of_turn==20){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                         }
                        else if(number_of_turn==19){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[1]<<8;
                        }
                        else if(number_of_turn==18){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[1]<<8;
                        }
                        else if(number_of_turn==17){
                            *HEX3_HEX0_ptr = seg7[7]|seg7[1]<<8;
                        }
                        else if(number_of_turn==16){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[1]<<8;
                        }
                        else if(number_of_turn==15){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[1]<<8;
                        }
                        else if(number_of_turn==14){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[1]<<8;
                        }
                        else if(number_of_turn==13){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[1]<<8;
                        }
                        else if(number_of_turn==12){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[1]<<8;
                        }
                        else if(number_of_turn==11){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[1]<<8;
                        }
                        else if(number_of_turn==10){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[1]<<8;
                        }
                        else if(number_of_turn==9){
                       *HEX3_HEX0_ptr = seg7[9]|seg7[0]<<8;
                        }
                        else if(number_of_turn==8){
                        *HEX3_HEX0_ptr = seg7[8]|seg7[0]<<8;
                        }
                        else if(number_of_turn==7){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[0]<<8;
                        }
                        else if(number_of_turn==6){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[0]<<8;
                        }
                        else if(number_of_turn==5){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[0]<<8;
                        }
                        else if(number_of_turn==4){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[0]<<8;
                        }
                        else if(number_of_turn==3){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                        }
                        else if(number_of_turn==2){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[0]<<8;
                        }
                        else if(number_of_turn==1){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[0]<<8;
                        }
                         else if(number_of_turn==0){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8;    
                        }
                        
                        
                        
                        char row=check_diffculty();
                        if (row == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (row == 'h') {
                            return_home = true;
                            break;
                        } else if (row == 'r') {
                            reset = true;
                            break;
                        }

                       
                       char column=check_diffculty();
                        if (column == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (column == 'h') {
                            return_home = true;
                            break;
                        } else if (column == 'r') {
                            reset = true;
                            break;
                        }
                        number_of_turn--;
                        //update the database
                        if ((row != '1' && row != '2' && row != '3') || (column != '1' && column != '2' && column != '3')) {
                           // printf("you should enter a number within the range 1 to 9. Please try again.\n");
                            number_of_turn++;
                        } else {
                            //get input
                            int row_int = row - '0';
                            int column_int = column - '0';
                            int input = (row_int - 1) * 3 + column_int;
                           
                            bool status;
                            status = update_database_two(game_two_array, input);
                            //print_set_game(game_two_array, 9);
                            if (status == true) {
                                break;
                            }
                            else{
                                update_two(game_two_array[input-1][0],game_two_array[input-1][1],
                                game_two_array[input-1][3],game_two_array[input-1][4]);
                            }
                        }
                    }

                    //win or lose or loop
                    if (return_home) {
                        break;
                    }
                    else if (!reset) {
                        //win or lose
                        if (number_of_turn != 0) {
                        
              
                             draw_two_final();
                            volatile int * pixel_ctrl_ptr2 = (int *) 0xFF203020;
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
	                        wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            clear_screen();
                            draw_win();
                         
                            volatile int *SW_ptr = (int *) 0xFF200040;
                            volatile int *LEDR_ptr = (int *) 0xFF200000;
                            int value=*SW_ptr;
                            *LEDR_ptr=value;
                            
                     
                
                           bool valid_select=true;
                           while(valid_select) {
 
                                 char choice=check_enter();
                                //draw_water();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    *LEDR_ptr=0;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }

                        } else {
                            clear_screen();
                            draw_fail();
                            *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8; 
                            bool valid_select = true;
                            while(valid_select) {
                                
                              char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }
                        }
                    }
                }
                //level three game
               else if (level_of_difficulty=='3') {
                    //initialize the for game one
                    int number_of_turn = 20;
                    int game_array[15][5];
                    set_game_three(game_array);
                    clear_screen();
                    draw_three_initial();
                    draw_level_three();
                     *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                    //updating
                    while (number_of_turn != 0) {
                        //printf("\nthe remaining chances is: %d/20\n", number_of_turn);
                  
                         if(number_of_turn==19){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[1]<<8;
                        }
                        else if(number_of_turn==18){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[1]<<8;
                        }
                        else if(number_of_turn==17){
                            *HEX3_HEX0_ptr = seg7[7]|seg7[1]<<8;
                        }
                        else if(number_of_turn==16){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[1]<<8;
                        }
                        else if(number_of_turn==15){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[1]<<8;
                        }
                        else if(number_of_turn==14){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[1]<<8;
                        }
                        else if(number_of_turn==13){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[1]<<8;
                        }
                        else if(number_of_turn==12){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[1]<<8;
                        }
                        else if(number_of_turn==11){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[1]<<8;
                        }
                        else if(number_of_turn==10){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[1]<<8;
                        }
                        else if(number_of_turn==9){
                       *HEX3_HEX0_ptr = seg7[9]|seg7[0]<<8;
                        }
                        else if(number_of_turn==8){
                        *HEX3_HEX0_ptr = seg7[8]|seg7[0]<<8;
                        }
                        else if(number_of_turn==7){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[0]<<8;
                        }
                        else if(number_of_turn==6){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[0]<<8;
                        }
                        else if(number_of_turn==5){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[0]<<8;
                        }
                        else if(number_of_turn==4){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[0]<<8;
                        }
                        else if(number_of_turn==3){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                        }
                        else if(number_of_turn==2){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[0]<<8;
                        }
                        else if(number_of_turn==1){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[0]<<8;
                        }
                         else if(number_of_turn==0){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8;    
                        }


                        char row=check_diffculty();
                        if (row == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (row == 'h') {
                            return_home = true;
                            break;
                        } else if (row == 'r') {
                            reset = true;
                            break;
                        }

                       
                       char column=check_diffculty();
                       number_of_turn--;
                        if (column == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (column == 'h') {
                            return_home = true;
                            break;
                        } else if (column == 'r') {
                            reset = true;
                            break;
                        }

                        //update the database
                        if ((row != '1' && row != '2' && row != '3') || (column != '1' && column != '2' && column != '3'&&column!='4'
                        &&column!='5')) {
                           // printf("you should enter a number within the range 1 to 9. Please try again.\n");
                            number_of_turn++;
                        } else {
                            //get input
                            int row_int = row - '0';
                            int column_int = column - '0';
                            int input = (row_int - 1) *5 + column_int;
                           
                            bool status;
                            status = update_database_three(game_array, input);
                            //print_set_game(game_two_array, 9);
                            if (status == true) {
                                break;
                            }
                            else{
                                update_three(game_array[input-1][0],game_array[input-1][1],
                                game_array[input-1][3],game_array[input-1][4]);
                            }
                        }
                    }

                    //win or lose or loop
                    if (return_home) {
                        break;
                    }
                    else if (!reset) {
                        //win or lose
                        if (number_of_turn != 0) {
                          draw_three_final();
                            volatile int * pixel_ctrl_ptr2 = (int *) 0xFF203020;
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
	                        wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            clear_screen();
                            draw_win();
                         
                           volatile int *SW_ptr = (int *) 0xFF200040;
                           volatile int *LEDR_ptr = (int *) 0xFF200000;
                        int value=*SW_ptr;
                        *LEDR_ptr=value;
                        
   
                           bool valid_select=true;
                           while(valid_select) {
                            
                                char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    *LEDR_ptr=0;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }

                        } else {
                            clear_screen();
                            draw_fail();
                            *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8; 
                            bool valid_select = true;
                            while(valid_select) {
                                
                               char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }
                        }
                    }
                }

                //level four game
               else if (level_of_difficulty=='4') {
                    //initialize the for game one
                    int number_of_turn = 25;
                    int game_array[13][5];
                    set_game_four(game_array);
                    clear_screen();
                    draw_four_initial();
                    draw_level_four();
                     *HEX3_HEX0_ptr = seg7[5]|seg7[2]<<8;
                    //updating
                    while (number_of_turn != 0) {
                        //printf("\nthe remaining chances is: %d/20\n", number_of_turn);
                      
                        if(number_of_turn==25){
                            *HEX3_HEX0_ptr = seg7[5]|seg7[2]<<8;
                        }
                        else if(number_of_turn==24){
                            *HEX3_HEX0_ptr = seg7[4]|seg7[2]<<8;
                        }
                        else if(number_of_turn==23){
                            *HEX3_HEX0_ptr = seg7[3]|seg7[2]<<8;
                        }
                        else if(number_of_turn==22){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[2]<<8;
                        }
                        else if(number_of_turn==21){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[2]<<8;
                        }
                        else if(number_of_turn==20){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                        }
                        else if(number_of_turn==19){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[1]<<8;
                        }
                        else if(number_of_turn==18){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[1]<<8;
                        }
                        else if(number_of_turn==17){
                            *HEX3_HEX0_ptr = seg7[7]|seg7[1]<<8;
                        }
                        else if(number_of_turn==16){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[1]<<8;
                        }
                        else if(number_of_turn==15){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[1]<<8;
                        }
                        else if(number_of_turn==14){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[1]<<8;
                        }
                        else if(number_of_turn==13){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[1]<<8;
                        }
                        else if(number_of_turn==12){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[1]<<8;
                        }
                        else if(number_of_turn==11){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[1]<<8;
                        }
                        else if(number_of_turn==10){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[1]<<8;
                        }
                        else if(number_of_turn==9){
                       *HEX3_HEX0_ptr = seg7[9]|seg7[0]<<8;
                        }
                        else if(number_of_turn==8){
                        *HEX3_HEX0_ptr = seg7[8]|seg7[0]<<8;
                        }
                        else if(number_of_turn==7){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[0]<<8;
                        }
                        else if(number_of_turn==6){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[0]<<8;
                        }
                        else if(number_of_turn==5){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[0]<<8;
                        }
                        else if(number_of_turn==4){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[0]<<8;
                        }
                        else if(number_of_turn==3){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                        }
                        else if(number_of_turn==2){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[0]<<8;
                        }
                        else if(number_of_turn==1){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[0]<<8;
                        }
                         else if(number_of_turn==0){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8;    
                        }
                        char row=check_diffculty();
                        if (row == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (row == 'h') {
                            return_home = true;
                            break;
                        } else if (row == 'r') {
                            reset = true;
                            break;
                        }

                       
                       char column=check_diffculty();
                       number_of_turn--;
                        if (column == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (column == 'h') {
                            return_home = true;
                            break;
                        } else if (column == 'r') {
                            reset = true;
                            break;
                        }

                        //update the database
                        if ((row != '1' && row != '2' && row != '3') || (column != '1' && column != '2' && column != '3'&&column!='4'
                        &&column!='5'||(row=='1'&&column=='5')||(row=='3'&&column=='5'))) {
                           // printf("you should enter a number within the range 1 to 9. Please try again.\n");
                            number_of_turn++;
                        } else {
                            //get input
                            int row_int = row - '0';
                            int column_int = column - '0';
                            int input = (row_int - 1) *5 + column_int;
                            if(row_int>1){
                                input--;
                            }
                           
                            bool status;
                            status = update_database_four(game_array, input);
                            //print_set_game(game_two_array, 9);
                            if (status == true) {
                                break;
                            }
                            else{
                                update_four(game_array[input-1][0],game_array[input-1][1],
                                game_array[input-1][3],game_array[input-1][4]);
                            }
                        }
                    }

                    //win or lose or loop
                    if (return_home) {
                        break;
                    }
                    else if (!reset) {
                        //win or lose
                        if (number_of_turn != 0) {
                           clear_screen();
                           
                              draw_four_final();
                             wait_for_vsync();
                             
                           draw_win();
                            volatile int *SW_ptr = (int *) 0xFF200040;
                           volatile int *LEDR_ptr = (int *) 0xFF200000;
                        int value=*SW_ptr;
                        *LEDR_ptr=value;
                        
                  
                
                           bool valid_select=true;
                           
                           while(valid_select) {
                             
                                char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    *LEDR_ptr=0;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }

                        } else {
                            clear_screen();
                            draw_fail();
                            
                            *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8; 
                            bool valid_select = true;
                            while(valid_select) {
                                
                               char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }
                        }
                    }
                }


                //level five game
                else if (level_of_difficulty=='5') {
                    //initialize the for game one
                    int number_of_turn = 25;
                    int game_array[12][5];
                    set_game_five(game_array);
                    clear_screen();
                    draw_five_initial();
                    draw_level_five();
                    *HEX3_HEX0_ptr = seg7[5]|seg7[2]<<8;
                    //updating
                    while (number_of_turn != 0) {
                        //printf("\nthe remaining chances is: %d/20\n", number_of_turn);
                     
                         if(number_of_turn==25){
                            *HEX3_HEX0_ptr = seg7[5]|seg7[2]<<8;
                        }
                        else if(number_of_turn==24){
                            *HEX3_HEX0_ptr = seg7[4]|seg7[2]<<8;
                        }
                        else if(number_of_turn==23){
                            *HEX3_HEX0_ptr = seg7[3]|seg7[2]<<8;
                        }
                        else if(number_of_turn==22){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[2]<<8;
                        }
                        else if(number_of_turn==21){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[2]<<8;
                        }
                        else if(number_of_turn==20){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                        }
                        else if(number_of_turn==19){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[1]<<8;
                        }
                        else if(number_of_turn==18){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[1]<<8;
                        }
                        else if(number_of_turn==17){
                            *HEX3_HEX0_ptr = seg7[7]|seg7[1]<<8;
                        }
                        else if(number_of_turn==16){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[1]<<8;
                        }
                        else if(number_of_turn==15){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[1]<<8;
                        }
                        else if(number_of_turn==14){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[1]<<8;
                        }
                        else if(number_of_turn==13){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[1]<<8;
                        }
                        else if(number_of_turn==12){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[1]<<8;
                        }
                        else if(number_of_turn==11){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[1]<<8;
                        }
                        else if(number_of_turn==10){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[1]<<8;
                        }
                        else if(number_of_turn==9){
                       *HEX3_HEX0_ptr = seg7[9]|seg7[0]<<8;
                        }
                        else if(number_of_turn==8){
                        *HEX3_HEX0_ptr = seg7[8]|seg7[0]<<8;
                        }
                        else if(number_of_turn==7){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[0]<<8;
                        }
                        else if(number_of_turn==6){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[0]<<8;
                        }
                        else if(number_of_turn==5){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[0]<<8;
                        }
                        else if(number_of_turn==4){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[0]<<8;
                        }
                        else if(number_of_turn==3){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                        }
                        else if(number_of_turn==2){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[0]<<8;
                        }
                        else if(number_of_turn==1){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[0]<<8;
                        }
                         else if(number_of_turn==0){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8;    
                        }

                        char row=check_diffculty();
                        if (row == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (row == 'h') {
                            return_home = true;
                            break;
                        } else if (row == 'r') {
                            reset = true;
                            break;
                        }

                       
                       char column=check_diffculty();
                        number_of_turn--;
                        if (column == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (column == 'h') {
                            return_home = true;
                            break;
                        } else if (column == 'r') {
                            reset = true;
                            break;
                        }

                        //update the database
                        if ((row != '1' && row != '2' && row != '3') || (column != '1' && column != '2' && column != '3'&&column!='4'
                        )) {
                           // printf("you should enter a number within the range 1 to 9. Please try again.\n");
                            number_of_turn++;
                        } else {
                            //get input
                            int row_int = row - '0';
                            int column_int = column - '0';
                            int input = (row_int - 1) *4 + column_int;
                           
                            bool status;
                            status = update_database_five(game_array, input);
                            //print_set_game(game_two_array, 9);
                            if (status == true) {
                                break;
                            }
                            else{
                                update_five(game_array[input-1][0],game_array[input-1][1],
                                game_array[input-1][3],game_array[input-1][4]);
                            }
                        }
                    }

                    //win or lose or loop
                    if (return_home) {
                        break;
                    }
                    else if (!reset) {
                        //win or lose
                        if (number_of_turn != 0) {
                           clear_screen();
                           draw_five_final();
                            volatile int * pixel_ctrl_ptr2 = (int *) 0xFF203020;
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
	                        wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            clear_screen();
                            draw_win();
                         
                           
                           
                            volatile int *SW_ptr = (int *) 0xFF200040;
                           volatile int *LEDR_ptr = (int *) 0xFF200000;
                        int value=*SW_ptr;
                        *LEDR_ptr=value;
                       
                           bool valid_select=true;
                     
                           while(valid_select) {
                    
                                 char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    *LEDR_ptr=0;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }

                        } else {
                            clear_screen();
                            draw_fail();
                            *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8; 
                            bool valid_select = true;
                            while(valid_select) {
                                
                               char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }
                        }
                    }
                }

                //level six game
                else if (level_of_difficulty=='6') {
                    //initialize the for game one
                    int number_of_turn = 20;
                    int game_array[13][5];
                    set_game_six(game_array);
                    clear_screen();
                    draw_six_initial();
                    draw_level_six();
                    *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                    //updating
                    while (number_of_turn != 0) {
                        //printf("\nthe remaining chances is: %d/20\n", number_of_turn);
                        
                     
                        if(number_of_turn==20){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                        }
                        else if(number_of_turn==19){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[1]<<8;
                        }
                        else if(number_of_turn==18){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[1]<<8;
                        }
                        else if(number_of_turn==17){
                            *HEX3_HEX0_ptr = seg7[7]|seg7[1]<<8;
                        }
                        else if(number_of_turn==16){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[1]<<8;
                        }
                        else if(number_of_turn==15){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[1]<<8;
                        }
                        else if(number_of_turn==14){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[1]<<8;
                        }
                        else if(number_of_turn==13){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[1]<<8;
                        }
                        else if(number_of_turn==12){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[1]<<8;
                        }
                        else if(number_of_turn==11){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[1]<<8;
                        }
                        else if(number_of_turn==10){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[1]<<8;
                        }
                        else if(number_of_turn==9){
                       *HEX3_HEX0_ptr = seg7[9]|seg7[0]<<8;
                        }
                        else if(number_of_turn==8){
                        *HEX3_HEX0_ptr = seg7[8]|seg7[0]<<8;
                        }
                        else if(number_of_turn==7){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[0]<<8;
                        }
                        else if(number_of_turn==6){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[0]<<8;
                        }
                        else if(number_of_turn==5){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[0]<<8;
                        }
                        else if(number_of_turn==4){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[0]<<8;
                        }
                        else if(number_of_turn==3){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                        }
                        else if(number_of_turn==2){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[0]<<8;
                        }
                        else if(number_of_turn==1){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[0]<<8;
                        }
                         else if(number_of_turn==0){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8;    
                        }
                        char row=check_diffculty();
                        if (row == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (row == 'h') {
                            return_home = true;
                            break;
                        } else if (row == 'r') {
                            reset = true;
                            break;
                        }

                       
                       char column=check_diffculty();
                       number_of_turn--;
                        if (column == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (column == 'h') {
                            return_home = true;
                            break;
                        } else if (column == 'r') {
                            reset = true;
                            break;
                        }
                    
                        //update the database
                        if ((row != '1' && row != '2' && row != '3') || (column != '1' && column != '2' && column != '3'&&column!='4'
                        &&column!='5')||(row=='2'&&column=='4')||(row=='2'&&column=='5')) {
                           // printf("you should enter a number within the range 1 to 9. Please try again.\n");
                            number_of_turn++;
                        } else {
                            //get input
                            int row_int = row - '0';
                            int column_int = column - '0';
                            int input = (row_int - 1) *5 + column_int;
                            if(row=='3'){
                                input-=2;
                            }
                            bool status;
                            status = update_database_six(game_array, input);
                            //print_set_game(game_two_array, 9);
                            if (status == true) {
                                break;
                            }
                            else{
                                update_six(game_array[input-1][0],game_array[input-1][1],
                                game_array[input-1][3],game_array[input-1][4]);
                            }
                        }
                    }

                    //win or lose or loop
                    if (return_home) {
                        break;
                    }
                    else if (!reset) {
                        //win or lose
                        if (number_of_turn != 0) {
                           
                            draw_six_final();
                            volatile int * pixel_ctrl_ptr2 = (int *) 0xFF203020;
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
	                        wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            clear_screen();
                            draw_win();
                            
                            volatile int *SW_ptr = (int *) 0xFF200040;
                           volatile int *LEDR_ptr = (int *) 0xFF200000;
                         
                            int value=*SW_ptr;
                           *LEDR_ptr=value;
                           bool valid_select=true;
                           while(valid_select) {
                    
                                 char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    *LEDR_ptr=0;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }

                        } else {
                            clear_screen();
                            draw_fail();
                            *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8; 
                            bool valid_select = true;
                            while(valid_select) {
                                
                               char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }
                        }
                    }
                }


                //level seven game
               else if (level_of_difficulty=='7') {
                    //initialize the for game one
                    int number_of_turn = 20;
                    int game_array[12][5];
                    set_game_seven(game_array);
                    clear_screen();
                    draw_seven_initial();
                    draw_level_seven();
                    *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                    //updating
                    while (number_of_turn != 0) {
                        //printf("\nthe remaining chances is: %d/20\n", number_of_turn);
                        
                      
                        if(number_of_turn==20){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                        }
                        else if(number_of_turn==19){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[1]<<8;
                        }
                        else if(number_of_turn==18){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[1]<<8;
                        }
                        else if(number_of_turn==17){
                            *HEX3_HEX0_ptr = seg7[7]|seg7[1]<<8;
                        }
                        else if(number_of_turn==16){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[1]<<8;
                        }
                        else if(number_of_turn==15){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[1]<<8;
                        }
                        else if(number_of_turn==14){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[1]<<8;
                        }
                        else if(number_of_turn==13){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[1]<<8;
                        }
                        else if(number_of_turn==12){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[1]<<8;
                        }
                        else if(number_of_turn==11){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[1]<<8;
                        }
                        else if(number_of_turn==10){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[1]<<8;
                        }
                        else if(number_of_turn==9){
                       *HEX3_HEX0_ptr = seg7[9]|seg7[0]<<8;
                        }
                        else if(number_of_turn==8){
                        *HEX3_HEX0_ptr = seg7[8]|seg7[0]<<8;
                        }
                        else if(number_of_turn==7){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[0]<<8;
                        }
                        else if(number_of_turn==6){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[0]<<8;
                        }
                        else if(number_of_turn==5){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[0]<<8;
                        }
                        else if(number_of_turn==4){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[0]<<8;
                        }
                        else if(number_of_turn==3){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                        }
                        else if(number_of_turn==2){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[0]<<8;
                        }
                        else if(number_of_turn==1){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[0]<<8;
                        }
                         else if(number_of_turn==0){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8;    
                        }

                        char row=check_diffculty();
                        if (row == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (row == 'h') {
                            return_home = true;
                            break;
                        } else if (row == 'r') {
                            reset = true;
                            break;
                        }

                       
                       char column=check_diffculty();
                      number_of_turn--;
                        if (column == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (column == 'h') {
                            return_home = true;
                            break;
                        } else if (column == 'r') {
                            reset = true;
                            break;
                        }

                        //update the database
                        if ((row != '1' && row != '2' && row != '3') || (column != '1' && column != '2' && column != '3'&&column!='4'
                        )) {
                           // printf("you should enter a number within the range 1 to 9. Please try again.\n");
                            number_of_turn++;
                        } else {
                            //get input
                            int row_int = row - '0';
                            int column_int = column - '0';
                            int input = (row_int - 1) *4 + column_int;
                           
                            bool status;
                            status = update_database_seven(game_array, input);
                            //print_set_game(game_two_array, 9);
                            if (status == true) {
                                break;
                            }
                            else{
                                update_seven(game_array[input-1][0],game_array[input-1][1],
                                game_array[input-1][3],game_array[input-1][4]);
                            }
                        }
                    }

                    //win or lose or loop
                    if (return_home) {
                        break;
                    }
                    else if (!reset) {
                        //win or lose
                        if (number_of_turn != 0) {
                           clear_screen();
                             draw_seven_final();
                            volatile int * pixel_ctrl_ptr2 = (int *) 0xFF203020;
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
	                        wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            clear_screen();
                            draw_win();
                         
                     
                            volatile int *SW_ptr = (int *) 0xFF200040;
                            volatile int *LEDR_ptr = (int *) 0xFF200000;
                           int value=*SW_ptr;
                           *LEDR_ptr=value;
                           bool valid_select=true;
                           while(valid_select) {
                    
                                 char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                     *LEDR_ptr=0;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }

                        } else {
                            clear_screen();
                            draw_fail();
                        
                            *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8; 
                            bool valid_select = true;
                            while(valid_select) {
                                
                              char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }
                        }
                    }
                }

                //level eight game
               else if (level_of_difficulty=='8') {
                    //initialize the for game one
                    int number_of_turn = 30;
                    int game_array[18][5];
                    set_game_eight(game_array);
                    clear_screen();
                    draw_eight_initial();
                    draw_level_eight();
                    *HEX3_HEX0_ptr = seg7[0]|seg7[3]<<8;
                    //updating
                    while (number_of_turn != 0) {
                        //printf("\nthe remaining chances is: %d/20\n", number_of_turn);
                      
                        
                        if(number_of_turn==30){
                            *HEX3_HEX0_ptr = seg7[0]|seg7[3]<<8;
                        }
                
                        else if(number_of_turn==29){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[2]<<8;
                        }
                        else if(number_of_turn==28){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[2]<<8;
                        }
                        else if(number_of_turn==27){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[2]<<8;
                        }
                        else if(number_of_turn==26){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[2]<<8;
                        }
                        if(number_of_turn==25){
                            *HEX3_HEX0_ptr = seg7[5]|seg7[2]<<8;
                        }
                        else if(number_of_turn==24){
                            *HEX3_HEX0_ptr = seg7[4]|seg7[2]<<8;
                        }
                        else if(number_of_turn==23){
                            *HEX3_HEX0_ptr = seg7[3]|seg7[2]<<8;
                        }
                        else if(number_of_turn==22){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[2]<<8;
                        }
                        else if(number_of_turn==21){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[2]<<8;
                        }
                        else if(number_of_turn==20){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                        }
                        else if(number_of_turn==19){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[1]<<8;
                        }
                        else if(number_of_turn==18){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[1]<<8;
                        }
                        else if(number_of_turn==17){
                            *HEX3_HEX0_ptr = seg7[7]|seg7[1]<<8;
                        }
                        else if(number_of_turn==16){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[1]<<8;
                        }
                        else if(number_of_turn==15){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[1]<<8;
                        }
                        else if(number_of_turn==14){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[1]<<8;
                        }
                        else if(number_of_turn==13){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[1]<<8;
                        }
                        else if(number_of_turn==12){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[1]<<8;
                        }
                        else if(number_of_turn==11){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[1]<<8;
                        }
                        else if(number_of_turn==10){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[1]<<8;
                        }
                        else if(number_of_turn==9){
                       *HEX3_HEX0_ptr = seg7[9]|seg7[0]<<8;
                        }
                        else if(number_of_turn==8){
                        *HEX3_HEX0_ptr = seg7[8]|seg7[0]<<8;
                        }
                        else if(number_of_turn==7){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[0]<<8;
                        }
                        else if(number_of_turn==6){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[0]<<8;
                        }
                        else if(number_of_turn==5){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[0]<<8;
                        }
                        else if(number_of_turn==4){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[0]<<8;
                        }
                        else if(number_of_turn==3){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                        }
                        else if(number_of_turn==2){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[0]<<8;
                        }
                        else if(number_of_turn==1){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[0]<<8;
                        }
                         else if(number_of_turn==0){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8;    
                        }

                        char row=check_diffculty();
                        if (row == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (row == 'h') {
                            return_home = true;
                            break;
                        } else if (row == 'r') {
                            reset = true;
                            break;
                        }

                       
                       char column=check_diffculty();
                        number_of_turn--;
                        if (column == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (column == 'h') {
                            return_home = true;
                            break;
                        } else if (column == 'r') {
                            reset = true;
                            break;
                        }

                        //update the database
                        if ((row != '1' && row != '2' && row != '3') || (column != '1' && column != '2' && column != '3'&&column!='4'
                        &&column!='5'&&column!='6')) {
                           // printf("you should enter a number within the range 1 to 9. Please try again.\n");
                            number_of_turn++;
                        } else {
                            //get input
                            int row_int = row - '0';
                            int column_int = column - '0';
                            int input = (row_int - 1) *6 + column_int;
                           
                            bool status;
                            status = update_database_eight(game_array, input);
                            //print_set_game(game_two_array, 9);
                            if (status == true) {
                                break;
                            }
                            else{
                                update_eight(game_array[input-1][0],game_array[input-1][1],
                                game_array[input-1][3],game_array[input-1][4]);
                            }
                        }
                    }

                    //win or lose or loop
                    if (return_home) {
                        break;
                    }
                    else if (!reset) {
                        //win or lose
                        if (number_of_turn != 0) {
                        
                            draw_eight_final();
                            volatile int * pixel_ctrl_ptr2 = (int *) 0xFF203020;
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
	                        wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            clear_screen();
                            draw_win();
                         
                     
                            volatile int *SW_ptr = (int *) 0xFF200040;
                           volatile int *LEDR_ptr = (int *) 0xFF200000;
                        int value=*SW_ptr;
                        *LEDR_ptr=value;
                           bool valid_select=true;
                           while(valid_select) {
                    
                                 char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    LEDR_ptr=0;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }

                        } else {
                            clear_screen();
                            draw_fail();
                            *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8; 
                            bool valid_select = true;
                            while(valid_select) {
                                
                               char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }
                        }
                    }
                }


                //level nine game
                else if (level_of_difficulty=='9') {
                    //initialize the for game one
                    int number_of_turn = 30;
                    int game_array[18][5];
                    set_game_nine(game_array);
                    clear_screen();
                    draw_nine_initial();
                    draw_level_nine();
                    *HEX3_HEX0_ptr = seg7[0]|seg7[3]<<8;
                    //updating 
                    while (number_of_turn != 0) {
                        //printf("\nthe remaining chances is: %d/20\n", number_of_turn);
                     
                        if(number_of_turn==30){
                            *HEX3_HEX0_ptr = seg7[0]|seg7[3]<<8;
                        }
                
                        else if(number_of_turn==29){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[2]<<8;
                        }
                        else if(number_of_turn==28){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[2]<<8;
                        }
                        else if(number_of_turn==27){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[2]<<8;
                        }
                        else if(number_of_turn==26){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[2]<<8;
                        }
                        else if(number_of_turn==25){
                            *HEX3_HEX0_ptr = seg7[5]|seg7[2]<<8;
                        }
                        else if(number_of_turn==24){
                            *HEX3_HEX0_ptr = seg7[4]|seg7[2]<<8;
                        }
                        else if(number_of_turn==23){
                            *HEX3_HEX0_ptr = seg7[3]|seg7[2]<<8;
                        }
                        else if(number_of_turn==22){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[2]<<8;
                        }
                        else if(number_of_turn==21){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[2]<<8;
                        }
                        else if(number_of_turn==20){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                        }
                        else  if(number_of_turn==19){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[1]<<8;
                        }
                        else if(number_of_turn==18){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[1]<<8;
                        }
                        else if(number_of_turn==17){
                            *HEX3_HEX0_ptr = seg7[7]|seg7[1]<<8;
                        }
                        else if(number_of_turn==16){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[1]<<8;
                        }
                        else if(number_of_turn==15){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[1]<<8;
                        }
                        else if(number_of_turn==14){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[1]<<8;
                        }
                        else if(number_of_turn==13){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[1]<<8;
                        }
                        else if(number_of_turn==12){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[1]<<8;
                        }
                        else if(number_of_turn==11){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[1]<<8;
                        }
                        else if(number_of_turn==10){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[1]<<8;
                        }
                        else if(number_of_turn==9){
                       *HEX3_HEX0_ptr = seg7[9]|seg7[0]<<8;
                        }
                        else if(number_of_turn==8){
                        *HEX3_HEX0_ptr = seg7[8]|seg7[0]<<8;
                        }
                        else if(number_of_turn==7){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[0]<<8;
                        }
                        else if(number_of_turn==6){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[0]<<8;
                        }
                        else if(number_of_turn==5){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[0]<<8;
                        }
                        else if(number_of_turn==4){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[0]<<8;
                        }
                        else if(number_of_turn==3){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                        }
                        else if(number_of_turn==2){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[0]<<8;
                        }
                        else if(number_of_turn==1){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[0]<<8;
                        }
                         else if(number_of_turn==0){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8;    
                        }

                        char row=check_diffculty();
                        if (row == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (row == 'h') {
                            return_home = true;
                            break;
                        } else if (row == 'r') {
                            reset = true;
                            break;
                        }

                       
                       char column=check_diffculty();
                       number_of_turn--;
                        if (column == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (column == 'h') {
                            return_home = true;
                            break;
                        } else if (column == 'r') {
                            reset = true;
                            break;
                        }

                        //update the database
                        if ((row != '1' && row != '2' && row != '3') || (column != '1' && column != '2' && column != '3'&&column!='4'
                        &&column!='5'&&column!='6')) {
                           // printf("you should enter a number within the range 1 to 9. Please try again.\n");
                            number_of_turn++;
                        } else {
                            //get input
                            int row_int = row - '0';
                            int column_int = column - '0';
                            int input = (row_int - 1) *6 + column_int;
                           
                            bool status;
                            status = update_database_nine(game_array, input);
                            //print_set_game(game_two_array, 9)
                            if (status == true) {
                                break;
                            }
                            else{
                                update_nine(game_array[input-1][0],game_array[input-1][1],
                                game_array[input-1][3],game_array[input-1][4]);
                            }
                        }
                    }

                    //win or lose or loop
                    if (return_home) {
                        break;
                    }
                    else if (!reset) {
                        //win or lose
                        if (number_of_turn != 0) {
                          draw_nine_final();
                            volatile int * pixel_ctrl_ptr2 = (int *) 0xFF203020;
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
	                        wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            clear_screen();
                            draw_win();
                         
                    
                            volatile int *SW_ptr = (int *) 0xFF200040;
                           volatile int *LEDR_ptr = (int *) 0xFF200000;
                        int value=*SW_ptr;
                        *LEDR_ptr=value;
                           bool valid_select=true;
                           while(valid_select) {
                    
                                 char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    *LEDR_ptr=0;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }

                        } else {
                            clear_screen();
                            draw_fail();
                            *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8; 
                            bool valid_select = true;
                            while(valid_select) {
                                
                               char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }
                        }
                    }
                }


                //level ten game
               else if (level_of_difficulty=='a') {
                    //initialize the for game one
                    int number_of_turn = 30;
                    int game_array[21][5];
                    set_game_ten(game_array);
                    clear_screen();
                    draw_ten_initial();
                    draw_level_ten();
                    *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                    //updating
                    while (number_of_turn != 0) {
                        //printf("\nthe remaining chances is: %d/20\n", number_of_turn);
                        
                         if(number_of_turn==30){
                            *HEX3_HEX0_ptr = seg7[0]|seg7[3]<<8;
                        }
                
                        else if(number_of_turn==29){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[2]<<8;
                        }
                        else if(number_of_turn==28){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[2]<<8;
                        }
                        else if(number_of_turn==27){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[2]<<8;
                        }
                        else if(number_of_turn==26){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[2]<<8;
                        }
                        else if(number_of_turn==25){
                            *HEX3_HEX0_ptr = seg7[5]|seg7[2]<<8;
                        }
                        else if(number_of_turn==24){
                            *HEX3_HEX0_ptr = seg7[4]|seg7[2]<<8;
                        }
                        else if(number_of_turn==23){
                            *HEX3_HEX0_ptr = seg7[3]|seg7[2]<<8;
                        }
                        else if(number_of_turn==22){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[2]<<8;
                        }
                        else if(number_of_turn==21){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[2]<<8;
                        }
                        else if(number_of_turn==20){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                        }
                        else  if(number_of_turn==19){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[1]<<8;
                        }
                        else if(number_of_turn==18){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[1]<<8;
                        }
                        else if(number_of_turn==17){
                            *HEX3_HEX0_ptr = seg7[7]|seg7[1]<<8;
                        }
                        else if(number_of_turn==16){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[1]<<8;
                        }
                        else if(number_of_turn==15){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[1]<<8;
                        }
                        else if(number_of_turn==14){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[1]<<8;
                        }
                        else if(number_of_turn==13){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[1]<<8;
                        }
                        else if(number_of_turn==12){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[1]<<8;
                        }
                        else if(number_of_turn==11){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[1]<<8;
                        }
                        else if(number_of_turn==10){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[1]<<8;
                        }
                        else if(number_of_turn==9){
                       *HEX3_HEX0_ptr = seg7[9]|seg7[0]<<8;
                        }
                        else if(number_of_turn==8){
                        *HEX3_HEX0_ptr = seg7[8]|seg7[0]<<8;
                        }
                        else if(number_of_turn==7){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[0]<<8;
                        }
                        else if(number_of_turn==6){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[0]<<8;
                        }
                        else if(number_of_turn==5){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[0]<<8;
                        }
                        else if(number_of_turn==4){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[0]<<8;
                        }
                        else if(number_of_turn==3){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                        }
                        else if(number_of_turn==2){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[0]<<8;
                        }
                        else if(number_of_turn==1){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[0]<<8;
                        }
                         else if(number_of_turn==0){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8;    
                        }
                       

                        char row=check_enter();
                        if (row == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (row == 'h') {
                            return_home = true;
                            break;
                        } else if (row == 'r') {
                            reset = true;
                            break;
                        }

                       
                       char column=check_enter();
                        number_of_turn--;
                        if (column == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (column == 'h') {
                            return_home = true;
                            break;
                        } else if (column == 'r') {
                            reset = true;
                            break;
                        }

                        //update the database
                        if ((row != '1' && row != '2' && row != '3') || (column != '1' && column != '2' && column != '3'&&column!='4'
                        &&column!='5'&&column!='6'&&column!='7')) {
                           // printf("you should enter a number within the range 1 to 9. Please try again.\n");
                            number_of_turn++;
                        } else {
                            //get input
                            int row_int = row - '0';
                            int column_int = column - '0';
                            int input = (row_int - 1) *7 + column_int;
                           
                            bool status;
                            status = update_database_ten(game_array, input);
                            //print_set_game(game_two_array, 9);
                            if (status == true) {
                                break;
                            }
                            else{
                                update_ten(game_array[input-1][0],game_array[input-1][1],
                                game_array[input-1][3],game_array[input-1][4]);
                            }
                        }
                    }

                    //win or lose or loop
                    if (return_home) {
                        break;
                    }
                    else if (!reset) {
                        //win or lose
                        if (number_of_turn != 0) {
                           draw_ten_final();
                            volatile int * pixel_ctrl_ptr2 = (int *) 0xFF203020;
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
	                        wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            clear_screen();
                            draw_win();
                         
                            volatile int *SW_ptr = (int *) 0xFF200040;
                           volatile int *LEDR_ptr = (int *) 0xFF200000;
                        int value=*SW_ptr;
                        *LEDR_ptr=value;
                           bool valid_select=true;
                           while(valid_select) {
                    
                                char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    *LEDR_ptr=0;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }

                        } else {
                            clear_screen();
                            draw_fail();
                            *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8; 
                            bool valid_select = true;
                            while(valid_select) {
                                
                              char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }
                        }
                    }
                }


                //level eleven game
                else if (level_of_difficulty=='b') {
                    //initialize the for game one
                    int number_of_turn = 45;
                    int game_array[36][5];
                    set_game_eleven(game_array);
                    clear_screen();
                    draw_eleven_initial();
                    draw_level_eleven();
                     *HEX3_HEX0_ptr = seg7[5]|seg7[4]<<8;
                    //updating
                    while (number_of_turn != 0) {
                        //printf("\nthe remaining chances is: %d/20\n", number_of_turn);
                        
                         if(number_of_turn==44){
                            *HEX3_HEX0_ptr = seg7[4]|seg7[4]<<8;
                        }
                        else if(number_of_turn==43){
                            *HEX3_HEX0_ptr = seg7[3]|seg7[4]<<8;
                        }
                        else if(number_of_turn==42){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[4]<<8;
                        }
                        else if(number_of_turn==41){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[4]<<8;
                        }
                        else if(number_of_turn==40){
                            *HEX3_HEX0_ptr = seg7[0]|seg7[4]<<8;
                        }
                        else if(number_of_turn==39){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[3]<<8;
                        }
                        else if(number_of_turn==38){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[3]<<8;
                        }
                        else if(number_of_turn==37){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[3]<<8;
                        }
                        else if(number_of_turn==36){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[3]<<8;
                        }
                        else if(number_of_turn==35){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[3]<<8;
                        }
                        else if(number_of_turn==34){
                            *HEX3_HEX0_ptr = seg7[4]|seg7[3]<<8;
                        }
                        else if(number_of_turn==33){
                            *HEX3_HEX0_ptr = seg7[3]|seg7[3]<<8;
                        }
                        else if(number_of_turn==32){
                            *HEX3_HEX0_ptr = seg7[2]|seg7[3]<<8;
                        }
                         else if(number_of_turn==31){
                            *HEX3_HEX0_ptr = seg7[1]|seg7[3]<<8;
                        }
                        
                       else if(number_of_turn==30){
                            *HEX3_HEX0_ptr = seg7[0]|seg7[3]<<8;
                        }
                
                        else if(number_of_turn==29){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[2]<<8;
                        }
                        else if(number_of_turn==28){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[2]<<8;
                        }
                        else if(number_of_turn==27){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[2]<<8;
                        }
                        else if(number_of_turn==26){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[2]<<8;
                        }
                        else if(number_of_turn==25){
                            *HEX3_HEX0_ptr = seg7[5]|seg7[2]<<8;
                        }
                        else if(number_of_turn==24){
                            *HEX3_HEX0_ptr = seg7[4]|seg7[2]<<8;
                        }
                        else if(number_of_turn==23){
                            *HEX3_HEX0_ptr = seg7[3]|seg7[2]<<8;
                        }
                        else if(number_of_turn==22){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[2]<<8;
                        }
                        else if(number_of_turn==21){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[2]<<8;
                        }
                        else if(number_of_turn==20){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                        }
                        else  if(number_of_turn==19){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[1]<<8;
                        }
                        else if(number_of_turn==18){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[1]<<8;
                        }
                        else if(number_of_turn==17){
                            *HEX3_HEX0_ptr = seg7[7]|seg7[1]<<8;
                        }
                        else if(number_of_turn==16){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[1]<<8;
                        }
                        else if(number_of_turn==15){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[1]<<8;
                        }
                        else if(number_of_turn==14){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[1]<<8;
                        }
                        else if(number_of_turn==13){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[1]<<8;
                        }
                        else if(number_of_turn==12){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[1]<<8;
                        }
                        else if(number_of_turn==11){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[1]<<8;
                        }
                        else if(number_of_turn==10){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[1]<<8;
                        }
                        else if(number_of_turn==9){
                       *HEX3_HEX0_ptr = seg7[9]|seg7[0]<<8;
                        }
                        else if(number_of_turn==8){
                        *HEX3_HEX0_ptr = seg7[8]|seg7[0]<<8;
                        }
                        else if(number_of_turn==7){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[0]<<8;
                        }
                        else if(number_of_turn==6){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[0]<<8;
                        }
                        else if(number_of_turn==5){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[0]<<8;
                        }
                        else if(number_of_turn==4){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[0]<<8;
                        }
                        else if(number_of_turn==3){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                        }
                        else if(number_of_turn==2){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[0]<<8;
                        }
                        else if(number_of_turn==1){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[0]<<8;
                        }
                         else if(number_of_turn==0){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8;    
                        }
                        char row=check_enter();
                        if (row == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (row == 'h') {
                            return_home = true;
                            break;
                        } else if (row == 'r') {
                            reset = true;
                            break;
                        }

                       
                       char column=check_enter();
                       number_of_turn--;
                        if (column == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (column == 'h') {
                            return_home = true;
                            break;
                        } else if (column == 'r') {
                            reset = true;
                            break;
                        }

                        //update the database
                        if ((row != '1' && row != '2' && row != '3') || (column != '1' && column != '2' && column != '3'&&column!='4'
                        &&column!='5'&&column!='6'&&column!='7'&&column!='8'&&column!='9')) {
                           // printf("you should enter a number within the range 1 to 9. Please try again.\n");
                            number_of_turn++;
                        } else {
                            //get input
                            int row_int = row - '0';
                            int column_int = column - '0';
                            int input = (row_int - 1) *9 + column_int;
                           
                            bool status;
                            status = update_database_eleven(game_array, input);
                            //print_set_game(game_two_array, 9);
                            if (status == true) {
                                break;
                            }
                            else{
                                update_eleven(game_array[input-1][0],game_array[input-1][1],
                                game_array[input-1][3],game_array[input-1][4]);
                            }
                        }
                    }

                    //win or lose or loop
                    if (return_home) {
                        break;
                    }
                    else if (!reset) {
                        //win or lose
                        if (number_of_turn != 0) {
                           
                            draw_eleven_final();
                            volatile int * pixel_ctrl_ptr2 = (int *) 0xFF203020;
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
	                        wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            clear_screen();
                            draw_win();
                         
                            volatile int *SW_ptr = (int *) 0xFF200040;
                           volatile int *LEDR_ptr = (int *) 0xFF200000;
                        int value=*SW_ptr;
                        *LEDR_ptr=value;
                           bool valid_select=true;
                           while(valid_select) {
                    
                                char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    *LEDR_ptr=0;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }

                        } else {
                            clear_screen();
                            draw_fail();
                            *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8; 
                            bool valid_select = true;
                            while(valid_select) {
                                
                              char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }
                        }
                    }
                }
                else if (level_of_difficulty=='c') {
                    //initialize the for game one
                    int number_of_turn = 45;
                    int game_array[34][5];
                    set_game_twelve(game_array);
                    clear_screen();
                    draw_twelve_initial();
                    draw_level_twelve();
                    *HEX3_HEX0_ptr = seg7[5]|seg7[4]<<8;
                    //updating
                    while (number_of_turn != 0) {
                        //printf("\nthe remaining chances is: %d/20\n", number_of_turn);
                        
                        if(number_of_turn==45){
                            *HEX3_HEX0_ptr = seg7[5]|seg7[4]<<8;
                        }
                        else if(number_of_turn==44){
                            *HEX3_HEX0_ptr = seg7[4]|seg7[4]<<8;
                        }
                        else if(number_of_turn==43){
                            *HEX3_HEX0_ptr = seg7[3]|seg7[4]<<8;
                        }
                        else if(number_of_turn==42){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[4]<<8;
                        }
                        else if(number_of_turn==41){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[4]<<8;
                        }
                        else if(number_of_turn==40){
                            *HEX3_HEX0_ptr = seg7[0]|seg7[4]<<8;
                        }
                        else if(number_of_turn==39){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[3]<<8;
                        }
                        else if(number_of_turn==38){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[3]<<8;
                        }
                        else if(number_of_turn==37){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[3]<<8;
                        }
                        else if(number_of_turn==36){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[3]<<8;
                        }
                        else if(number_of_turn==35){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[3]<<8;
                        }
                        else if(number_of_turn==34){
                            *HEX3_HEX0_ptr = seg7[4]|seg7[3]<<8;
                        }
                        else if(number_of_turn==33){
                            *HEX3_HEX0_ptr = seg7[3]|seg7[3]<<8;
                        }
                        else if(number_of_turn==32){
                            *HEX3_HEX0_ptr = seg7[2]|seg7[3]<<8;
                        }
                         else if(number_of_turn==31){
                            *HEX3_HEX0_ptr = seg7[1]|seg7[3]<<8;
                        }
                        
                       else if(number_of_turn==30){
                            *HEX3_HEX0_ptr = seg7[0]|seg7[3]<<8;
                        }
                
                        else if(number_of_turn==29){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[2]<<8;
                        }
                        else if(number_of_turn==28){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[2]<<8;
                        }
                        else if(number_of_turn==27){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[2]<<8;
                        }
                        else if(number_of_turn==26){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[2]<<8;
                        }
                        else if(number_of_turn==25){
                            *HEX3_HEX0_ptr = seg7[5]|seg7[2]<<8;
                        }
                        else if(number_of_turn==24){
                            *HEX3_HEX0_ptr = seg7[4]|seg7[2]<<8;
                        }
                        else if(number_of_turn==23){
                            *HEX3_HEX0_ptr = seg7[3]|seg7[2]<<8;
                        }
                        else if(number_of_turn==22){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[2]<<8;
                        }
                        else if(number_of_turn==21){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[2]<<8;
                        }
                        else if(number_of_turn==20){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[2]<<8;
                        }
                        else  if(number_of_turn==19){
                            *HEX3_HEX0_ptr = seg7[9]|seg7[1]<<8;
                        }
                        else if(number_of_turn==18){
                            *HEX3_HEX0_ptr = seg7[8]|seg7[1]<<8;
                        }
                        else if(number_of_turn==17){
                            *HEX3_HEX0_ptr = seg7[7]|seg7[1]<<8;
                        }
                        else if(number_of_turn==16){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[1]<<8;
                        }
                        else if(number_of_turn==15){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[1]<<8;
                        }
                        else if(number_of_turn==14){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[1]<<8;
                        }
                        else if(number_of_turn==13){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[1]<<8;
                        }
                        else if(number_of_turn==12){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[1]<<8;
                        }
                        else if(number_of_turn==11){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[1]<<8;
                        }
                        else if(number_of_turn==10){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[1]<<8;
                        }
                        else if(number_of_turn==9){
                       *HEX3_HEX0_ptr = seg7[9]|seg7[0]<<8;
                        }
                        else if(number_of_turn==8){
                        *HEX3_HEX0_ptr = seg7[8]|seg7[0]<<8;
                        }
                        else if(number_of_turn==7){
                        *HEX3_HEX0_ptr = seg7[7]|seg7[0]<<8;
                        }
                        else if(number_of_turn==6){
                        *HEX3_HEX0_ptr = seg7[6]|seg7[0]<<8;
                        }
                        else if(number_of_turn==5){
                        *HEX3_HEX0_ptr = seg7[5]|seg7[0]<<8;
                        }
                        else if(number_of_turn==4){
                        *HEX3_HEX0_ptr = seg7[4]|seg7[0]<<8;
                        }
                        else if(number_of_turn==3){
                        *HEX3_HEX0_ptr = seg7[3]|seg7[0]<<8;
                        }
                        else if(number_of_turn==2){
                        *HEX3_HEX0_ptr = seg7[2]|seg7[0]<<8;
                        }
                        else if(number_of_turn==1){
                        *HEX3_HEX0_ptr = seg7[1]|seg7[0]<<8;
                        }
                        else if(number_of_turn==0){
                        *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8;    
                        }
                        char row=check_enter();
                        if (row == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (row == 'h') {
                            return_home = true;
                            break;
                        } else if (row == 'r') {
                            reset = true;
                            break;
                        }

                       
                       char column=check_enter();
                       
                       number_of_turn--;
                        if (column == 'q') {
                            run = false;
                            clear_screen();
                            return 0;
                        } else if (column == 'h') {
                            return_home = true;
                            break;
                        } else if (column == 'r') {
                            reset = true;
                            break;
                        }

                        //update the database
                        if ((row != '1' && row != '2' && row != '3'&&row!='4') || (column != '1' && column != '2' && column != '3'&&column!='4'
                        &&column!='5'&&column!='6'&&column!='7'&&column!='8'&&column!='9')||
                        (row=='2'&&column=='9')||(row=='3'&&column=='9')) {
                           // printf("you should enter a number within the range 1 to 9. Please try again.\n");
                            number_of_turn++;
                        } else {
                            //get input
                            int row_int = row - '0';
                            int column_int = column - '0';
                            int input = (row_int - 1) *9 + column_int;
                           
                            bool status;
                            status = update_database_twelve(game_array, input);
                            //print_set_game(game_two_array, 9);
                            if (status == true) {
                                break;
                            }
                            else{
                                update_twelve(game_array[input-1][0],game_array[input-1][1],
                                game_array[input-1][3],game_array[input-1][4]);
                            }
                        }
                    }

                    //win or lose or loop
                    if (return_home) {
                        break;
                    }
                    else if (!reset) {
                        //win or lose
                        if (number_of_turn != 0) {
                           draw_twelve_final();
                            volatile int * pixel_ctrl_ptr2 = (int *) 0xFF203020;
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
	                        wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            wait_screen(pixel_ctrl_ptr2);
                            clear_screen();
                            draw_win();
                         
                            volatile int *SW_ptr = (int *) 0xFF200040;
                           volatile int *LEDR_ptr = (int *) 0xFF200000;
                        int value=*SW_ptr;
                        *LEDR_ptr=value;
                           bool valid_select=true;
                           while(valid_select) {
                    
                                char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    *LEDR_ptr=0;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }

                        } else {
                            clear_screen();
                            draw_fail();
                            *HEX3_HEX0_ptr = seg7[0]|seg7[0]<<8; 
                            bool valid_select = true;
                            while(valid_select) {
                                
                              char choice=check_enter();
                                if (choice == 'q') {
                                    valid_select=false;
                                    run = false;
                                    clear_screen();
                                    return 0;
                                } else if (choice == 'h') {
                                    valid_select=false;
                                    return_home = true;
                                    break;
                                } else if (choice == 'r') {
                                    valid_select=false;
                                    reset = true;
                                }
                            }
                        }
                    }
                }
                //level twelve game
                
            }
        }
    }
}

//helper function
void print_set_game(int array[][5], int size){
    for(int i=0; i<size; i++) {
        printf("number: %d + ", i);
        printf("type: %d + ", array[i][3]);
        printf("water: %d + ", array[i][2]);
        printf("position: %d", array[i][4]);
        printf("\n");
    }
}

bool update_database_one(int array[][5], int input){
    //update the position for the input index
    if(input != 5) {
        array[input - 1][4]--;
        if (array[input - 1][4] == -1) {
            array[input - 1][4] = 3;
        }
    }

    //check if valid
    if(array[0][4]==0 && array[2][4]==3 && array[3][4]==1 && array[5][4]==0){
        for(int i = 0; i<6; i++){
            if(i!=1){
                array[i][2]=1;
            }
        }
        return true;
    }
    else{
        return false;
    }
}

void set_game_one(int array[][5]) {
    array[0][0] = 1;
    array[0][1] = 1;
    array[0][2] = 0;
    array[0][3] = 1;
    array[0][4] = 0;

    array[1][0] =1;
    array[1][1] =2;
    array[1][2] =0;
    array[1][3] =4;
    array[1][4] =0;

    array[2][0] =2;
    array[2][1] =1;
    array[2][2] =0;
    array[2][3] =4;
    array[2][4] =3;

    array[3][0] =2;
    array[3][1] =2;
    array[3][2] =0;
    array[3][3] =4;
    array[3][4] =2;

    array[4][0] =3;
    array[4][1] =1;
    array[4][2] =0;
    array[4][3] =2;
    array[4][4] =3;

    array[5][0] =3;
    array[5][1] =2;
    array[5][2] =0;
    array[5][3] =4;
    array[5][4] =0;
}

bool update_database_two(int array[][5], int input) {
    //update the position for the input index
    if(input != 9) {
        array[input - 1][4]--;
        if (array[input - 1][4] == -1) {
            array[input - 1][4] = 3;
        }
    }

    //check if valid
    if(array[0][4]==0 && array[3][4]==3 && (array[4][4]==0 || array[4][4]==2) && array[5][4]==1 && array[8][4]==0){
        for(int i = 0; i<9; i++){
            if(i!=1 && i!=2 && i!=6 && i!=7){
                array[i][2]=1;
            }
        }
        return true;
    }
    else{
        return false;
    }
}

void set_game_two(int array[][5]) {
    array[0][0] = 1;
    array[0][1] = 1;
    array[0][2] = 0;
    array[0][3] = 1;
    array[0][4] = 1;

    array[1][0] = 1;
    array[1][1] = 2;
    array[1][2] = 0;
    array[1][3] = 3;
    array[1][4] = 3;

    array[2][0] = 1;
    array[2][1] = 3;
    array[2][2] = 0;
    array[2][3] = 4;
    array[2][4] = 1;

    array[3][0] = 2;
    array[3][1] = 1;
    array[3][2] = 0;
    array[3][3] = 4;
    array[3][4] = 1;

    array[4][0] = 2;
    array[4][1] = 2;
    array[4][2] = 0;
    array[4][3] = 3;
    array[4][4] = 3;

    array[5][0] = 2;
    array[5][1] = 3;
    array[5][2] = 0;
    array[5][3] = 4;
    array[5][4] = 0;

    array[6][0] = 3;
    array[6][1] = 1;
    array[6][2] = 0;
    array[6][3] = 4;
    array[6][4] = 3;

    array[7][0] = 3;
    array[7][1] = 2;
    array[7][2] = 0;
    array[7][3] = 3;
    array[7][4] = 0;

    array[8][0] = 3;
    array[8][1] = 3;
    array[8][2] = 0;
    array[8][3] = 2;
    array[8][4] = 0;
}

bool update_database_three(int array[][5], int input){
    //update the position for the input index
    if(input != 11) {
        array[input - 1][4]--;
        if (array[input - 1][4] == -1) {
            array[input - 1][4] = 3;
        }
    }

    //check if valid
    if(array[3][4]==2 && array[4][4]==3 && array[5][4]== 2 && array[6][4]==1 && array[7][4]==2 && array[8][4]==0
                                        && array[10][4]==0 && array[11][4]==3 && array[12][4]==0){
        for(int i = 0; i<15; i++){
            if(i==3 || i==4 || i==5 || i==6 || i==7 || i==8 || i==10 || i==11 || i==12){
                array[i][2]=1;
            }
        }
        return true;
    }
    else{
        return false;
    }
}

void set_game_three(int array[][5]){
    array[0][0] = 1;
    array[0][1] = 1;
    array[0][2] = 0;
    array[0][3] = 4;
    array[0][4] = 2;

    array[1][0] = 1;
    array[1][1] = 2;
    array[1][2] = 0;
    array[1][3] = 4;
    array[1][4] = 1;

    array[2][0] = 1;
    array[2][1] = 3;
    array[2][2] = 0;
    array[2][3] = 4;
    array[2][4] = 1;

    array[3][0] = 1;
    array[3][1] = 4;
    array[3][2] = 0;
    array[3][3] = 4;
    array[3][4] = 2;

    array[4][0] = 1;
    array[4][1] = 5;
    array[4][2] = 0;
    array[4][3] = 1;
    array[4][4] = 0;

    array[5][0] = 2;
    array[5][1] = 1;
    array[5][2] = 0;
    array[5][3] = 4;
    array[5][4] = 3;

    array[6][0] = 2;
    array[6][1] = 2;
    array[6][2] = 0;
    array[6][3] = 4;
    array[6][4] = 1;

    array[7][0] = 2;
    array[7][1] = 3;
    array[7][2] = 0;
    array[7][3] = 4;
    array[7][4] = 3;

    array[8][0] = 2;
    array[8][1] = 4;
    array[8][2] = 0;
    array[8][3] = 4;
    array[8][4] = 1;

    array[9][0] = 2;
    array[9][1] = 5;
    array[9][2] = 0;
    array[9][3] = 4;
    array[9][4] = 1;

    array[10][0] = 3;
    array[10][1] = 1;
    array[10][2] = 0;
    array[10][3] = 2;
    array[10][4] = 0;

    array[11][0] = 3;
    array[11][1] = 2;
    array[11][2] = 0;
    array[11][3] = 4;
    array[11][4] = 0;

    array[12][0] = 3;
    array[12][1] = 3;
    array[12][2] = 0;
    array[12][3] = 4;
    array[12][4] = 3;

    array[13][0] = 3;
    array[13][1] = 4;
    array[13][2] = 0;
    array[13][3] = 4;
    array[13][4] = 0;

    array[14][0] = 3;
    array[14][1] = 5;
    array[14][2] = 0;
    array[14][3] = 4;
    array[14][4] = 0;
}

bool update_database_four(int array[][5], int input){
    //update the position for the input index
    if(input != 10) {
        array[input - 1][4]--;
        if (array[input - 1][4] == -1) {
            array[input - 1][4] = 3;
        }
    }

    //check if valid
    if(array[0][4]==2 && (array[1][4]==0 || array[1][4]==2) && array[2][4]==1 && array[4][4]==1 && array[5][4]==0
                        && array[6][4]==2 && array[7][4]==0 && array[9][4]==3 && (array[10][4]==0 || array[10][4]==2) && array[11][4]==0){
        for(int i = 0; i<13; i++){
            if(i==0 || i==1 || i==2 || i==4 || i==5 || i==6 || i==7 || i==9 || i==10 || i==11){
                array[i][2]=1;
            }
        }
        return true;
    }
    else{
        return false;
    }
}

void set_game_four(int array[][5]){
    array[0][0] = 1;
    array[0][1] = 2;
    array[0][2] = 0;
    array[0][3] = 4;
    array[0][4] = 1;

    array[1][0] = 1;
    array[1][1] = 3;
    array[1][2] = 0;
    array[1][3] = 3;
    array[1][4] = 3;

    array[2][0] = 1;
    array[2][1] = 4;
    array[2][2] = 0;
    array[2][3] = 4;
    array[2][4] = 2;

    array[3][0] = 1;
    array[3][1] = 5;
    array[3][2] = 0;
    array[3][3] = 4;
    array[3][4] = 0;

    array[4][0] = 2;
    array[4][1] = 1;
    array[4][2] = 0;
    array[4][3] = 1;
    array[4][4] = 1;

    array[5][0] = 2;
    array[5][1] = 2;
    array[5][2] = 0;
    array[5][3] = 4;
    array[5][4] = 1;

    array[6][0] = 2;
    array[6][1] = 3;
    array[6][2] = 0;
    array[6][3] = 4;
    array[6][4] = 3;

    array[7][0] = 2;
    array[7][1] = 4;
    array[7][2] = 0;
    array[7][3] = 4;
    array[7][4] = 1;

    array[8][0] = 2;
    array[8][1] = 5;
    array[8][2] = 0;
    array[8][3] = 4;
    array[8][4] = 1;

    array[9][0] = 3;
    array[9][1] = 1;
    array[9][2] = 0;
    array[9][3] = 2;
    array[9][4] = 3;

    array[10][0] = 3;
    array[10][1] = 2;
    array[10][2] = 0;
    array[10][3] = 3;
    array[10][4] = 2;

    array[11][0] = 3;
    array[11][1] = 3;
    array[11][2] = 0;
    array[11][3] = 4;
    array[11][4] = 3;

    array[12][0] = 3;
    array[12][1] = 4;
    array[12][2] = 0;
    array[12][3] = 4;
    array[12][4] = 0;
}

bool update_database_five(int array[][5], int input){
    //update the position for the input index
    if(input != 9) {
        array[input - 1][4]--;
        if (array[input - 1][4] == -1) {
            array[input - 1][4] = 3;
        }
    }

    //check if valid
    if(array[2][4]==0 && array[4][4]==2 && array[5][4]==1 && (array[6][4]==1 || array[6][4]==3) && array[8][4]==0 && array[9][4]==3 && array[10][4]==0){
        for(int i = 0; i<12; i++){
            if(i==2 || i==4 || i==5 || i==6 || i==8 || i==9 || i==10){
                array[i][2]=1;
            }
        }
        return true;
    }
    else{
        return false;
    }
}

void set_game_five(int array[][5]){
    array[0][0] = 1;
    array[0][1] = 1;
    array[0][2] = 0;
    array[0][3] = 4;
    array[0][4] = 2;

    array[1][0] = 1;
    array[1][1] = 2;
    array[1][2] = 0;
    array[1][3] = 3;
    array[1][4] = 0;

    array[2][0] = 1;
    array[2][1] = 3;
    array[2][2] = 0;
    array[2][3] = 1;
    array[2][4] = 1;

    array[3][0] = 1;
    array[3][1] = 4;
    array[3][2] = 0;
    array[3][3] = 4;
    array[3][4] = 1;

    array[4][0] = 2;
    array[4][1] = 1;
    array[4][2] = 0;
    array[4][3] = 4;
    array[4][4] = 1;

    array[5][0] = 2;
    array[5][1] = 2;
    array[5][2] = 0;
    array[5][3] = 4;
    array[5][4] = 1;

    array[6][0] = 2;
    array[6][1] = 3;
    array[6][2] = 0;
    array[6][3] = 3;
    array[6][4] = 1;

    array[7][0] = 2;
    array[7][1] = 4;
    array[7][2] = 0;
    array[7][3] = 4;
    array[7][4] = 0;

    array[8][0] = 3;
    array[8][1] = 1;
    array[8][2] = 0;
    array[8][3] = 2;
    array[8][4] = 0;

    array[9][0] = 3;
    array[9][1] = 2;
    array[9][2] = 0;
    array[9][3] = 4;
    array[9][4] = 1;

    array[10][0] = 3;
    array[10][1] = 3;
    array[10][2] = 0;
    array[10][3] = 4;
    array[10][4] = 3;

    array[11][0] = 3;
    array[11][1] = 4;
    array[11][2] = 0;
    array[11][3] = 4;
    array[11][4] = 1;
}

bool update_database_six(int array[][5], int input){
    //update the position for the input index
    if(input != 1 && input !=13) {
        array[input - 1][4]--;
        if (array[input - 1][4] == -1) {
            array[input - 1][4] = 3;
        }
    }

    //check if valid
    if(array[0][4]==2 && array[1][4]==1 && (array[2][4]==0 || array[2][4]==2) && (array[3][4]==0 || array[3][4]==2) && array[4][4]==1 && (array[5][4]==1 || array[5][4]==3) &&
                    array[6][4]==2 && array[7][4]==0 && array[8][4]==3 && (array[9][4]==0 || array[9][4]==2) && (array[10][4]==0 || array[10][4]==2) && array[11][4]==0 && array[12][4]==3){
        for(int i = 0; i<13; i++){
            array[i][2]=1;
        }
        return true;
    }
    else{
        return false;
    }
}

void set_game_six(int array[][5]){
    array[0][0] = 1;
    array[0][1] = 1;
    array[0][2] = 0;
    array[0][3] = 2;
    array[0][4] = 2;

    array[1][0] = 1;
    array[1][1] = 2;
    array[1][2] = 0;
    array[1][3] = 1;
    array[1][4] = 1;

    array[2][0] = 1;
    array[2][1] = 3;
    array[2][2] = 0;
    array[2][3] = 3;
    array[2][4] = 1;

    array[3][0] = 1;
    array[3][1] = 4;
    array[3][2] = 0;
    array[3][3] = 3;
    array[3][4] = 0;

    array[4][0] = 1;
    array[4][1] = 5;
    array[4][2] = 0;
    array[4][3] = 4;
    array[4][4] = 1;

    array[5][0] = 2;
    array[5][1] = 1;
    array[5][2] = 0;
    array[5][3] = 3;
    array[5][4] = 1;

    array[6][0] = 2;
    array[6][1] = 4;
    array[6][2] = 0;
    array[6][3] = 4;
    array[6][4] = 3;

    array[7][0] = 2;
    array[7][1] = 5;
    array[7][2] = 0;
    array[7][3] = 4;
    array[7][4] = 0;

    array[8][0] = 3;
    array[8][1] = 1;
    array[8][2] = 0;
    array[8][3] = 4;
    array[8][4] = 3;

    array[9][0] = 3;
    array[9][1] = 2;
    array[9][2] = 0;
    array[9][3] = 3;
    array[9][4] = 1;

    array[10][0] = 3;
    array[10][1] = 3;
    array[10][2] = 0;
    array[10][3] = 3;
    array[10][4] = 0;

    array[11][0] = 3;
    array[11][1] = 4;
    array[11][2] = 0;
    array[11][3] = 5;
    array[11][4] = 1;

    array[12][0] = 3;
    array[12][1] = 5;
    array[12][2] = 0;
    array[12][3] = 2;
    array[12][4] = 3;
}

bool update_database_seven(int array[][5], int input){
    //update the position for the input index
    if(input !=1 && input !=12) {
        array[input - 1][4]--;
        if (array[input - 1][4] == -1) {
            array[input - 1][4] = 3;
        }
    }

    //check if valid
    if(array[0][4]==2 && array[2][4]==0 && array[4][4]==3 && (array[5][4]==2 || array[5][4]==0) && array[6][4]==0 && array[7][4]==1 && array[11][4]==0){
        for(int i = 0; i<12; i++){
            if(i==0 || i==2 || i==4 || i==5 || i==6 || i==7 || i==11){
                array[i][2]=1;
            }
        }
        return true;
    }
    else{
        return false;
    }
}

void set_game_seven(int array[][5]){
    array[0][0] = 1;
    array[0][1] = 1;
    array[0][2] = 0;
    array[0][3] = 2;
    array[0][4] = 2;

    array[1][0] = 1;
    array[1][1] = 2;
    array[1][2] = 0;
    array[1][3] = 4;
    array[1][4] = 0;

    array[2][0] = 1;
    array[2][1] = 3;
    array[2][2] = 0;
    array[2][3] = 1;
    array[2][4] = 3;

    array[3][0] = 1;
    array[3][1] = 4;
    array[3][2] = 0;
    array[3][3] = 4;
    array[3][4] = 1;

    array[4][0] = 2;
    array[4][1] = 1;
    array[4][2] = 0;
    array[4][3] = 4;
    array[4][4] = 1;

    array[5][0] = 2;
    array[5][1] = 2;
    array[5][2] = 0;
    array[5][3] = 3;
    array[5][4] = 1;

    array[6][0] = 2;
    array[6][1] = 3;
    array[6][2] = 0;
    array[6][3] = 5;
    array[6][4] = 2;

    array[7][0] = 2;
    array[7][1] = 4;
    array[7][2] = 0;
    array[7][3] = 4;
    array[7][4] = 0;

    array[8][0] = 3;
    array[8][1] = 1;
    array[8][2] = 0;
    array[8][3] = 4;
    array[8][4] = 1;

    array[9][0] = 3;
    array[9][1] = 2;
    array[9][2] = 0;
    array[9][3] = 4;
    array[9][4] = 1;

    array[10][0] = 3;
    array[10][1] = 3;
    array[10][2] = 0;
    array[10][3] = 4;
    array[10][4] = 3;

    array[11][0] = 3;
    array[11][1] = 4;
    array[11][2] = 0;
    array[11][3] = 2;
    array[11][4] = 0;
}

bool update_database_eight(int array[][5], int input){
    //update the position for the input index
    if(input != 4 && input != 15 && input!=18) {
        array[input - 1][4]--;
        if (array[input - 1][4] == -1) {
            array[input - 1][4] = 3;
        }
    }

    //check if valid
    if(array[0][4]==2 && array[1][4]==3 && array[2][4]==2 && array[3][4]==3 && (array[6][4]==1 || array[6][4]==3) && array[7][4]==2 &&
                        array[8][4]==0 && array[10][4]==2 && array[11][4]==3 && array[12][4]==3 && array[13][4]==0 && array[14][4]==3 &&
                        array[16][4]==3 && array[17][4]==3){
        for(int i = 0; i<18; i++){
            if(i!=4 && i!=5 && i!=9 && i!=15){
                array[i][2]=1;
            }
        }
        return true;
    }
    else{
        return false;
    }
}

void set_game_eight(int array[][5]){
    array[0][0] = 1;
    array[0][1] = 1;
    array[0][2] = 0;
    array[0][3] = 4;
    array[0][4] = 3;

    array[1][0] = 1;
    array[1][1] = 2;
    array[1][2] = 0;
    array[1][3] = 1;
    array[1][4] = 0;

    array[2][0] = 1;
    array[2][1] = 3;
    array[2][2] = 0;
    array[2][3] = 4;
    array[2][4] = 3;

    array[3][0] = 1;
    array[3][1] = 4;
    array[3][2] = 0;
    array[3][3] = 2;
    array[3][4] = 3;

    array[4][0] = 1;
    array[4][1] = 5;
    array[4][2] = 0;
    array[4][3] = 4;
    array[4][4] = 1;

    array[5][0] = 1;
    array[5][1] = 6;
    array[5][2] = 0;
    array[5][3] = 3;
    array[5][4] = 1;

    array[6][0] = 2;
    array[6][1] = 1;
    array[6][2] = 0;
    array[6][3] = 3;
    array[6][4] = 1;

    array[7][0] = 2;
    array[7][1] = 2;
    array[7][2] = 0;
    array[7][3] = 4;
    array[7][4] = 0;

    array[8][0] = 2;
    array[8][1] = 3;
    array[8][2] = 0;
    array[8][3] = 4;
    array[8][4] = 3;

    array[9][0] = 2;
    array[9][1] = 4;
    array[9][2] = 0;
    array[9][3] = 3;
    array[9][4] = 0;

    array[10][0] = 2;
    array[10][1] = 5;
    array[10][2] = 0;
    array[10][3] = 4;
    array[10][4] = 3;

    array[11][0] = 2;
    array[11][1] = 6;
    array[11][2] = 0;
    array[11][3] = 1;
    array[11][4] = 1;

    array[12][0] = 3;
    array[12][1] = 1;
    array[12][2] = 0;
    array[12][3] = 4;
    array[12][4] = 3;

    array[13][0] = 3;
    array[13][1] = 2;
    array[13][2] = 0;
    array[13][3] = 5;
    array[13][4] = 1;

    array[14][0] = 3;
    array[14][1] = 3;
    array[14][2] = 0;
    array[14][3] = 2;
    array[14][4] = 3;

    array[15][0] = 3;
    array[15][1] = 4;
    array[15][2] = 0;
    array[15][3] = 4;
    array[15][4] = 3;

    array[16][0] = 3;
    array[16][1] = 5;
    array[16][2] = 0;
    array[16][3] = 4;
    array[16][4] = 0;

    array[17][0] = 3;
    array[17][1] = 6;
    array[17][2] = 0;
    array[17][3] = 2;
    array[17][4] = 3;
}

bool update_database_nine(int array[][5], int input){
    //update the position for the input index
    if(input != 7 && input != 10) {
        array[input - 1][4]--;
        if (array[input - 1][4] == -1) {
            array[input - 1][4] = 3;
        }
    }

    //check if valid
    if(array[0][4]==2 && array[1][4]==2 && (array[2][4]==2 || array[2][4]==0) && (array[3][4]==2 || array[3][4]==0) && (array[4][4]==2 || array[4][4]==0) && array[5][4]==1 &&
                        array[6][4]==0 && (array[7][4]==1 || array[7][4]==3) && array[9][4]==2 && array[10][4]==2 && array[11][4]==0 && array[13][4]==3 &&
                        array[14][4]==3 && array[15][4]==3 && array[16][4]==0){
        for(int i = 0; i<18; i++){
            if(i!=8 && i!=12 && i!=17){
                array[i][2]=1;
            }
        }
        return true;
    }
    else{
        return false;
    }
}

void set_game_nine(int array[][5]){
    array[0][0] = 1;
    array[0][1] = 1;
    array[0][2] = 0;
    array[0][3] = 4;
    array[0][4] = 3;

    array[1][0] = 1;
    array[1][1] = 2;
    array[1][2] = 0;
    array[1][3] = 5;
    array[1][4] = 1;

    array[2][0] = 1;
    array[2][1] = 3;
    array[2][2] = 0;
    array[2][3] = 3;
    array[2][4] = 1;

    array[3][0] = 1;
    array[3][1] = 4;
    array[3][2] = 0;
    array[3][3] = 3;
    array[3][4] = 0;

    array[4][0] = 1;
    array[4][1] = 5;
    array[4][2] = 0;
    array[4][3] = 3;
    array[4][4] = 1;

    array[5][0] = 1;
    array[5][1] = 6;
    array[5][2] = 0;
    array[5][3] = 4;
    array[5][4] = 1;

    array[6][0] = 2;
    array[6][1] = 1;
    array[6][2] = 0;
    array[6][3] = 2;
    array[6][4] = 0;

    array[7][0] = 2;
    array[7][1] = 2;
    array[7][2] = 0;
    array[7][3] = 3;
    array[7][4] = 0;

    array[8][0] = 2;
    array[8][1] = 3;
    array[8][2] = 0;
    array[8][3] = 3;
    array[8][4] = 1;

    array[9][0] = 2;
    array[9][1] = 4;
    array[9][2] = 0;
    array[9][3] = 2;
    array[9][4] = 2;

    array[10][0] = 2;
    array[10][1] = 5;
    array[10][2] = 0;
    array[10][3] = 4;
    array[10][4] = 3;

    array[11][0] = 2;
    array[11][1] = 6;
    array[11][2] = 0;
    array[11][3] = 4;
    array[11][4] = 1;

    array[12][0] = 3;
    array[12][1] = 1;
    array[12][2] = 0;
    array[12][3] = 4;
    array[12][4] = 3;

    array[13][0] = 3;
    array[13][1] = 2;
    array[13][2] = 0;
    array[13][3] = 4;
    array[13][4] = 0;

    array[14][0] = 3;
    array[14][1] = 3;
    array[14][2] = 0;
    array[14][3] = 1;
    array[14][4] = 2;

    array[15][0] = 3;
    array[15][1] = 4;
    array[15][2] = 0;
    array[15][3] = 4;
    array[15][4] = 0;

    array[16][0] = 3;
    array[16][1] = 5;
    array[16][2] = 0;
    array[16][3] = 4;
    array[16][4] = 0;

    array[17][0] = 3;
    array[17][1] = 6;
    array[17][2] = 0;
    array[17][3] = 4;
    array[17][4] = 2;
}

bool update_database_ten(int array[][5], int input){
    //update the position for the input index
    if(input != 4 && input != 15 && input != 21) {
        array[input - 1][4]--;
        if (array[input - 1][4] == -1) {
            array[input - 1][4] = 3;
        }
    }

    //check if valid
    if(array[0][4]==1 && (array[1][4]==0 || array[1][4]==2) && array[2][4]==1 && array[3][4]==2 && array[7][4]==2 && array[8][4]==1 &&
                                (array[9][4]==1 || array[9][4]==3) && array[10][4]==3 && (array[11][4]==0 || array[11][4]==2) &&
                                array[12][4]==1 && array[14][4]==0 && array[15][4]==3 &&
                                array[16][4]==0 && (array[17][4]==0 || array[17][4]==2) && (array[18][4]==0 || array[18][4]==2) && array[19][4]==0 && array[20][4]==3){
        for(int i = 0; i<21; i++){
            if(i!=4 && i!=5 && i!=6 && i!=13){
                array[i][2]=1;
            }
        }
        return true;
    }
    else{
        return false;
    }
}

void set_game_ten(int array[][5]){
    array[0][0] = 1;
    array[0][1] = 1;
    array[0][2] = 0;
    array[0][3] = 1;
    array[0][4] = 1;

    array[1][0] = 1;
    array[1][1] = 2;
    array[1][2] = 0;
    array[1][3] = 3;
    array[1][4] = 0;

    array[2][0] = 1;
    array[2][1] = 3;
    array[2][2] = 0;
    array[2][3] = 4;
    array[2][4] = 3;

    array[3][0] = 1;
    array[3][1] = 4;
    array[3][2] = 0;
    array[3][3] = 2;
    array[3][4] = 2;

    array[4][0] = 1;
    array[4][1] = 5;
    array[4][2] = 0;
    array[4][3] = 4;
    array[4][4] = 1;

    array[5][0] = 1;
    array[5][1] = 6;
    array[5][2] = 0;
    array[5][3] = 3;
    array[5][4] = 1;

    array[6][0] = 1;
    array[6][1] = 7;
    array[6][2] = 0;
    array[6][3] = 3;
    array[6][4] = 1;

    array[7][0] = 2;
    array[7][1] = 1;
    array[7][2] = 0;
    array[7][3] = 4;
    array[7][4] = 0;

    array[8][0] = 2;
    array[8][1] = 2;
    array[8][2] = 0;
    array[8][3] = 4;
    array[8][4] = 3;

    array[9][0] = 2;
    array[9][1] = 3;
    array[9][2] = 0;
    array[9][3] = 3;
    array[9][4] = 0;

    array[10][0] = 2;
    array[10][1] = 4;
    array[10][2] = 0;
    array[10][3] = 4;
    array[10][4] = 3;

    array[11][0] = 2;
    array[11][1] = 5;
    array[11][2] = 0;
    array[11][3] = 3;
    array[11][4] = 1;

    array[12][0] = 2;
    array[12][1] = 6;
    array[12][2] = 0;
    array[12][3] = 4;
    array[12][4] = 3;

    array[13][0] = 2;
    array[13][1] = 7;
    array[13][2] = 0;
    array[13][3] = 5;
    array[13][4] = 1;

    array[14][0] = 3;
    array[14][1] = 1;
    array[14][2] = 0;
    array[14][3] = 2;
    array[14][4] = 0;

    array[15][0] = 3;
    array[15][1] = 2;
    array[15][2] = 0;
    array[15][3] = 4;
    array[15][4] = 3;

    array[16][0] = 3;
    array[16][1] = 3;
    array[16][2] = 0;
    array[16][3] = 5;
    array[16][4] = 1;

    array[17][0] = 3;
    array[17][1] = 4;
    array[17][2] = 0;
    array[17][3] = 3;
    array[17][4] = 0;

    array[18][0] = 3;
    array[18][1] = 5;
    array[18][2] = 0;
    array[18][3] = 3;
    array[18][4] = 1;

    array[19][0] = 3;
    array[19][1] = 6;
    array[19][2] = 0;
    array[19][3] = 5;
    array[19][4] = 1;

    array[20][0] = 3;
    array[20][1] = 7;
    array[20][2] = 0;
    array[20][3] = 2;
    array[20][4] = 3;
}

bool update_database_eleven(int array[][5], int input){
    //update the position for the input index
    if(input != 4 && input != 18 && input != 33) {
        array[input - 1][4]--;
        if (array[input - 1][4] == -1) {
            array[input - 1][4] = 3;
        }
    }

    //check if valid
    if(array[0][4]==0 && array[3][4]==2 && array[5][4]==2 && array[6][4]==1 && (array[9][4]==1 || array[9][4]==3) && array[12][4]==3 &&
                    array[13][4]==2 && array[14][4]==0 && array[15][4]==3 && array[16][4]==2 && array[17][4]==3 && array[18][4]==3 &&
                    array[19][4]==1 && array[21][4]==2 && array[22][4]==0 && array[24][4]==2 && array[25][4]==0 && array[28][4]==3 &&
                    (array[29][4]==0 || array[29][4]==2) && array[30][4]==0 && array[32][4]==1 && array[33][4]==0){
        for(int i = 0; i<36; i++){
            if(i==0 || i==3 || i==5 || i==6 || i==9 || i==12 || i==13 || i==14 || i==15 || i==16 || i==17 || i==18 || i==19
                                                || i==21 || i==22 || i==24 || i==25 || i==28 || i==29 || i==30 || i==32 || i==33){
                array[i][2]=1;
            }
        }
        return true;
    }
    else{
        return false;
    }
}

void set_game_eleven(int array[][5]){
    array[0][0] = 1;
    array[0][1] = 1;
    array[0][2] = 0;
    array[0][3] = 1;
    array[0][4] = 1;

    array[1][0] = 1;
    array[1][1] = 2;
    array[1][2] = 0;
    array[1][3] = 3;
    array[1][4] = 0;

    array[2][0] = 1;
    array[2][1] = 3;
    array[2][2] = 0;
    array[2][3] = 4;
    array[2][4] = 3;

    array[3][0] = 1;
    array[3][1] = 4;
    array[3][2] = 0;
    array[3][3] = 2;
    array[3][4] = 2;

    array[4][0] = 1;
    array[4][1] = 5;
    array[4][2] = 0;
    array[4][3] = 4;
    array[4][4] = 1;

    array[5][0] = 1;
    array[5][1] = 6;
    array[5][2] = 0;
    array[5][3] = 4;
    array[5][4] = 1;

    array[6][0] = 1;
    array[6][1] = 7;
    array[6][2] = 0;
    array[6][3] = 4;
    array[6][4] = 1;

    array[7][0] = 1;
    array[7][1] = 8;
    array[7][2] = 0;
    array[7][3] = 4;
    array[7][4] = 0;

    array[8][0] = 1;
    array[8][1] = 9;
    array[8][2] = 0;
    array[8][3] = 5;
    array[8][4] = 0;

    array[9][0] = 2;
    array[9][1] = 1;
    array[9][2] = 0;
    array[9][3] = 3;
    array[9][4] = 0;

    array[10][0] = 2;
    array[10][1] = 2;
    array[10][2] = 0;
    array[10][3] = 4;
    array[10][4] = 3;

    array[11][0] = 2;
    array[11][1] = 3;
    array[11][2] = 0;
    array[11][3] = 4;
    array[11][4] = 1;

    array[12][0] = 2;
    array[12][1] = 4;
    array[12][2] = 0;
    array[12][3] = 4;
    array[12][4] = 3;

    array[13][0] = 2;
    array[13][1] = 5;
    array[13][2] = 0;
    array[13][3] = 5;
    array[13][4] = 1;

    array[14][0] = 2;
    array[14][1] = 6;
    array[14][2] = 0;
    array[14][3] = 4;
    array[14][4] = 1;

    array[15][0] = 2;
    array[15][1] = 7;
    array[15][2] = 0;
    array[15][3] = 4;
    array[15][4] = 3;

    array[16][0] = 2;
    array[16][1] = 8;
    array[16][2] = 0;
    array[16][3] = 5;
    array[16][4] = 1;

    array[17][0] = 2;
    array[17][1] = 9;
    array[17][2] = 0;
    array[17][3] = 2;
    array[17][4] = 3;

    array[18][0] = 3;
    array[18][1] = 1;
    array[18][2] = 0;
    array[18][3] = 4;
    array[18][4] = 0;

    array[19][0] = 3;
    array[19][1] = 2;
    array[19][2] = 0;
    array[19][3] = 4;
    array[19][4] = 3;

    array[20][0] = 3;
    array[20][1] = 3;
    array[20][2] = 0;
    array[20][3] = 4;
    array[20][4] = 1;

    array[21][0] = 3;
    array[21][1] = 4;
    array[21][2] = 0;
    array[21][3] = 4;
    array[21][4] = 3;

    array[22][0] = 3;
    array[22][1] = 5;
    array[22][2] = 0;
    array[22][3] = 4;
    array[22][4] = 0;

    array[23][0] = 3;
    array[23][1] = 6;
    array[23][2] = 0;
    array[23][3] = 3;
    array[23][4] = 0;

    array[24][0] = 3;
    array[24][1] = 7;
    array[24][2] = 0;
    array[24][3] = 4;
    array[24][4] = 3;

    array[25][0] = 3;
    array[25][1] = 8;
    array[25][2] = 0;
    array[25][3] = 4;
    array[25][4] = 0;

    array[26][0] = 3;
    array[26][1] = 9;
    array[26][2] = 0;
    array[26][3] = 3;
    array[26][4] = 0;

    array[27][0] = 4;
    array[27][1] = 1;
    array[27][2] = 0;
    array[27][3] = 4;
    array[27][4] = 0;

    array[28][0] = 4;
    array[28][1] = 2;
    array[28][2] = 0;
    array[28][3] = 4;
    array[28][4] = 3;

    array[29][0] = 4;
    array[29][1] = 3;
    array[29][2] = 0;
    array[29][3] = 3;
    array[29][4] = 1;

    array[30][0] = 4;
    array[30][1] = 4;
    array[30][2] = 0;
    array[30][3] = 4;
    array[30][4] = 3;

    array[31][0] = 4;
    array[31][1] = 5;
    array[31][2] = 0;
    array[31][3] = 5;
    array[31][4] = 1;

    array[32][0] = 4;
    array[32][1] = 6;
    array[32][2] = 0;
    array[32][3] = 2;
    array[32][4] = 1;

    array[33][0] = 4;
    array[33][1] = 7;
    array[33][2] = 0;
    array[33][3] = 4;
    array[33][4] = 3;

    array[34][0] = 4;
    array[34][1] = 8;
    array[34][2] = 0;
    array[34][3] = 4;
    array[34][4] = 0;

    array[35][0] = 4;
    array[35][1] = 9;
    array[35][2] = 0;
    array[35][3] = 3;
    array[35][4] = 0;
}

bool update_database_twelve(int array[][5], int input){
    //update the position for the input index
    if(input != 9 && input !=26 && input != 14) {
        array[input - 1][4]--;
        if (array[input - 1][4] == -1) {
            array[input - 1][4] = 3;
        }
    }

    //check if valid
    if(array[3][4]==2 && array[4][4]==3 && array[5][4]==2 && array[6][4]==2 && array[7][4]==1 && array[8][4]==2 &&
                    array[11][4]==2 && array[12][4]==0 && array[13][4]==0 &&
                    (array[14][4]==1 || array[14][4]==3) && (array[15][4]==1 || array[15][4]==3) && (array[16][4]==1 || array[16][4]==3) &&
                    array[18][4]==2 && array[19][4]==0 && array[21][4]==2 && array[22][4]==0 &&
                    (array[23][4]==1 || array[23][4]==3) && (array[24][4]==1 || array[24][4]==3) &&
                    array[25][4]==1 && array[26][4]==0 && array[29][4]==1 && array[30][4]==0 && array[32][4]==3 && array[33][4]==0){
        for(int i = 0; i<34; i++){
            if(i==3 || i==4 || i==5 || i==6 || i==7 || i==8 || i==11 || i==12 || i==13 || i==14 || i==15 || i==16 ||
                                i==18 || i==19 || i==21 || i==22 || i==23 || i==24 || i==25 || i==26 || i==29 || i==30 || i==32 || i==33){
                array[i][2]=1;
            }
        }
        return true;
    }
    else{
        return false;
    }
}

void set_game_twelve(int array[][5]){
    array[0][0] = 1;
    array[0][1] = 1;
    array[0][2] = 0;
    array[0][3] = 4;
    array[0][4] = 1;

    array[1][0] = 1;
    array[1][1] = 2;
    array[1][2] = 0;
    array[1][3] = 3;
    array[1][4] = 0;

    array[2][0] = 1;
    array[2][1] = 3;
    array[2][2] = 0;
    array[2][3] = 4;
    array[2][4] = 3;

    array[3][0] = 1;
    array[3][1] = 4;
    array[3][2] = 0;
    array[3][3] = 4;
    array[3][4] = 3;

    array[4][0] = 1;
    array[4][1] = 5;
    array[4][2] = 0;
    array[4][3] = 1;
    array[4][4] = 1;

    array[5][0] = 1;
    array[5][1] = 6;
    array[5][2] = 0;
    array[5][3] = 4;
    array[5][4] = 1;

    array[6][0] = 1;
    array[6][1] = 7;
    array[6][2] = 0;
    array[6][3] = 5;
    array[6][4] = 2;

    array[7][0] = 1;
    array[7][1] = 8;
    array[7][2] = 0;
    array[7][3] = 4;
    array[7][4] = 0;

    array[8][0] = 1;
    array[8][1] = 9;
    array[8][2] = 0;
    array[8][3] = 2;
    array[8][4] = 2;

    array[9][0] = 2;
    array[9][1] = 1;
    array[9][2] = 0;
    array[9][3] = 3;
    array[9][4] = 0;

    array[10][0] = 2;
    array[10][1] = 2;
    array[10][2] = 0;
    array[10][3] = 4;
    array[10][4] = 3;

    array[11][0] = 2;
    array[11][1] = 3;
    array[11][2] = 0;
    array[11][3] = 4;
    array[11][4] = 1;

    array[12][0] = 2;
    array[12][1] = 4;
    array[12][2] = 0;
    array[12][3] = 4;
    array[12][4] = 3;

    array[13][0] = 2;
    array[13][1] = 6;
    array[13][2] = 0;
    array[13][3] = 2;
    array[13][4] = 0;

    array[14][0] = 2;
    array[14][1] = 7;
    array[14][2] = 0;
    array[14][3] = 3;
    array[14][4] = 1;

    array[15][0] = 2;
    array[15][1] = 8;
    array[15][2] = 0;
    array[15][3] = 3;
    array[15][4] = 0;

    array[16][0] = 2;
    array[16][1] = 9;
    array[16][2] = 0;
    array[16][3] = 3;
    array[16][4] = 0;

    array[17][0] = 3;
    array[17][1] = 1;
    array[17][2] = 0;
    array[17][3] = 4;
    array[17][4] = 0;

    array[18][0] = 3;
    array[18][1] = 2;
    array[18][2] = 0;
    array[18][3] = 4;
    array[18][4] = 3;

    array[19][0] = 3;
    array[19][1] = 3;
    array[19][2] = 0;
    array[19][3] = 4;
    array[19][4] = 1;

    array[20][0] = 3;
    array[20][1] = 4;
    array[20][2] = 0;
    array[20][3] = 4;
    array[20][4] = 3;

    array[21][0] = 3;
    array[21][1] = 6;
    array[21][2] = 0;
    array[21][3] = 4;
    array[21][4] = 0;

    array[22][0] = 3;
    array[22][1] = 7;
    array[22][2] = 0;
    array[22][3] = 4;
    array[22][4] = 3;

    array[23][0] = 3;
    array[23][1] = 8;
    array[23][2] = 0;
    array[23][3] = 3;
    array[23][4] = 0;

    array[24][0] = 3;
    array[24][1] = 9;
    array[24][2] = 0;
    array[24][3] = 3;
    array[24][4] = 0;

    array[25][0] = 4;
    array[25][1] = 1;
    array[25][2] = 0;
    array[25][3] = 2;
    array[25][4] = 1;

    array[26][0] = 4;
    array[26][1] = 2;
    array[26][2] = 0;
    array[26][3] = 4;
    array[26][4] = 3;

    array[27][0] = 4;
    array[27][1] = 3;
    array[27][2] = 0;
    array[27][3] = 3;
    array[27][4] = 1;

    array[28][0] = 4;
    array[28][1] = 4;
    array[28][2] = 0;
    array[28][3] = 4;
    array[28][4] = 3;

    array[29][0] = 4;
    array[29][1] = 5;
    array[29][2] = 0;
    array[29][3] = 1;
    array[29][4] = 3;

    array[30][0] = 4;
    array[30][1] = 6;
    array[30][2] = 0;
    array[30][3] = 4;
    array[30][4] = 0;

    array[31][0] = 4;
    array[31][1] = 7;
    array[31][2] = 0;
    array[31][3] = 4;
    array[31][4] = 3;

    array[32][0] = 4;
    array[32][1] = 8;
    array[32][2] = 0;
    array[32][3] = 4;
    array[32][4] = 0;

    array[33][0] = 4;
    array[33][1] = 9;
    array[33][2] = 0;
    array[33][3] = 4;
    array[33][4] = 2;
}
void config_GIC(void)
{

    int address;	// used to calculate register addresses
 
    *((int *)0xFFFED8C4) = 0x01000000;
    *((int *)0xFFFED118) = 0x00000080;
    /* configure the FPGA interval timer and KEYs interrupts */
    *((int *)0xFFFED848) = 0x00000101;
    *((int *)0xFFFED108) = 0x00000300;

    /* enable some examples of interrupts */
     config_interrupt (PS2_IRQ, CPU0);
    // Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts for lowest priority
    address = MPCORE_GIC_CPUIF + ICCPMR;
    *((int *) address) = 0xFFFF;

    // Set CPU Interface Control Register (ICCICR). Enable signaling of interrupts
    address = MPCORE_GIC_CPUIF + ICCICR;
    *((int *) address) = ENABLE;

    // Configure the Distributor Control Register (ICDDCR) to send pending interrupts to CPUs
    address = MPCORE_GIC_DIST + ICDDCR;
    *((int *) address) = ENABLE;
}

void PS2_ISR(void) {
    volatile int * PS2_ptr = (int *) 0xFF200100;		// PS/2 port address
    int PS2_data, RAVAIL;

    PS2_data = *(PS2_ptr);									// read the Data register in the PS/2 port
    RAVAIL = (PS2_data & 0xFFFF0000) >> 16;			// extract the RAVAIL field
    if (RAVAIL > 0)
    {
        /* always save the last three bytes received */
       // byte1 = byte2;
        //byte2 = byte3;
        //byte3 = PS2_data & 0xFF;
        if((byte2 == (char) 0x16) || (byte3 == (char)0x16)){
         
     
        }
        else if((byte2 == (char) 0x1E) || (byte3 == (char)0x1E)){
          
          
        }
        else if((byte2 == (char) 0x26) || (byte3 == (char)0x26)){
           
        }
        else if((byte2 == (char) 0x25) || (byte3 == (char)0x25)){
         
        
        }
        else if((byte2 == (char) 0x2E) || (byte3 == (char)0x2E)){
       
        }
        else if((byte2 == (char) 0x36) || (byte3 == (char)0x36)){
      
        }
        else if((byte2 == (char) 0x3D) || (byte3 == (char)0x3D)){
           
        }
        else if((byte2 == (char) 0x3E) || (byte3 == (char)0x3E)){
          
        }
        else if((byte2 == (char) 0x46) || (byte3 == (char)0x46)){
       
        }
        else if((byte2 == (char) 0x1C) || (byte3 == (char)0x1C)){
          
        }
        else if((byte2 == (char) 0x32) || (byte3 == (char)0x32)){
         
        }
        else if((byte2 == (char) 0x21) || (byte3 == (char)0x21)){
             
        }
        else if((byte2 == (char) 0x15) || (byte3 == (char)0x15)){
            
        }
        else if((byte2 == (char) 0x33) || (byte3 == (char)0x33)){
      
        }
        else if((byte2 == (char) 0x2D) || (byte3 == (char)0x2D)){
          
        }
    }
  
}


void config_interrupt (int int_ID, int CPU_target)
{
    int n, addr_offset, value, address;
    /* Set Interrupt Processor Targets Register (ICDIPTRn) to cpu0.
     * n = integer_div(int_ID / 4) * 4
     * addr_offet = #ICDIPTR + n
     * value = CPU_target << ((int_ID & 0x3) * 8)
     */
    n = (int_ID >> 2) << 2;
    addr_offset = ICDIPTR + n;
    value = CPU_target << ((int_ID & 0x3) << 3);

    /* Now that we know the register address and value, we need to set the correct bits in
     * the GIC register, without changing the other bits */
    address = MPCORE_GIC_DIST + addr_offset;
    hw_write_bits((int *) address, 0xff << ((int_ID & 0x3) << 3), value);

    /* Set Interrupt Set-Enable Registers (ICDISERn).
     * n = (integer_div(in_ID / 32) * 4
     * addr_offset = 0x100 + n
     * value = enable << (int_ID & 0x1F) */
    n = (int_ID >> 5) << 2;
    addr_offset = ICDISER + n;
    value = 0x1 << (int_ID & 0x1f);
    /* Now that we know the register address and value, we need to set the correct bits in
     * the GIC register, without changing the other bits */
    address = MPCORE_GIC_DIST + addr_offset;
    hw_write_bits((int *) address, 0x1 << (int_ID & 0x1f), value);
}

void hw_write_bits(volatile int * addr, volatile int unmask, volatile int value)
{
    *addr = ((~unmask) & *addr) | value;
}
void __attribute__ ((interrupt)) __cs3_isr_irq (void)
{
    // Read the ICCIAR from the processor interface
    int address = MPCORE_GIC_CPUIF + ICCIAR;
    int int_ID = *((int *) address);

    if (int_ID == PS2_IRQ){				// check if interrupt is from the PS/2
       PS2_ISR();
    }
    else
        while (1);									// if unexpected, then halt

    // Write to the End of Interrupt Register (ICCEOIR)
    address = MPCORE_GIC_CPUIF + ICCEOIR;
    *((int *) address) = int_ID;

    return;
}

// Define the remaining exception handlers
void __attribute__((interrupt)) __cs3_reset(void) {
    while (1);
}

void __attribute__((interrupt)) __cs3_isr_undef(void) {
    while (1);
}

void __attribute__((interrupt)) __cs3_isr_swi(void) {
    while (1);
}

void __attribute__((interrupt)) __cs3_isr_pabort(void) {
    while (1);
}

void __attribute__((interrupt)) __cs3_isr_dabort(void) {
    while (1);
}

void __attribute__((interrupt)) __cs3_isr_fiq(void) {
    while (1);
}
void VGA_text_clean() {
 
    for(int y=0;y<60;y++){
        for(int x=0;x<80;x++){
            int offset = (y << 7) + x;
            *(character_buffer + offset) = 0;
            ++offset;
        }
    }
}
void wait_for_vsync() { 
    volatile int* pixelCtrlPtr = (int*) 0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    register int status; 

    *pixelCtrlPtr = 1; 
    status = *(pixelCtrlPtr + 3); 
    while((status & 0x01) != 0) {
        status = *(pixelCtrlPtr + 3); 
    }
}

void VGA_text(int x, int y, char *text_ptr) {
 
    int offset = (y << 7) + x;

    while (*(text_ptr)) 
    {
     
        *(character_buffer + offset) = *(text_ptr);
        ++text_ptr;
        ++offset;
    }
}

void set_A9_IRQ_stack(void)
{
    int stack, mode;
    stack = A9_ONCHIP_END - 7;		// top of A9 onchip memory, aligned to 8 bytes
    /* change processor to IRQ mode with interrupts disabled */
    mode = INT_DISABLE | IRQ_MODE;
    asm("msr cpsr, %[ps]" : : [ps] "r" (mode));
    /* set banked stack pointer */
    asm("mov sp, %[ps]" : : [ps] "r" (stack));

    /* go back to SVC mode before executing subroutine return! */
    mode = INT_DISABLE | SVC_MODE;
    asm("msr cpsr, %[ps]" : : [ps] "r" (mode));
}

/*
 * Turn on interrupts in the ARM processor
*/
void enable_A9_interrupts(void)
{
    int status = SVC_MODE | INT_ENABLE;
    asm("msr cpsr,%[ps]" : : [ps]"r"(status));
}
void disable_A9_interrupts(void) {
    int status = 0b11010011;
    asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}

int wait_for_key() {
    int PS2_data, RVALID, current_key;

    while (1) {
        PS2_data = *(PS2_ptr);	// read the Data register in the PS/2 port
        RVALID = (PS2_data & 0x8000);	// extract the RVALID field
        if (RVALID != 0)
        {

            byte1 = byte2;
            byte2 = byte3;
            byte3 = PS2_data & 0xFF;
        }
        current_key = (byte3 << 16) | ((byte2 < 8)) | byte3;
        return current_key;
    }
}

char check_diffculty() {
    

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
        volatile int * pixel_ctrl_ptr2 = (int *) 0xFF203020;
        
         for(int i=0; i<N; i++){
			draw_water_one(initial_x_position_left[i],initial_y_position_left[i]);
			draw_water_one(initial_x_position_right[i],initial_y_position_right[i]);
		}
		wait_screen(pixel_ctrl_ptr2);
		wait_screen(pixel_ctrl_ptr2);
		
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
		    
        int key = wait_for_key();
        if (key == 0x00160016) {
        
        byte1=0;
        byte2=0;
        byte3=0;
    

	    return '1';
        
        } 
        else if (key == 0x001E001E) {
        byte1=0;
        byte2=0;
        byte3=0;
        
      
		
		
	    return '2';
        } else if (key == 0x00260026) {
        byte1=0;
        byte2=0;
        byte3=0;
       
      
      
	    return '3';
	  
	    
        }
        else if ( key == 0x00250025) {
        byte1=0;
        byte2=0;
        byte3=0;  
         
        
     
	    return '4';
	    
        } 
         else if ( key == 0x002E002E) {
        byte1=0;
        byte2=0;
        byte3=0;  
       
		 
	
		
	    return '5';
	   
        } 
        else if ( key == 0x00360036) {
        byte1=0;
        byte2=0;
        byte3=0;  
     
		 
		
	    return '6';
	     
        } 
        else if ( key == 0x003D003D) {
         byte1=0;
        byte2=0;
        byte3=0; 
      
      
		  
		
	    return '7';
	    
        } 
        else if ( key == 0x003E003E) {
         byte1=0;
        byte2=0;
        byte3=0;  
      
		   
	    return '8';
	     
        } 
        else if ( key == 0x00460046) {
            
        byte1=0;
        byte2=0;
        byte3=0;  
    
		  
		
	    return '9';
        }  
        else if ( key == 0x001C001C) {
            
        byte1=0;
        byte2=0;
        byte3=0;   
	    return 'a';
	   
        } 
        else if ( key == 0x00320032) {
        byte1=0;
        byte2=0;
        byte3=0; 
	    return 'b';
	     
        } 
        else if ( key == 0x00210021) {
            byte1=0;
        byte2=0;
        byte3=0;
	    return 'c';
	    
	    
        } 
        else if ( key == 0x00150015) {
            
	    byte1=0;
        byte2=0;
        byte3=0;
        return 'q';
        } 
       else if ( key == 0x00330033) {
            
	 
	       byte1=0;
           byte2=0;
           byte3=0;
        return 'h';
        } 
        else if(key ==0x002D002D ){
            byte1=0;
            byte2=0;
            byte3=0;
        return 'r';
        }
           else { // the key is invalid
            key = wait_for_key(); // repeat again
           
            continue;
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

void wait_screen(volatile int* pixel_ctrl_ptr){
    *pixel_ctrl_ptr = 1;
    register int status;
	status = *(pixel_ctrl_ptr + 3);
    while((status & 0x01) == 1){
        status = *(pixel_ctrl_ptr + 3);
    }
}

char check_enter() {
    
    while (1) {
        int key = wait_for_key();
        if (key == 0x00160016) {
        
        byte1=0;
        byte2=0;
        byte3=0;
        
	    return '1';
        
        } 
        else if (key == 0x001E001E) {
        byte1=0;
        byte2=0;
        byte3=0;
 	    return '2';
	    
        } else if (key == 0x00260026) {
        byte1=0;
        byte2=0;
        byte3=0;
      
	    return '3';
	  
	    
        } else if ( key == 0x00250025) {
        byte1=0;
        byte2=0;
        byte3=0;    
	    return '4';
	    
        } 
         else if ( key == 0x002E002E) {
         byte1=0;
        byte2=0;
        byte3=0;   
	    return '5';
	   
        } 
        else if ( key == 0x00360036) {
        byte1=0;
        byte2=0;
        byte3=0;   
	    return '6';
	     
        } 
        else if ( key == 0x003D003D) {
         byte1=0;
        byte2=0;
        byte3=0;   
	    return '7';
	    
        } 
        else if ( key == 0x003E003E) {
         byte1=0;
        byte2=0;
        byte3=0;   
	    return '8';
	     
        } 
        else if ( key == 0x00460046) {
            
        byte1=0;
        byte2=0;
        byte3=0;   
	    return '9';
	    
        }  
        else if ( key == 0x001C001C) {
            
        byte1=0;
        byte2=0;
        byte3=0;   
	    return 'a';
	   
        } 
        else if ( key == 0x00320032) {
        byte1=0;
        byte2=0;
        byte3=0; 
	    return 'b';
	     
        } 
        else if ( key == 0x00210021) {
            byte1=0;
        byte2=0;
        byte3=0;
	    return 'c';
	    
	    
        } 
        else if ( key == 0x00150015) {
            

	    
	    byte1=0;
        byte2=0;
        byte3=0;
        return 'q';
        } 
       else if ( key == 0x00330033) {
            
	 
	     byte1=0;
        byte2=0;
        byte3=0;
        return 'h';
        } 
        else if(key ==0x002D002D ){
            byte1=0;
            byte2=0;
            byte3=0;
        return 'r';
        }
           else { // the key is invalid
            key = wait_for_key(); // repeat again
            continue;
        }
    }
}	









