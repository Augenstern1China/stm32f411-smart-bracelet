#include "oled.h"
#include "font.h"
#include "dzk.h"

/*
PA15---D/C-----通用推挽功能---高数低命
PB7----CS------通用推挽功能
PB13---RES-----通用推挽功能---显示控制器复位管脚，有效电平为低电平

PB3----SCL-----复用推挽功能 
PB5----MOSI----复用推挽功能

*/

void OLED_IO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |RCC_AHB1Periph_GPIOB,ENABLE);
	
	//PA15 -------通用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;   //通用推挽输出
	GPIO_InitStruct.GPIO_OType =GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	//PB7 PB13-------通用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;   //通用推挽输出
	GPIO_InitStruct.GPIO_OType =GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//PB3  PB5-------复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;   //复用推挽输出
	GPIO_InitStruct.GPIO_OType =GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);//选择复位功能
	
}

/************************* OLED写命令函数 ***********************/
void OLED_WriteCom(u8 Command)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);       //拉低数据/命令选择管脚(D/C)，低电平表示要操作命令
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);        //拉低/CS片选管脚，使能器件
	SPI1_Write_Byte(Command);  			 	//发送命令数据
	GPIO_SetBits(GPIOB,GPIO_Pin_7);         //拉高/CS片选管脚，结束通信
}

/************************* OLED写数据函数 ***********************/
void OLED_WriteData(u8 Data)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_15);         //拉高数据/命令选择管脚(D/C)，高电平表示要操作数据
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);           //拉低/CS片选管脚，使能器件
	SPI1_Write_Byte(Data);      //发送命令数据
	GPIO_SetBits(GPIOB,GPIO_Pin_7);          //拉高/CS片选管脚，结束通信
}


/************************* OLED初始化函数 ***********************/
void OLED_Init(void)
{	
	OLED_IO_Init();  
	SPI_1_Init();

	GPIOB->ODR &= (~(1 << 13));
	TIM3_ms(500);
	GPIOB->ODR = 1 << 13;
	TIM3_ms(500); 

	
	OLED_WriteCom(0xAE); /*display off*/ 

	OLED_WriteCom(0x02); /*set lower column address*/ 
	OLED_WriteCom(0x10); /*set higher column address*/ 

	OLED_WriteCom(0x40); /*set display start line*/ 

	OLED_WriteCom(0xB0); /*set page address*/ 

	OLED_WriteCom(0x81); /*contract control*/ 
	OLED_WriteCom(0xBF); /*128*/ 

	OLED_WriteCom(0xA1); /*set segment remap  左右翻转*/ 

	OLED_WriteCom(0xA6); /*normal / reverse*/ 

	OLED_WriteCom(0xA8); /*multiplex ratio*/ 
	OLED_WriteCom(0x3F); /*duty = 1/64*/ 

	OLED_WriteCom(0xad); /*set charge pump enable*/ 
	OLED_WriteCom(0x8b); /*  0x8B = 开启电荷泵 */ 

	OLED_WriteCom(0x32); /*0X30---0X33 set VPP 8V */ 

	OLED_WriteCom(0xC8); /*Com scan direction*/ 

	OLED_WriteCom(0xD3); /*set display offset*/ 
	OLED_WriteCom(0x00); /* 0x20 */ 

	OLED_WriteCom(0xD5); /*set osc division*/ 
	OLED_WriteCom(0x80); 

	OLED_WriteCom(0xD9); /*set pre-charge period*/ 
	OLED_WriteCom(0x22); /*0x22*/ 

	OLED_WriteCom(0xDA); /*set COM pins*/ 
	OLED_WriteCom(0x12); 

	OLED_WriteCom(0xdb); /*set vcomh*/ 
	OLED_WriteCom(0x40); 

	OLED_WriteCom(0x8D);
	OLED_WriteCom(0x14);


	OLED_WriteCom(0xAF); /*display ON*/ 


	OLED_Clear(0,0,127,7);  //把整个OLED液晶屏进行清屏
}
/*************************** OLED设置像素点函数 ***************************
参数说明：
x：像素点的X轴坐标，x的范围0～127，
y：像素点的Y轴坐标，y为页的范围0～7    121   0X82   0001 1000 
**************************************************************************/
void OLED_SetPos(unsigned char x, unsigned char y) 
{ 
	x += 2;       //OLED原点对应SH1106的原点偏移2个像素点
	OLED_WriteCom(0xB0 + y);  //设置Y轴   0X13 & 0XF0  0X11 0X03
	OLED_WriteCom(((x & 0xf0) >> 4)| 0x10);   //高四位  1X
	OLED_WriteCom((x & 0x0f) | 0x00);    //低四位       0X
}              //0000 0010 | 0000 0000  0X 0 2         

/****************************** OLED清屏函数 ******************************
参数说明：
x、x1：X轴的起始、结束坐标，x的范围0～127，
y、y1：Y轴的起始、结束坐标，y为页的范围0～7
B0-B7
***************************************************************************/
void OLED_Clear(u8 x0,u8 y0,u8 x1, u8 y1)
{
	u8 x,y;

	for(y = y0;y <= y1;y++)
	{
		OLED_SetPos(x0,y);
		for(x = x0;x <= x1;x++)//0-127
		{
			OLED_WriteData(0x00);    //写入数据0，清屏	    	
		}
	}
}


/*************************************************************************************
函数功能：OLED显示字符函数 
函数参数：
	Xpos：写入字符的列地址
	Ypos 写入字符的页地址
	Str：  需要写入的字符数据
OLED_8X16_Ascii_Disp(0,0,'g');
**************************************************************************************/
void OLED_8X16_Ascii_Disp(int Xpos,int Ypos,char Str)
{
	char Buff[16];                //存放显示字符的字模
	u8 i = 0,j;
	
	/* 处理要显示的字符字模 */
	Str = Str - 32;            	//得到Ascill码值的偏移量
	memcpy(Buff, &AsciII[Str * 16], 16);  //把需要显示的字模赋值到buff数组
	//memset
	/* 显示字符字模 */
	for(i = 0;i < (16 / 8); i++)
	{    //  10  , 3
		OLED_SetPos(Xpos,Ypos + i); 
		for(j = 0;j < 8; j++)
		{
		  OLED_WriteData( Buff[j + 8 * i] );
		}
	}
	
}

/*************************************************************************************
函数功能：OLED显示字符串函数 
参数说明：
	Xpos： 写入字符的列地址
	Ypos： 写入字符的页地址
	Str：  需要写入的字符串数据
*************************************************************************************/
void OLED_String_Disp(u8 Xpos,u8 Ypos,char *Str)
{
	while( *Str != '\0') 
	{
		OLED_8X16_Ascii_Disp(Xpos,Ypos,*Str);
		Xpos += 8;
		Str++;
	}	
}

/*********************************************************************************
函数功能： OLED显示中文汉字函数 
参数说明  ：   
Page： 写入汉字的页地址
Xpos：写入汉字的行地址
Str：  需要写入的汉字数据字模
Num：  需要显示字模中的第几个汉字
***********************************************************************************/ 
void OLED_16X16_CHinese_Disp(int Xpos,int Ypos,  const char *Font,u8 Num)
{
	u8 Buff[32] = {0};
	u8 i,j;

	/* 读取字模数据 */
	memcpy(Buff,&Font[Num * 32],32);  //读取一个汉字的字模
	
	/* OLED液晶屏显示 */
	for(i = 0; i < 16 / 8; i++)                
	{
		OLED_SetPos(Xpos,Ypos + i);
		for(j = 0; j < 16; j++)
		{
			OLED_WriteData(Buff[16 * i+ j]);
		}
	}
}

/**********************************************************************************************************
* 函数名    ：   OLED_drawBMP
* 功能描述  ：   功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7
* 参数说明  ：
* 返回值说明：   none
**********************************************************************************************************/ 
void OLED_DrawBMP(u8 x0, u8 y0,u8 x1, u8 y1,u8 BMP[])
{
	unsigned int j = 0;
	unsigned char x,y;

	for(y = y0; y < y1;y++)
	{
		OLED_SetPos(x0,y);
		for(x = x0; x < x1; x++)
		{
			OLED_WriteData(BMP[j++]);	    	
		}
	}
} 

 

/****************************** OLED显示中文汉字函数 ******************************
参数说明  ：   
Page： 写入汉字的页地址
Xpos：写入汉字的行地址
Str：  需要写入的汉字数据字模
Width：字符字体的宽度
High： 字符字体的高度（高度必须是8的倍数）
函数功能说明：从工程当中头文件当中的数组里面找对应的汉字的字模 
***********************************************************************************/ 
void OLED_CHinese_Disp(int Xpos,int Ypos,  const unsigned char *Font,int Width,int High)
{
	u8 Buff[32] = {0};
	u8 i,j;

	/* 读取字模数据 */
	memcpy((char *)Buff,Font, 32);
	
	/* OLED液晶屏显示 */
	for(i = 0; i < High / 8; i++)                
	{
		OLED_SetPos(Xpos,Ypos + i);
		for(j = 0; j < Width; j++)
		{
			OLED_WriteData(Buff[Width * i + j]);
		}
	}
}


/************************************** OLED混合显示函数 **************************************
功  能：在OLED液晶屏混合显示汉字和字符
参  数：Page： 写入汉字的页地址
		Xpos：写入汉字的行地址
		Str：  需要写入的汉字数据字模
		Width：字符字体的宽度
		High： 字符字体的高度（高度必须是8的倍数）
返回值：无
备注:显示没有取模的汉字，汉字是显示不出来的，添加新汉字的方法：
1、取模16x16汉字(阴码、列行式、逆向)
2、将取模好的好的数据粘贴到dzk.h文件的DZK[]数组最后
3、在Hz_Index[]数组后面加上取模数据的对应汉字
4、更改dzk.h文件中GB16_NUM宏定义的数值，这个宏的值就是Hz_Index[]中总的汉字个数

函数功能说明：从工程当中头文件当中的数组里面找对应的汉字的字模 
************************************************************************************************/
void OLED_Dis_String(u8 Xpos,u8 Ypos,  const unsigned char *Str)
{
	u32 i;
	
	while (*Str != '\0')
	{
		if(*Str >= ' ' && *Str <= '~')                 
		{
			if( Xpos >= (128 - 8) )
			{
				Xpos = 0;
				Ypos += 2;
			}
			OLED_8X16_Ascii_Disp(Xpos,Ypos,*Str);    
			Xpos += 8;
			Str++;                                 
		}
		else if(*Str >= 0xA1 && *Str <= 0xFE)
		{
			if( Xpos >= (128 - 16) )
			{
				Xpos = 0;
				Ypos += 2;
			}
			for (i = 0; i < GB16_NUM; i++)                                      
			{
				if ( (*Str == Hz_Index[i*2]) && (*(Str+1) == Hz_Index[i*2+1]) )
				{
					OLED_CHinese_Disp(Xpos,Ypos,  &DZK[i * 32],16,16);
					Xpos += 16;
					Str+=2;
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
}
