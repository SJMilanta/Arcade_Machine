#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "cpu_speed.h"
#include "ascii_font.h"
#include "print_library.h"
#include "reaction_timer_library.h"

// Function Declarations
void init_hardware();
void init_timer();
void getscore(void);
void winCondition(char colour);

// Globals
Matrix_Buff Matrix_Buffer;

volatile Buttons button;

volatile int16_t global_x;
static char *ReactionTester = "Reaction Tester";
static char *HowFast = "How fast are you?";

volatile char game;
volatile int16_t time;
volatile int16_t gameTime_start;
volatile int16_t gameTime_fin;

volatile char tempColour;

// Main
int main() {
  set_clock_speed(CPU_8MHz);
	game = 0;
	global_x = 0;

    // Setup the hardware
    init_hardware();

    while (1) {
		Matrix_Update(Matrix_Buffer);

    tempColour = TCNT0 % 3;

  		if(global_x < -200){
  			global_x = 32;
  		}
    }

    return 0;
}

void init_hardware(void) {
  Init_Screen(Matrix_Buffer);
  Init_Buttons();
  init_timer();

  DDRF |= 1<<0;   // TODO What are these for?
  PORTF &= ~(1<<0);
}

void init_timer(){
	// Timer 1 to update display -- 16 bit
	// set to normal mode
	TCCR1A &= ~(1<<0|1<<1);
	TCCR1B &= ~(1<<3);
	// prescaler
	TCCR1B |= (1<<1);
	TCCR1B &= ~(1<<2|1<<0);
	// enable interrupt
	TIMSK1 |= (1 << 0);

	// Timer 0 to poll button press -- 8 bit
	// set to normal mode
	TCCR0B &= ~((1<<WGM02));

	//Set Prescaler using TCCR0B, using Clock Speed find timer speed = 1/(Clock Speed/Prescaler)
	//Prescaler = 1024
	//Timer Speed = 128 micro seconds
	//Timer Overflow Speed = 32678 micro seconds (Timer Speed * 256) - (256 since 8-bit timer)
	TCCR0B |= (1<<1);
	TCCR0B &= ~((1<<2)|(1<<0));
	// Enable the interrupt and that the global interrupts are enabled.
	TIMSK0 |= (1 << 0);

	// enable global interupts
	sei();
}

ISR(TIMER0_OVF_vect){
	button.red = button.red<<1;
	button.green = button.green<<1;
	button.blue = button.blue<<1;
	if(((BUTTON_REGISTER>>RED_BIT) & 0b1) == 1){
		button.red |= 0b1;
	}
	if(((BUTTON_REGISTER>>GREEN_BIT) & 0b1) == 1){
		button.green |= 0b1;
	}
	if(((BUTTON_REGISTER>>BLUE_BIT) & 0b1) == 1){
		button.blue |= 0b1;
	}
	time++;
}

ISR(TIMER1_OVF_vect){
	if (game == 0) {
		global_x--;
		Matrix_Clear();
		Matrix_drawString(global_x, 0, ReactionTester, 'w');
		Matrix_drawString(global_x + 92, 8, HowFast, 'w');
		if(button.red == 255 || button.green == 255 || button.blue == 255){
			game++;
		}
	}	else {
		// GAME MODE
		if (game == 1) {
			Matrix_Clear();
			Matrix_setCharacter(((SCREEN_WIDTH/2)-4),4,'3','m');
			game++;
		} else if(game == 10) {
			Matrix_setCharacter(((SCREEN_WIDTH/2)-4),4,'2','m');
			game++;
		} else if(game == 20) {
			Matrix_setCharacter(((SCREEN_WIDTH/2)-4),4,'1','m');
			game++;
		}else if(game <30){                //??????????
			game++;
		}else if(game > 30 && game <100){  //??????????
			game++;
		}else if (game > 80){              //??????????
			game = 0;
		} else { // When does this get accessed???
			if (Matrix_Buffer.solidColour == 'x' || Matrix_Buffer.solidColour == 'm') {
				Matrix_Clear();
				switch (tempColour){
					case 0:
						Matrix_Buffer.solidColour = 'r';
						gameTime_start = time;
						break;
					case 1:
						Matrix_Buffer.solidColour = 'g';
						gameTime_start = time;
						break;
					case 2:
						Matrix_Buffer.solidColour = 'b';
						gameTime_start = time;
						break;
					default:
						Matrix_Buffer.solidColour = 'x';
				}
			}
			if(button.red == 255){winCondition('r');}
			if(button.green == 255){winCondition('g');}
			if(button.blue == 255){winCondition('b');}
		}
	}
}

/***** FUNCTIONS - GAME LOGIC *****/
void getscore(){
	uint16_t score = (gameTime_fin-gameTime_start);

	char str[6];
	sprintf(str,"%d",score);

	Matrix_drawString(0, 0, "Score", 'y');
	Matrix_drawString(0, 8, str, 'y');
}

void winCondition(char colour){
  if(Matrix_Buffer.solidColour == colour){
    Matrix_drawString(0, 0, "WIN!", colour);
    gameTime_fin = time;
    getscore();
  }else{
    Matrix_drawString(0, 0, "miss", colour);
    gameTime_fin = gameTime_start + 4500;
    getscore();
  }
  game = 31;
  Matrix_Buffer.solidColour = 'x';
}
