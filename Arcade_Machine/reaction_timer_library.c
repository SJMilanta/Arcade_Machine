#include <stdio.h>

#include "reaction_timer_library.h"


void Init_Buttons(Buttons button){
  button.red = 0;
  button.green = 0;
  button.blue = 0;
	BUTTON_REG_INIT &= ~(0b1<<RED_BIT | 0b1<<BLUE_BIT | 0b1<<GREEN_BIT); // Set as input
}
