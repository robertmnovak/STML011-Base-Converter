#include "LCD_SPI.h"
#include "font.h"
uint32_t fill_screen = 0;
uint16_t current_color = 0;
struct LCD{
    uint16_t X;
    uint16_t Y;
    uint16_t HEIGHT;
    uint16_t WIDTH;
}LCD;

/********************************************************************************
LCD Delay function

Simple function to cause some short delays during the initialization of the LCD
********************************************************************************/
void LCD_Delay(int x){
	while(x > 0){
		x--;
	}
}

/********************************************************************************
Move Cursor Function

Sets the current position to our LCD struct. This position is used when writing
information on the screen. Such as needing to choose where you want to start
writing your text.
********************************************************************************/
void moveCursor(uint8_t x , uint8_t y){
    if(x < LCD.WIDTH && y < LCD.HEIGHT ){
       LCD.X = x;
       LCD.Y = y;
    }
}

/********************************************************************************
Reverse Bits Function

The current font library that I built has bits reversed to a previous library
I was using. This was a simple fix to that, since I couldn't find an easy way
to produce a font library that was designed the way I wanted it to be.
********************************************************************************/
uint8_t reverseBits(uint8_t num){
	uint8_t result = 0, i, temp;

	for (i = 0; i < 8; i++){
			temp = (num & (1 << i));
			if(temp) {
					result |= (1 << ((8 - 1) - i));
			}
	}

	return result;
}

/********************************************************************************
Write Data Function

This sets the DC pin high which tells the LCD screen that data is being sent to it
********************************************************************************/
void writedata(uint8_t d){
	uint8_t data = d;
	GPIOA->ODR |= DC;
	GPIOA->ODR &= ~CS;
	SPI_Transmit(&data, 1);
	GPIOA->ODR |= CS;
}

/********************************************************************************
Write Command Function

This sets the DC pin low which tells the LCD screen that there is a command being
sent to it
********************************************************************************/
void writecommand(uint8_t d){
	uint8_t data = d;
	GPIOA->ODR &= ~DC;
	GPIOA->ODR &= ~CS;
	SPI_Transmit(&data, 1);
	GPIOA->ODR |= CS;
}

/********************************************************************************
Set Address Window Function

Page 110+111 of ILI9341 datasheet

This is the first step to the flowchart to writing display data to the LCD
This defines the area of frame memory that the MCU will access. This can be as
small as a single pixel you are displaying, or the whole screen such as when
the init function sets the background to black.
********************************************************************************/

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    writecommand(0x2A);
    writedata(x0>>8);
    writedata(x0);   
    writedata(x1>>8);
    writedata(x1);    
	
    writecommand(0x2B); 
    writedata(y0>>8);
    writedata(y0);    
    writedata(y1>>8);
    writedata(y1); 

    writecommand(0x2C); 
}

/********************************************************************************
Fill Rectangle Function

Fills a rectangle with specified color and dimensions. First 2 if statements are
used to ensure that the specified region is inside the dimensions of the screen.

Set addressing window is called to tell the LCD which area of memory we are
going to be writing to. The DC pin must be set high for data being sent. The for
loop goes through each pixel and sets it to the specified color.
********************************************************************************/
void fillRect(int16_t y, int16_t x, int16_t h, int16_t w, uint16_t color){
	uint8_t color_data[2];

  if((x + w - 1) >=  LCD.WIDTH){
		w =  LCD.WIDTH  - x;
	}
  if((y + h - 1) >= LCD.HEIGHT){
		h = LCD.HEIGHT - y;
	}
	
	setAddrWindow(x, y, w+x-1, h+y-1);
	color_data[0] =  color>>8;
	color_data[1] =  color;
	DMA1_Channel3->CMAR = (uint32_t)color_data;
	GPIOA->ODR &= ~CS;
	GPIOA->ODR |= DC;
	DMA1_Channel3->CCR |= DMA_CCR_EN | DMA_CCR_TCIE;
	while(fill_screen < 55000);
	GPIOA->ODR |= CS;
}

/********************************************************************************
DMA Interrupt Handler

This is used to write data to the LCD screen. In terms of filling up the entire
screen with a color, this brought it down to 1-1.3 second fill time vs the 
3-3.3 second fill time without DMA
********************************************************************************/
void DMA1_Channel2_3_IRQHandler(void){
	while(!(DMA1->ISR & DMA_ISR_TCIF3));
	while (!(SPI1->SR & SPI_SR_TXE));
	fill_screen++;
	DMA1->IFCR = DMA_IFCR_CTCIF1;
	if(fill_screen > 55000){
		DMA1_Channel3->CCR &= ~(DMA_CCR_EN |DMA_CCR_TCIE);
	}
}
/********************************************************************************
Initialize LCD Screen Function

This runs through the set of commands needed to get the LCD screen ready to 
write data to it. These were taken from a combination of the data sheet and 
previous driver libraries
********************************************************************************/
void init_lcd(void){
    GPIOA->ODR &= ~(RS);
    LCD_Delay(100);
    GPIOA->ODR |= (RS);
    LCD_Delay(100);

    writecommand(0xEF);
    writedata(0x03);
    writedata(0x80);
    writedata(0x02);

    writecommand(0xCF);
    writedata(0x00);
    writedata(0XC1);
    writedata(0X30);

    writecommand(0xED);
    writedata(0x64);
    writedata(0x03);
    writedata(0X12);
    writedata(0X81);

    writecommand(0xE8);
    writedata(0x85);
    writedata(0x00);
    writedata(0x78);

    writecommand(0xCB);
    writedata(0x39);
    writedata(0x2C);
    writedata(0x00);
    writedata(0x34);
    writedata(0x02);

    writecommand(0xF7);
    writedata(0x20);

    writecommand(0xEA);
    writedata(0x00);
    writedata(0x00);

    writecommand(0xC0);    
    writedata(0x23);   

    writecommand(0xC1);    
    writedata(0x10);  

    writecommand(0xC5);  
    writedata(0x3e); 
    writedata(0x28);

    writecommand(0xC7);   
    writedata(0x86); 

    writecommand(0x36);    
    writedata(0x48);

    writecommand(0x3A);
    writedata(0x55);

    writecommand(0xB1);
    writedata(0x00);
    writedata(0x18);

    writecommand(0xB6);  
    writedata(0x08);
    writedata(0x82);
    writedata(0x27);

    writecommand(0xF2);  
    writedata(0x00);

    writecommand(0x26);  
    writedata(0x01);

    writecommand(0xE0);  
    writedata(0x0F);
    writedata(0x31);
    writedata(0x2B);
    writedata(0x0C);
    writedata(0x0E);
    writedata(0x08);
    writedata(0x4E);
    writedata(0xF1);
    writedata(0x37);
    writedata(0x07);
    writedata(0x10);
    writedata(0x03);
    writedata(0x0E);
    writedata(0x09);
    writedata(0x00);

    writecommand(0xE1); 
    writedata(0x00);
    writedata(0x0E);
    writedata(0x14);
    writedata(0x03);
    writedata(0x11);
    writedata(0x07);
    writedata(0x31);
    writedata(0xC1);
    writedata(0x48);
    writedata(0x08);
    writedata(0x0F);
    writedata(0x0C);
    writedata(0x31);
    writedata(0x36);
    writedata(0x0F);

    writecommand(0x11); 
    LCD_Delay(1200);
    writecommand(0x29);  
    LCD_Delay(100000);
		
    LCD.HEIGHT = 320;
    LCD.WIDTH = 240;
    LCD.X = 10;
    LCD.Y = 220;
		fillRect(0, 0, LCD.HEIGHT, LCD.WIDTH, BLACK);
}

void fill_background(void){
	fillRect(0, 0, LCD.HEIGHT, LCD.WIDTH, BLACK);
}

/********************************************************************************
Draw Pixel Function

Similar to the draw rectangle function, but it only focuses on 1 single pixel.
Set address window is called to define the region we are going to be writing to
then we set the DC pin high and send the color data to the LCD
********************************************************************************/
void drawPixel(int16_t x, int16_t y, uint16_t color){

    uint8_t color_data[2];

    setAddrWindow(y,x,y+1,x+1);

    GPIOA->ODR |= DC;
    GPIOA->ODR &= ~CS;

    color_data[0] = color >> 8;
    color_data[1] = color;
		
    SPI_Transmit(color_data, 2);

    GPIOA->ODR |= CS;
}

/********************************************************************************
Clear Char Function

This is similar to draw rectangle, but it's primary purpose is to blank out a
section of the screen that was previously written on, so that a new character
can be printed there. Uses DMA for faster writing.
********************************************************************************/
void clearChar(int16_t y, int16_t x, int16_t h, int16_t w, uint16_t color){
	uint8_t color_data[2];

  if((x + w - 1) >=  LCD.WIDTH){
		w =  LCD.WIDTH  - x;
	}
  if((y + h - 1) >= LCD.HEIGHT){
		h = LCD.HEIGHT - y;
	}

	setAddrWindow(x, y, w+x, h+y);
	color_data[0] = color >> 8;
	color_data[1] = color;
	DMA1_Channel3->CMAR = (uint32_t)color_data;
	GPIOA->ODR &= ~CS;
	GPIOA->ODR |= DC;
	fill_screen = 54800;
	DMA1_Channel3->CCR |= (DMA_CCR_EN | DMA_CCR_TCIE);
	while(fill_screen < 55000);
	GPIOA->ODR |= CS;
}
/********************************************************************************
Draw Char Function

This uses the Verdana 22x21 font library and in that library the width, and height
of each character is specified. This function checks the width of the character
then reads the bitmap information and stores it into a temp array. Once the temp
array is filled it then goes through a for loop that ORs each element of that array
with a 1 and draws the pixel if the result is a 1.
********************************************************************************/
void drawChar(char character){

    uint8_t k = 0,i,j,l = 0;
    uint8_t charWidth;
    uint8_t charHeight;
    uint16_t charOffset;
    uint8_t tempX,tempY;
    uint8_t mask = 0x80;
    uint16_t mask16 = 0x8000;
    uint32_t mask24 = 0x800000;
    tempX = LCD.X;
    tempY = LCD.Y;


//Taking data from font library such as width, height, and offset
    charWidth = verdana_descriptors[(uint8_t)character - ' '][0];

    charHeight = verdana_descriptors[(uint8_t)character - ' '][1];

    charOffset = verdana_descriptors[(uint8_t)character - ' '][2];
		
//Creating temp arrays with the length of the characters height
//These will be used to store the bitmap of the character
    uint8_t temp[charHeight];
    uint16_t temp16[charHeight];
    uint32_t temp24[charHeight];

//These if statements go through the bitmaps of the characters
//and stores each row of the bitmap into the temp array
  if(charWidth <= 8){
    while(k < charHeight){
        temp[k] = reverseBits(Verdana22x21[(charOffset+k)]);
        k++;
    }
  } else if(charWidth <= 16) {
      while(k < charHeight) {
          temp16[k] = ((reverseBits(Verdana22x21[(charOffset+l)]) << 8) & 0xFF00) + reverseBits(Verdana22x21[(charOffset+l+1)]);
          k++;
          l+=2;
      }
  } else {
      while(k < charHeight){
          temp24[k] = ((reverseBits(Verdana22x21[(charOffset+l)]) << 16) & 0xFF0000) + ((reverseBits(Verdana22x21[(charOffset+l+1)]) << 8) & 0xFF00) + reverseBits(Verdana22x21[(charOffset+l+2)]);
          k++;
          l+=3;
      }
  }
	
//Preps the block just incase there is any previous text written there
	clearChar(LCD.X, LCD.Y-21, charWidth, charHeight, BLACK);
	
	
/*
This is where the character is printed onto the LCD screen
It goes through the temp array that was created and ORs
each bit with a the mask for the given charWidth
This means that each times a 1 is encountered it will
draw a pixel with the specified color at that location	
*/
  if(charWidth <= 8){
    for(i = 0 ; i < charHeight ; i++){
           for(j = 0 ; j < (charWidth + 1) ; j++){
              if(temp[i] & (mask >> j)) {
                drawPixel(tempX , tempY , GREEN);
              }
              tempX++;
           }
           tempX = LCD.X;
           tempY--;
    }
  } else if(charWidth <= 16){
      for(i = 0 ; i < charHeight ; i++) {
             for(j = 0 ; j < (charWidth + 1) ; j++) {
                if(temp16[i] & (mask16 >> j)) {
                  drawPixel(tempX , tempY , GREEN);
                }
                tempX++;
             }
             tempX = LCD.X;
             tempY--;
      }
  } else {
      for(i = 0 ; i < charHeight ; i++) {
             for(j = 0 ; j < (charWidth + 1) ; j++) {
                if(temp24[i] & (mask24 >> j)) {
                  drawPixel(tempX , tempY , GREEN);
                }
                tempX++;
             }
             tempX = LCD.X;
             tempY--;
      }
  }
  LCD.X += ((charWidth) + 1);
}

uint8_t get_charWidth(char character){
	uint8_t charWidth = charWidth = verdana_descriptors[(uint8_t)character - ' '][0];
	return (charWidth+1);
}
/********************************************************************************
Draw String Function

Goes through a string and runs drawChar for each character in the string
********************************************************************************/
void drawString(char *str){
    while(*str){
        drawChar(*(str++));
    }
}
