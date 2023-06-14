#include "stm32f10x.h"


void IRSensor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

int IRSensor_Left(void)
{
    int input = 0;
    input = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11);

    return input;
}

int IRSensor_Right(void)
{
    int input = 0;
    input = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12);

    return input;
}

