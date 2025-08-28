#include "sw.h"

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
    Delay_Ms(10);
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
    Delay_Ms(10);
    GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_RESET);
}

void SW_Catch(void)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
}