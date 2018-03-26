#ifndef __PRINT_LIB__
#define __PRINT_LIB__

#define SCREEN_WIDTH (32)
#define SCREEN_HEIGHT (16)
#define ROW_HEIGHT (8)

#define RGB0 (0) // No offset     [ROW 1]
#define RGB1 (3) // 3 dist offset [ROW 2]

/***** PIN OUTPUT DEFINITIONS *****/
#define OE_REG_INIT DDRC
#define OE_REGISTER PORTC
#define OE_BIT (7)

#define LAT_REG_INIT DDRC
#define LAT_REGISTER PORTC
#define LAT_BIT (6)

#define CLK_REGISTER PORTD
#define CLK_REG_INIT DDRD
#define CLK_BIT (3)

#define ROW_REGISTER PORTD
#define ROW_REG_INIT DDRD
#define A_BIT (0)
#define B_BIT (1)
#define C_BIT (2)

#define RGB_REGISTER PORTB
#define RGB_REG_INIT DDRB
#define R1_BIT (0)
#define G1_BIT (1)
#define B1_BIT (2)
#define R2_BIT (3)
#define G2_BIT (4)
#define B2_BIT (5)

// Matrix Buffer struct definition
typedef struct Matrix_Buff{
  char pixels[ROW_HEIGHT][SCREEN_WIDTH];
  char solidColour;
} Matrix_Buff;

/* TO CALL COLOURS BINARY CODE DIRECTLY
 * Red, Yellow, Green, Cyan, Blue, Magenta, White, Black
 */
char[] colourSpectrum = {0b00000001, 0b00000011, 0b00000010, 0b00000110, 0b00000100, 0b00000101, 0b00000111, 0b00000000};


void ROW(char xCBA);

/*  colourToBin
 *  Takes the input character, char colour,
 *  refers to look up table [Switch Case] to find 3-bit colour in
 *  3 LSB
 *
 *  COLOUR CONVERSION
 *	r	Red            0b00000001      | 0x01
 *	g	Green          0b00000010      | 0x02
 *	b	Blue           0b00000100      | 0x04
 *	m	Magenta        0b00000101      | 0x05
 *	c	Cyan           0b00000110      | 0x06
 *	y	Yellow         0b00000011      | 0x03
 *	k	Black          0b00000000      | 0x00
 *	w	White          0b00000111      | 0x07
 *
 *  returns uint8 with only last three bits affected
 */
char colourToBin (char colour);

void OE();

void LAT();

void CLK();

void ROW(char xCBA);

void Init_PrintPins();

/* FUNCTIONS - Matrix buffer dependent **SLOW** */
void Matrix_InitBuffer(Matrix_Buff buffer);

void Matrix_ClearBuffer(Matrix_Buff buffer);

void Matrix_SetPixel(uint8_t x, uint8_t y, unsigned char colour);

void Matrix_SetCharacter(int8_t x, uint8_t y, char letter, unsigned char colour);

void Matrix_DrawString(int8_t x, uint8_t y, char *string, unsigned char colour);

void Matrix_PrintBuffer(Matrix_Buff buffer);

/* FUNCTIONS - No memory access **FAST** */
void MonoScreen(unsigned char colour);

void ClearScreen();


#endif //__PRINT_LIB__
