#ifndef __REACT_LIB__
#define __REACT_LIB__

/***** PIN INPUT DEFINITIONS *****/
#define BUTTON_REG_INT DDRF
#define BUTTON_REGISTER PINF
#define RED_BIT (4)
#define GREEN_BIT (5)
#define BLUE_BIT (6)

/***** STRUCT CARRYING BUTTON COUNTS *****/
typedef struct Buttons{
  uint8_t red;
  uint8_t blue;
  uint8_t green;
} Buttons;

/***** FUNCTIONS *****/

void Init_Buttons();

#endif //__REACT_LIB__
