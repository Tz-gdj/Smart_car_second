#include "stm32f10x.h"
#include "Delay.h"

uint16_t overcount = 0;

void UltrasonicSensor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);


    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    TIM_DeInit(TIM1);
    TIM_InternalClockConfig(TIM1);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1; //具体的us值
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; // 1ms
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);


    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

}



float Get_UTSensorValue(void)
{
    float length = 0;
    uint16_t count = 0;
    float sum = 0;
    uint8_t i;

    for (i = 0; i < 5; i++)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_8);
        Delay_us(20);
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);

        while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == RESET);
        TIM_Cmd(TIM1, ENABLE);

        while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == SET);
        TIM_Cmd(TIM1, DISABLE);

        count = TIM_GetCounter(TIM1);
        length = (count + (overcount * 100)) / 58.0;    // us/58 = 厘米

        sum += length;
        TIM1->CNT = 0;
        overcount = 0;
        Delay_ms(10);
    }          
    length = sum / 5;

    return length;
}






void TIM1_UP_IRQHandler(void)
{
    if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) == SET)
    {
        overcount++;
        TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);
    }
}