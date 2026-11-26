#ifndef __OLED_H__
#define __OLED_H__

#include "stm32f4xx.h"
#include "spi.h"
#include "time.h"
#include "string.h"


void OLED_IO_Init(void);
void OLED_WriteCom(u8 Command);
void OLED_WriteData(u8 Data);
void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y) ;
void OLED_Clear(u8 x0,u8 y0,u8 x1, u8 y1);
void OLED_8X16_Ascii_Disp(int Xpos,int Ypos,char Str);
void OLED_String_Disp(u8 Xpos,u8 Ypos,char *Str);
void OLED_DrawBMP(u8 x0, u8 y0,u8 x1, u8 y1,u8 BMP[]);
void OLED_16X16_CHinese_Disp(int Xpos,int Ypos,  const char *Font,u8 Num);
void OLED_CHinese_Disp(int Xpos,int Ypos,  const unsigned char *Font,int Width,int High);
void OLED_Dis_String(u8 Xpos,u8 Ypos,  const unsigned char *Str);
#endif

