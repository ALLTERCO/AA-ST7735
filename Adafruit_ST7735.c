/***************************************************
 * This is an LGPL attempt to port the excelent Adafruit library 
 * from C++ to C for platforms lacking C++ or Arduino support 
 * 
 * This library is Copyright (c) 2016, Allterco, Inc.  All rights reserved
 * 
 ****************************************************/

/**********
 * T his library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 **********/

//Disclimer (sans licens lines) from C++ sources written by 
//Limor Fried/Ladyada for Adafruit Industries.
/***************************************************
 *  This is a library for the Adafruit 1.8" SPI display.
 * 
 * This library works with the Adafruit 1.8" TFT Breakout w/SD card
 *  ----> http://www.adafruit.com/products/358
 * The 1.8" TFT shield
 *  ----> https://www.adafruit.com/product/802
 * The 1.44" TFT breakout
 *  ----> https://www.adafruit.com/product/2088
 * as well as Adafruit raw 1.8" TFT display
 *  ----> http://www.adafruit.com/products/618
 * 
 *  Check out the links above for our tutorials and wiring diagrams
 *  These displays use SPI to communicate, 4 or 5 pins are required to
 *  interface (RST is optional)
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing
 *  products from Adafruit!
 * 
 ****************************************************/

#include "Adafruit_ST7735_c.h"


static inline uint16_t swapcolor(uint16_t x) { 
  return (x << 11) | (x & 0x07E0) | (x >> 11);
}




// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80
static const uint8_t 
  Bcmd[] = {                  // Initialization commands for 7735B screens
    18,                       // 18 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ST7735_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    ST7735_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ST7735_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ST7735_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    ST7735_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16+DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    ST7735_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    ST7735_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 },                  //     255 = 500 ms delay

  Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

  Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01 },      //     XEND = 159
  Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F },           //     XEND = 159

  Rcmd2green144[] = {              // Init for 7735R, part 2 (green 1.44 tab)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F },           //     XEND = 127

  Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay


// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Adafruit_ST7735_commandList(const Adafruit_ST7735_displayInfo_t* di, const uint8_t *addr) { 
	(void)di; //we ignore di here

	uint8_t  numCommands, numArgs;
	uint16_t ms;

	numCommands = *addr;addr++;   // Number of commands to follow
	while(numCommands--) {                 // For each command...
		uint8_t cmd=*addr;addr++;
		numArgs  = *addr;addr++;    //   Number of args to follow
		ms       = numArgs & DELAY;          //   If hibit set, delay follows args
		numArgs &= ~DELAY;                   //   Mask out delay bit
		Adafruit_ST7735_writecmddatabuf(cmd,addr,numArgs); addr+=numArgs;

		if(ms) {
			ms = *addr;addr++; // Read post-command delay time (ms)
			if(ms == 255) ms = 500;     // If 255, delay for 500 ms
			Adafruit_ST7735_dealy_ms(ms);
		}
	}
}


// Initialization for ST7735B screens
const Adafruit_ST7735_displayInfo_t*  Adafruit_ST7735_initB(Adafruit_ST7735_displayInfo_t* di) {
	MEMINIT_ADAFRUIT_ST7735_DISPLAYINFO_T(di,ST7735_TFTHEIGHT_18,0,0,0);
	Adafruit_ST7735_commandList(di,Bcmd);
	return di;
}


// Initialization for ST7735R screens (green or red tabs)
const Adafruit_ST7735_displayInfo_t* Adafruit_ST7735_initR(Adafruit_ST7735_displayInfo_t* di,uint8_t options) {
	if(options == INITR_GREENTAB) {
		MEMINIT_ADAFRUIT_ST7735_DISPLAYINFO_T(di,ST7735_TFTHEIGHT_18,2,1,options);
		Adafruit_ST7735_commandList(di,Rcmd1);
		Adafruit_ST7735_commandList(di,Rcmd2green);
	} else if(options == INITR_144GREENTAB) {
		MEMINIT_ADAFRUIT_ST7735_DISPLAYINFO_T(di,ST7735_TFTHEIGHT_144,2,3,options);
		Adafruit_ST7735_commandList(di,Rcmd1);
		Adafruit_ST7735_commandList(di,Rcmd2green144);
	} else {
		MEMINIT_ADAFRUIT_ST7735_DISPLAYINFO_T(di,ST7735_TFTHEIGHT_18,0,0,options);
		Adafruit_ST7735_commandList(di,Rcmd2red);
	}
	Adafruit_ST7735_commandList(di,Rcmd3);

	// if black, change MADCTL color filter
	if (options == INITR_BLACKTAB) {
		uint8_t data=0xC0;
		Adafruit_ST7735_writecmddatabuf(ST7735_MADCTL,&data,1);
	}

	return di;
}


void Adafruit_ST7735_setWindowCD(const Adafruit_ST7735_displayInfo_t* di,uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,const uint8_t*colordata) {
	uint8_t db[4];
	//             XSTART                           XEND
	db[0]=0x00;db[1]=x0+di->colstart;db[2]=0x00;db[3]=x1+di->colstart;
	Adafruit_ST7735_writecmddatabuf(ST7735_CASET,db,4); // Column addr set

	//         YSTART                YEND
	db[1]=y0+di->rowstart;db[3]=y1+di->rowstart;
	Adafruit_ST7735_writecmddatabuf(ST7735_RASET,db,4); // Row addr set
	Adafruit_ST7735_writecmddatabuf(ST7735_RAMWR,colordata,(x1-x0)*(y1-y0)*2); // write to RAM
}

void Adafruit_ST7735_setWindowFILL(const Adafruit_ST7735_displayInfo_t* di, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint16_t color){
	uint8_t db[4];
	//             XSTART                           XEND
	db[0]=0x00;db[1]=x0+di->colstart;db[2]=0x00;db[3]=x1+di->colstart;
	Adafruit_ST7735_writecmddatabuf(ST7735_CASET,db,4); // Column addr set

	//         YSTART                YEND
	db[1]=y0+di->rowstart;db[3]=y1+di->rowstart;
	Adafruit_ST7735_writecmddatabuf(ST7735_RASET,db,4); // Row addr set
	Adafruit_ST7735_writecmddatafill(ST7735_RAMWR,color>>8,color &0x0ff,(x1-x0)*(y1-y0)); // write to RAM
}

/*
void Adafruit_ST7735::pushColor(uint16_t color) {
#if defined (SPI_HAS_TRANSACTION)
    SPI.beginTransaction(mySPISettings);
#endif
  *rsport |=  rspinmask;
  *csport &= ~cspinmask;
  
  
  spiwrite(color >> 8);
  spiwrite(color);

  *csport |= cspinmask;
#if defined (SPI_HAS_TRANSACTION)
    SPI.endTransaction();
#endif
}*/

void Adafruit_ST7735_drawPixel(const Adafruit_ST7735_displayInfo_t* di,int16_t x, int16_t y, uint16_t color) {
	if((x < 0) ||(x >= di->_width) || (y < 0) || (y >= di->_height)) return;
	uint8_t cd[2]={color >> 8,color&0x0ff};
	Adafruit_ST7735_setWindowCD(di,x,y,x+1,y+1,cd);
}


void Adafruit_ST7735_drawFastVLine(const Adafruit_ST7735_displayInfo_t* di,int16_t x, int16_t y, int16_t h, uint16_t color) {
	// Rudimentary clipping
	if((x >= di->_width) || (y >= di->_height)) return;
	if((y+h-1) >= di->_height) h = di->_height-y;
	Adafruit_ST7735_setWindowFILL(di,x, y, x, y+h-1,color);
}


void Adafruit_ST7735_drawFastHLine(const Adafruit_ST7735_displayInfo_t* di,int16_t x, int16_t y, int16_t w, uint16_t color) {

	// Rudimentary clipping
	if((x >= di->_width) || (y >= di->_height)) return;
	if((x+w-1) >= di->_width)  w = di->_width-x;
	Adafruit_ST7735_setWindowFILL(di,x, y, x+w-1, y,color);
}



void Adafruit_ST7735_fillScreen(const Adafruit_ST7735_displayInfo_t* di,uint16_t color) {
	Adafruit_ST7735_setWindowFILL(di,0, 0,  di->_width, di->_height, color);
}



// fill a rectangle
void Adafruit_ST7735_fillRect(const Adafruit_ST7735_displayInfo_t* di, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= di->_width) || (y >= di->_height)) return;
  if((x + w - 1) >= di->_width)  w = di->_width  - x;
  if((y + h - 1) >= di->_height) h = di->_height - y;

  Adafruit_ST7735_setWindowFILL(di,x, y, x+w-1, y+h-1,color);

}


// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t Adafruit_ST7735_Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

void Adafruit_ST7735_setRotation(Adafruit_ST7735_displayInfo_t* di,uint8_t m) {
	uint8_t data;
	m = m % 4; // can't be higher than 3
	switch (m) {
	case 0:
		if (di->tabcolor == INITR_BLACKTAB) data=MADCTL_MX | MADCTL_MY | MADCTL_RGB;
		data=MADCTL_MX | MADCTL_MY | MADCTL_BGR;
		
		di->_width  = ST7735_TFTWIDTH;
		
		if (di->tabcolor == INITR_144GREENTAB) di->_height = ST7735_TFTHEIGHT_144;
		else di->_height = ST7735_TFTHEIGHT_18;

		break;
	case 1:
		if (di->tabcolor == INITR_BLACKTAB) data=MADCTL_MY | MADCTL_MV | MADCTL_RGB;
		else data =MADCTL_MY | MADCTL_MV | MADCTL_BGR;
		

		if (di->tabcolor == INITR_144GREENTAB) di->_width = ST7735_TFTHEIGHT_144;
		else di->_width = ST7735_TFTHEIGHT_18;

		di->_height = ST7735_TFTWIDTH;
		break;
	case 2:
		if (di->tabcolor == INITR_BLACKTAB) data=MADCTL_RGB;
		else data=MADCTL_BGR;
		
		di->_width  = ST7735_TFTWIDTH;
		if (di->tabcolor == INITR_144GREENTAB) di->_height = ST7735_TFTHEIGHT_144;
		else di->_height = ST7735_TFTHEIGHT_18;

	break;
	case 3:
		if (di->tabcolor == INITR_BLACKTAB) data=MADCTL_MX | MADCTL_MV | MADCTL_RGB;
		else data=MADCTL_MX | MADCTL_MV | MADCTL_BGR;
		
		if (di->tabcolor == INITR_144GREENTAB) di->_width = ST7735_TFTHEIGHT_144;
		else di->_width = ST7735_TFTHEIGHT_18;

		di->_height = ST7735_TFTWIDTH;
		break;
	}
	Adafruit_ST7735_writecmddatabuf(ST7735_MADCTL,&data,1);

}


void Adafruit_ST7735_invertDisplay(const Adafruit_ST7735_displayInfo_t* di, uint8_t i) {
	(void)di;
	Adafruit_ST7735_writecmddatabuf(i ? ST7735_INVON : ST7735_INVOFF,(uint8_t*)0/*NULL*/,0);
}

