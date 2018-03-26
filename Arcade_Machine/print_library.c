#include <avr/io.h>
#include <stdio.h>

#include "ascii_font.h"
#include "print_library.h"


/*
  Matrix	Teensy	Explained
  -------------------------
  R1		B0
  G1		B1
  B1		B2
  R2		B3
  G2		B4
  B2		B5

  A		D0		Row selects
  B		D1
  C		D2

  CLK		D3		Clock
  LAT		C6		Latch
  OE		C7		Output Enabled
*/

/************ INITIALISATION ************/

char colourToBin (char colour){
  switch(colour){
    case 'r': // Red
      return colourSpectrum[0];
      break;
    case 'g': // Green
      return colourSpectrum[1];
      break;
    case 'b': // Blue
      return colourSpectrum[2];
      break;
    case 'y': // Yellow
      return colourSpectrum[3];
      break;
    case 'm': // Magenta
      return colourSpectrum[4];
      break;
    case 'c': // Cyan
      return colourSpectrum[5];
      break;
    case 'w': // White
      return colourSpectrum[6];
      break;
    case 'k': // Black
      return colourSpectrum[7];
      break;
    default:  // Blank/black
      return colourSpectrum[7];
      break;
  }
}

void OE(){
  OE_REGISTER |= (1<<OE_BIT);
  OE_REGISTER &= ~(1<<OE_BIT);
}

void LAT(){
	LAT_REGISTER |= (1<<LAT_BIT);
	LAT_REGISTER &= ~(1<<LAT_BIT);
}

void CLK(){
	CLK_REGISTER |= (1<<CLK_BIT);
	CLK_REGISTER &= ~(1<<CLK_BIT);
}

void ROW(char xCBA){
	ROW_REGISTER = xCBA;
}

void Init_Screen(Matrix_Buff buffer){
  Init_PrintPins();
  Init_MatrixBuffer(buffer);
}

void Init_PrintPins(){
  RGB_REG_INIT |= (1<<R1_BIT | 1<<G1_BIT | 1<<B1_BIT | 1<<R2_BIT | 1<<G2_BIT | 1<<B2_BIT);
  ROW_REG_INIT |= (1<<A_BIT | 1<<B_BIT | 1<<G_BIT);
  OE_REG_INIT |= 1<<OE_BIT;
  LAT_REG_INIT |= 1<<LAT_BIT;
}

void Init_MatrixBuffer(Matrix_Buff buffer){
	for(int row = 0; row < ROW_HEIGHT; row++){
		for(int col = 0; col < SCREEN_HEIGHT; col++){
			buffer.pixels[row][col] = ((colourToBin('k')<<RGB1)|colourToBin('k'));
		}
	}
  buffer.solidColour = 'x';
}

/* FUNCTIONS - Matrix buffer dependent **SLOW** */

void Matrix_ClearBuffer(Matrix_Buff buffer){
	for(int row = 0; row < ROW_HEIGHT; row++){
		for(int col = 0; col < SCREEN_WIDTH; col++){
			buffer.pixels[row][col] = 0; // char array = null
		}
	}
}

void Matrix_SetPixel(uint8_t x, uint8_t y, unsigned char colour){
	if(x > SCREEN_WIDTH-1 || y > SCREEN_HEIGHT-1){ // OUT OF BOUNDS
		return;
	}

	uint8_t line = y%ROW_HEIGHT;
	unsigned char tmpColour = colourToBin(colour);

	if(y < ROW_HEIGHT) {
		buffer.pixels[line][x] = ((buffer.pixels[line][x] & ~(0b111))|(tmpColour<<RGB0));
	} else {
		buffer.pixels[line][x] = ((buffer.pixels[line][x] & ~(0b111000))|(tmpColour<<RGB1));
	}
}

void Matrix_SetCharacter(int8_t x, uint8_t y, char letter, unsigned char colour){
	if(x > SCREEN_WIDTH-1 || x < -7){ // TODO Is -7 arbitrary?
		return;
	}

	// loop through each pixel in the character array and plot each one individually
	for (unsigned char i = 0; i<FONT_WIDTH; i++) {
		for (unsigned char j = 0; j<FONT_HEIGHT; j++) {
			if((pgm_read_byte(&(ASCII[letter - 0x20][i])) & (1 << j)) >> j){
				Matrix_setPixel(x+i, y+j, colour);
			} else {
				Matrix_setPixel(x+i, y+j, 'k');
			}
		}
	}
	for (unsigned char j = 0; j<FONT_HEIGHT; j++){
		Matrix_setPixel(x+FONT_WIDTH, y+j, 'k');
	}
}

void Matrix_DrawString(int8_t x, uint8_t y, char *string, unsigned char colour){
	unsigned char i = 0;
  int CHAR_WIDTH = FONT_WIDTH+1; // Include spacing between letters
  char colours[] = {'r', 'y', 'g', 'c', 'b', 'm'};

	// Draw each character until the null terminator is reached
  if (colour = 'j'){ // Special tacky Mode: RAINBOW LETTERING
    int index = 0;
    while (*string != 0) {
      index = index % CHAR_WIDTH;
  		Matrix_setCharacter(x+i*CHAR_WIDTH, y, *(string), colours[index]);
  		string++;
  		i++;
      index++;
  	}
  } else {
    while (*string != 0) {
      Matrix_setCharacter(x+i*CHAR_WIDTH, y, *(string), colour);
      string++;
      i++;
    }
  }
}

void Matrix_PrintBuffer(Matrix_Buff buffer){
  for(int row = 0; row < ROW_HEIGHT; row++){
		for(int col = 0; col < SCREEN_WIDTH; col++){
			RGB_REGISTER = buffer.pixels[row][col];
			CLK();
		}
		LAT();
		ROW(row);
		OE();
  	}
}

/* FUNCTIONS - No memory access **FAST** */

void Screen_MonoColour(unsigned char colour){
  char tmp = colourToBin(colour);
  RGB_REGISTER = ((tmp<<RGB1)|(tmp<<RGB0));
  for(int row = 0; row < ROW_HEIGHT; row++){
    for(int col = 0; col < SCREEN_WIDTH; col++){
      CLK();
    }
    LAT();
    ROW(row);
    OE();
  }
}

void Screen_Clear(){
  MonoScreen('k');
}
