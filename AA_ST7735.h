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

#ifndef _ADAFRUIT_ST7735_C_H_
#define _ADAFRUIT_ST7735_C_H_

#include <stdint.h>

#ifndef ADAFRUIT_ST7735_NO_GFX //uness not GFX  disabled

	#ifdef ADAFRUIT_ST7735_CUSTOM_GFX //we got custom GFX config 
		#include "AA_ST7735_custom_gfx.h"
	#else //use full GFX config
		#include "AA_ST7735_default_gfx.h"
	#endif //ADAFRUIT_ST7735_CUSTOM_GFX


	#include "AA-GFX/gfxfont.h"
	#include "AA-GFX/AA_GFX_common.h"

#else //NO ADAFRUIT_ST7735_NO_GFX 

	#define AA_GFX_COMMON_DI_INCLUDES
	#define AA_GFX_COMMON_DI_INITS(di)

#endif // NO ADAFRUIT_ST7735_NO_GFX


// some flags for initR() :(
#define INITR_GREENTAB 0x0
#define INITR_REDTAB   0x1
#define INITR_BLACKTAB   0x2

#define INITR_18GREENTAB    INITR_GREENTAB
#define INITR_18REDTAB      INITR_REDTAB
#define INITR_18BLACKTAB    INITR_BLACKTAB
#define INITR_144GREENTAB   0x1

#define ST7735_TFTWIDTH  128
// for 1.44" display
#define ST7735_TFTHEIGHT_144 128
// for 1.8" display
#define ST7735_TFTHEIGHT_18  160

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF

//ST7735RC variant colors
#define	ST7735RC_BLACK   0x0000
#define ST7735RC_WHITE   0xFFFF

#define	ST7735RC_RED     0x001F
#define	ST7735RC_GREEN    0x07E0
#define	ST7735RC_BLUE   0xF800

#define ST7735RC_CYAN    (ST7735RC_GREEN | ST7735RC_BLUE)
#define ST7735RC_MAGENTA (ST7735RC_RED | ST7735RC_BLUE)
#define ST7735RC_YELLOW  (ST7735RC_RED | ST7735RC_GREEN)

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
#define  AA_ST7735_COLOR565(r,g,b)((uint16_t)(\
	  ((((uint16_t)(r)) & 0xF8) << 8) \
	| ((((uint16_t)(g)) & 0xFC) << 3) \
	|  (((uint16_t)(b)) >> 3) \
))

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
#define  AA_ST7735RC_COLOR565(r,g,b) ((uint16_t)(\
	  ((((uint16_t)(b)) & 0xF8) << 8) \
	| ((((uint16_t)(g)) & 0xFC) << 3) \
	|  (((uint16_t)(r)) >> 3) \
))


#define  AA_ST7735_COLOR565_(rgb) AA_ST7735_COLOR565((((rgb)&0x00FF0000)>>16),(((rgb)&0x0000FF00)>>8),((rgb)&0x000000FF))

#define  AA_ST7735RC_COLOR565_(rgb) AA_ST7735RC_COLOR565((((rgb)&0x00FF0000)>>16),(((rgb)&0x0000FF00)>>8),((rgb)&0x000000FF))


//You must provide this functions 
//to keep things simple we assume single display per device
//and we do not pass display info to HW Line driver functions below
extern void AA_ST7735_writecmddatabuf(uint8_t c,const uint8_t *d, uint16_t cnt);
extern void AA_ST7735_writecmddatafill(uint8_t c,uint8_t b1,uint8_t b2, uint16_t cnt); //number of two-byte-data blocks to be send
extern void AA_ST7735_dealy_ms(unsigned ms);


typedef struct {
	  int16_t width;
	  int16_t height;
	  uint8_t colstart;
	  uint8_t rowstart;
	  uint8_t tabcolor;
	  AA_GFX_COMMON_DI_INCLUDES
} AA_ST7735_displayInfo_t;
//a macro used in AA_ST7735_initX
#define MEMINIT_ADAFRUIT_ST7735_DISPLAYINFO_T(di,h,c,r,t) \
	di->width=ST7735_TFTWIDTH;\
	di->height=h;\
	di->colstart=c;\
	di->rowstart=r;\
	di->tabcolor=t;\
	AA_GFX_COMMON_DI_INITS(di)

//this functions init the displayInfo in di return the pointer to it just to make more obvious how to obtain const AA_ST7735_displayInfo_t*
const AA_ST7735_displayInfo_t* AA_ST7735_initB(AA_ST7735_displayInfo_t* di); // for ST7735B displays
const AA_ST7735_displayInfo_t* AA_ST7735_initR(AA_ST7735_displayInfo_t* di, uint8_t tabcolor/* some of INITR_XXXXXTAB */); // for ST7735R

//all the folowing functions get a const AA_ST7735_displayInfo_t* di for consistancy though not all of them might need it
void AA_ST7735_setWindowCD(const AA_ST7735_displayInfo_t* di,uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,const uint8_t*colordata);
static inline void AA_ST7735_rawImg(const AA_ST7735_displayInfo_t* di,uint8_t x, uint8_t y, int16_t w, int16_t h,const uint8_t*rawimg){AA_ST7735_setWindowCD (di,x,y,x+w-1,y+h-1,rawimg);};
void AA_ST7735_setWindowFILL(const AA_ST7735_displayInfo_t* di,uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,const uint16_t color);
void AA_ST7735_fillScreen(const AA_ST7735_displayInfo_t* di, uint16_t color);
void AA_ST7735_drawPixel(const AA_ST7735_displayInfo_t* di, int16_t x, int16_t y, uint16_t color);
void AA_ST7735_drawFastVLine(const AA_ST7735_displayInfo_t* di, int16_t x, int16_t y, int16_t h, uint16_t color);
void AA_ST7735_drawFastHLine(const AA_ST7735_displayInfo_t* di, int16_t x, int16_t y, int16_t w, uint16_t color);
void AA_ST7735_fillRect(const AA_ST7735_displayInfo_t* di, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
//For the second roration to succeed you need to reinit the display
void AA_ST7735_setRotation(AA_ST7735_displayInfo_t* di, uint8_t r);
void AA_ST7735_invertDisplay(const AA_ST7735_displayInfo_t* di, uint8_t i /*bool*/); 

//simple "scripting" function used mainly in init proc
void AA_ST7735_commandList(const AA_ST7735_displayInfo_t* di, const uint8_t *addr);

//colorspace convertion utility func
uint16_t AA_ST7735_Color565(uint8_t r, uint8_t g, uint8_t b);

#define GFX_displayInfo_t AA_ST7735_displayInfo_t

#define GFX_drawPixel AA_ST7735_drawPixel
#define GFX_drawFastHLine AA_ST7735_drawFastHLine
#define GFX_drawFastVLine AA_ST7735_drawFastVLine
#define GFX_fillRect AA_ST7735_fillRect
#define GFX_fillScreen AA_ST7735_fillScreen
#define GFX_rawImg AA_ST7735_rawImg

#define GFX_setWindowFILL AA_ST7735_setWindowFILL
#define GFX_setWindowCD AA_ST7735_setWindowCD
#define GFX_setRotation AA_ST7735_setRotation


#ifndef AA_ST7735_DISPLAY_ID 
	#define AA_ST7735_DISPLAY_ID 0x10007735
#endif
#ifndef AA_DEFAULT_DISPLAY
	#define AA_DEFAULT_DISPLAY  0x10000000
#endif

#if AA_ST7735_DISPLAY_ID == AA_DEFAULT_DISPLAY
	#define AA_GFX_DEFAULT_DISPLAY AA_ST7735_DISPLAY_ID
#else
	#undef AA_GFX_DEFAULT_DISPLAY
#endif

#ifndef ADAFRUIT_ST7735_NO_GFX //uness not GFX  disabled
	#include "AA-GFX/AA_GFX_proto.h"
#endif

#ifndef AA_GFX_DEFAULT_DISPLAY

	#undef GFX_displayInfo_t

	#undef GFX_drawPixel
	#undef GFX_drawFastHLine
	#undef GFX_drawFastVLine
	#undef GFX_fillRect
	#undef GFX_fillScreen

	#undef GFX_setWindowFILL
	#undef GFX_setWindowCD
	#undef GFX_rawImg
	#undef GFX_setRotation 

	#undef GFX_drawLine 
	#undef GFX_drawRect  
	#undef GFX_drawCircle 
	#undef GFX_drawCircleHelper 
	#undef GFX_fillCircle 
	#undef GFX_fillCircleHelper 
	#undef GFX_drawRoundRect 
	#undef GFX_fillRoundRect 
	#undef GFX_drawTriangle 
	#undef GFX_fillTriangle
	#undef GFX_drawBitmapT 
	#undef GFX_drawBitmapBG 
	#undef GFX_drawXBitmapT 
	#undef GFX_drawXBitmapBG 
	#undef GFX_setCursor
	#undef GFX_setCursorRel
	#undef GFX_setWrap
	#undef GFX_drawChar 
	#undef GFX_putsEx
	#undef GFX_putsSetup
	#undef GFX_puts
	


	#undef GFX_rawImgFromFile
	#undef GFX_rawImgFromFH
#endif //AA_GFX_DEFAULT_DISPLAY

#endif //_ADAFRUIT_ST7735_C_H_
