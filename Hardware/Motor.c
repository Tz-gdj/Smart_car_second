#include "stm32f10x.h"
#include "PWM.h"

void Motor_Init()
{
    TIM3_PWM_Init();

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void CarForward(void)
{
        GPIO_SetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_3);
        GPIO_ResetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_2);
}

void CarBackward(void)
{
        GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_2);
        GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_3);
}


/* 左右轮, left,right: 0-100 */
void CarLeft(int left, int right)
{   
    CarForward();
    TIM_SetCompare1(TIM3, left);
    TIM_SetCompare2(TIM3, right);
}

/* 左右轮, left,right: 0-100 */
void CarRight(int left, int right)
{   
    CarForward();
    TIM_SetCompare1(TIM3, left);
    TIM_SetCompare2(TIM3, right);
}

/* 左右轮, left,right: 0-100 */
void CarBackoff(int left, int right)
{
    CarBackward();
    TIM_SetCompare1(TIM3, left);
    TIM_SetCompare2(TIM3, right);
}