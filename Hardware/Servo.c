#include "stm32f10x.h"
#include "PWM.h"

void Servo_Init(void)
{
    TIM2_PWM_Init();
}

void ServoLeft(void)
{
    TIM_SetCompare2(TIM2, 1000);
}

void ServoRight(void)
{
    TIM_SetCompare2(TIM2, 2000);
}

void ServoMid(void)
{
    TIM_SetCompare2(TIM2, 1500);
}