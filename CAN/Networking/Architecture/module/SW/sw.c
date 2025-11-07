#include "sw.h"
#define NOP() asm volatile("nop" ::: )

void GPIO_SW_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_RESET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
}
void SW_Release(void)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_SET);
    Delay_Ms(20);
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
    Delay_Ms(20);
    // GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_RESET);
}

void SW_Catch(void)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
}
//2812
void delay_320ns()
{
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    //NOP();NOP();NOP();NOP();NOP();NOP();
    //NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    //NOP();NOP();NOP();NOP();NOP();NOP();
}
void delay_850ns()
{
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    //NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    //NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    //NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    //NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
}
void delay_700ns()
{
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
}
void delay_600ns()
{
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();NOP();
}

void delay_2812RES()
{
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
}

void delay_us(unsigned int i)
{

    while(i--)
    {
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();;
    }
}
void GPIO_Init2812()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
}
void WS2812B_Reset(void)          //复位函数
{
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
	Delay_Us(280);
}
void ws2812_write_byte(u8 dat)
{
	u8 i = 0;

	for(i = 0; i < 8; i++)
	{
		if((dat & 0x80) == 0x80)
		{
			GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
			delay_850ns();
			GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
			delay_320ns();
		}
		else
		{
			GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
			delay_320ns();
			GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
			delay_850ns();
		}
		dat = dat << 1;
	}
    
}
/*---------------------------------------------------------------------------
@Function   :WS2812B_WriteColor
@Description:写入1个24bit颜色数据
@Input      :无
@Retrun     :无
@Others     :
----------------------------------------------------------------------------*/
void WS2812B_WriteColor( unsigned char C_R,unsigned char C_G,unsigned char C_B)
{
	ws2812_write_byte(C_G);
	ws2812_write_byte(C_R);
	ws2812_write_byte(C_B);
}


//gpio
void yuyin_gpio()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    //A
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
    //B
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);

    //C
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_SET);
    GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_SET);
    GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
    GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_SET);
}

void RGB_huxi(unsigned char mode)
{
    static unsigned char jici=1;
    jici++;
    if(jici<=15)//亮度加
    {
        if(mode)//黄灯
        {
            WS2812B_Reset();
            WS2812B_WriteColor(jici*10,jici*6,0);
        }
        else if (mode==0)
        {
            WS2812B_Reset();
            WS2812B_WriteColor(jici*10,jici*10,jici*10);
        }
    }
    else//亮度减
    {
        if(mode)
        {
            WS2812B_Reset();
            WS2812B_WriteColor(150-(jici-15)*10,90-(jici-15)*6,0);
        }
        else if (mode==0)
        {
            WS2812B_Reset();
            WS2812B_WriteColor(150-(jici-15)*10,150-(jici-15)*10,150-(jici-15)*10);
        }
        if(jici>=29)
        {
            jici=1;
        }
    }
}
///
void GPIO_fan_flame_light()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    //B
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
    //C
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOC, GPIO_Pin_5, Bit_RESET);

}