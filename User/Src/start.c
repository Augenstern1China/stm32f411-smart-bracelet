#include "start.h"
#include "oled.h"
#include "pic.h"

u8 RIGHT_LEFT=0;
void caidan(void)
{

	u8 fx=GET_KEY_fx();
	if(fx==4)
	{
		RIGHT_LEFT++;
		OLED_Clear(0,0,127,7);
	}
	else if(fx==3)
	{
		RIGHT_LEFT--;
		OLED_Clear(0,0,127,7);
	}
	RIGHT_LEFT=RIGHT_LEFT%5;
	
	switch(RIGHT_LEFT)
	{
		case 0:
			OLED_Dis_String(30,0,(const unsigned char *)RTC_time);
			OLED_Dis_String(20,2,(const unsigned char *)RTC_day);
			OLED_Dis_String(35,4,(const unsigned char *)RTC_week);
			break;
		case 1:
			OLED_DrawBMP(37,0,91,7, pic_table[0]);
			OLED_Dis_String(0,0,"心率");
			break;
		case 2:
			OLED_DrawBMP(37,0,91,7, pic_table[1]);
			OLED_Dis_String(0,0,"血压");
			break;
		case 3:
			OLED_DrawBMP(37,0,91,7, pic_table[2]);
			OLED_Dis_String(0,0,"温湿");
			break;
		case 4:
			OLED_DrawBMP(37,0,91,7, pic_table[3]);
			OLED_Dis_String(0,0,"步数");
			break;
	}
	
	
	
	
}

