/*==============================================================================
    Project: RAINBO-Starter
    Version: 1.0				Date: October 18, 2017
    Target: RAINBO				Processor: PIC12F1840

 RAINBOb NeoPixel driver circuit with IR control.
 =============================================================================*/

#include    "xc.h"              // XC compiler general include file

#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include	"RAINBO.h"			// Include user-created constants and functions
// Byte variables for neopixel functions

unsigned char red = 254; // RGB colour bytes and default starting colour
unsigned char green =254;
unsigned char blue = 254;

unsigned char letsSaveTime, letsSaveMoreTime;

unsigned char tempRed; // Temporary RGB colour bytes
unsigned char tempGreen;
unsigned char tempBlue;
unsigned char colors[3][2] = {{70,0},{0,0},{0,70}};
//unsigned char colors[3][30] = {
//    {70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0},
//    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//    {0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70, 0, 70}
//};
//unsigned char colors[3][15] = {
//    { 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70},
//    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//    {70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5, 0}
//};
char redDir = 0;
char greenDir = -1;
char blueDir = 1;

unsigned char offset = 0;

char curCol = 'r';
const char maxLEDs = 30; // Number of LEDs in the string

unsigned char temp; // Temporary byte storage
unsigned char i; // Generic index counter

unsigned char adConvert(unsigned char chan) {
    ADON = 1; // Turn A-D converter on
    ADCON0 = (ADCON0 & 0b10000011); // Change the conversion channel by wiping
    ADCON0 = (ADCON0 | chan); // CHS bits and ORing with selected channel
    __delay_us(2); // Give input time to settle
    GO = 1; // Start the conversion

    while (GO); // Wait for the conversion to finish (GO=0)

    ADON = 0; // Turn A-D converter off and
    return (ADRESH); // return the upper 8-bits (MSB) of the result
}

/*==============================================================================
 *	neoRGB - sets requested number of Neopixel LEDs to same RGB value.
 =============================================================================*/

void changeDir() {
    red += redDir;
    green += greenDir;
    blue += blueDir;
    switch (curCol) {
        case 'r':
            if (red == 0) {
                curCol = 'g';
                redDir = 0;
                greenDir = -1;
                blueDir = 1;
            }
            break;
        case 'g':
            if (green == 0) {
                curCol = 'b';
                redDir = 1;
                greenDir = 0;
                blueDir = -1;
            }
            break;
        case 'b':
            if (blue == 0) {
                curCol = 'r';
                redDir = -1;
                greenDir = 1;
                blueDir = 0;
            }
            break;
    }
}

void diffCols(unsigned char leds, unsigned char offset) {
    for (leds; leds != 0; leds--) // Repeat all 24 colour bits for each LED
    {
        letsSaveTime = leds+offset;
        letsSaveTime = letsSaveTime%2;
        temp = colors[1][letsSaveTime]; // Copy green byte, prepare to shift MSB first
        for (i = 8; i != 0; i--) // PWM each bit in assembly code for speed
        {
            asm("bsf LATA,4"); // Make N2 output high
            asm("nop"); // Wait one instruction cycle
            asm("btfss _temp,7"); // If MSB == 1, skip next instruction
            asm("bcf LATA,4"); // Make N2 output low (0)
            asm("nop"); // Pad by one instruction cycle for timing
            asm("lslf _temp,f"); // Shift next bit into MSB position
            asm("bcf LATA,4"); // Make N2 output low (1)
        }
        temp = colors[0][letsSaveTime]; // Red next, same as green.
        for (i = 8; i != 0; i--) {
            asm("bsf LATA,4");
            asm("nop");
            asm("btfss _temp,7");
            asm("bcf LATA,4");
            asm("nop");
            asm("lslf _temp,f");
            asm("bcf LATA,4");
        }
        temp = colors[2][letsSaveTime]; // Blue last. 
        for (i = 8; i != 0; i--) {
            asm("bsf LATA,4");
            asm("nop");
            asm("btfss _temp,7");
            asm("bcf LATA,4");
            asm("nop");
            asm("lslf _temp,f");
            asm("bcf LATA,4");
        }

    }
}

void neoRGB(unsigned char red, unsigned char green, unsigned char blue, unsigned char leds) {
    for (leds; leds != 0; leds--) // Repeat all 24 colour bits for each LED
    {
        temp = green; // Copy green byte, prepare to shift MSB first
        for (i = 8; i != 0; i--) // PWM each bit in assembly code for speed
        {
            asm("bsf LATA,4"); // Make N2 output high
            asm("nop"); // Wait one instruction cycle
            asm("btfss _temp,7"); // If MSB == 1, skip next instruction
            asm("bcf LATA,4"); // Make N2 output low (0)
            asm("nop"); // Pad by one instruction cycle for timing
            asm("lslf _temp,f"); // Shift next bit into MSB position
            asm("bcf LATA,4"); // Make N2 output low (1)
        }
        temp = red; // Red next, same as green.
        for (i = 8; i != 0; i--) {
            asm("bsf LATA,4");
            asm("nop");
            asm("btfss _temp,7");
            asm("bcf LATA,4");
            asm("nop");
            asm("lslf _temp,f");
            asm("bcf LATA,4");
        }
        temp = blue; // Blue last. 
        for (i = 8; i != 0; i--) {
            asm("bsf LATA,4");
            asm("nop");
            asm("btfss _temp,7");
            asm("bcf LATA,4");
            asm("nop");
            asm("lslf _temp,f");
            asm("bcf LATA,4");
        }

    }
}

int main(void) // Start of program
{
    init(); // Initialize oscillator and I/O Ports
    __delay_us(200);

    while (1) {
        //neoRGB(red, green, blue, maxLEDs);
        //changeDir();
        diffCols(maxLEDs, offset);
        offset = (offset == 1) ? 0 : 1;
        __delay_ms(200);
    }
}
