#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

/* Library includes. */
#include "stm32f10x_it.h"
#include "stm32f10x.h"
/* Demo app includes. */

#include "BlockQ.h"
#include "death.h"
#include "integer.h"
#include "blocktim.h"
#include "partest.h"
#include "semtest.h"
#include "PollQ.h"
#include "flash.h"
#include "comtest2.h"


//--------------------------------------------------------------
// 事件组
#define Forward 1<<0
#define Left 1<<1
#define Right 1<<2
#define UT_Stop 1<<3
#define Traction 1<<4
#define Avoid 1<<5
#define Bluetooth 1<<6


//---------------------------------------------------------------
static EventGroupHandle_t EventGroupHandle;

int main_fake(void)
{
    
    EventGroupHandle = xEventGroupCreate();

    xTaskCreate(直行task, "直行", 1000, NULL, 1, NULL);
    xTaskCreate(左转task, "左转", 1000, NULL, 1, NULL);
    xTaskCreate(右转task, "右转", 1000, NULL, 1, NULL);
    xTaskCreate(红外task, "红外", 1000, NULL, 1, NULL);
    xTaskCreate(超声波侦测task, "超声波", 1000, NULL, 1, NULL);
    xTaskCreate(蓝牙控制task, "蓝牙", 1000, NULL, 4, NULL);
    xTaskCreate(超声波直行task, "超声波直行", 1000, NULL, 1, NULL);\
    xTaskCreate(前方有障碍_停车task, "停车", 1000, NULL, 2, NULL);
}   


// 循迹


void 红外task(void)
{
    xEventGroupWaitBits(EventGroupHandle, Traction, pdFALSE, pdTRUE, portMAX_DELAY);

    int flags_left, flags_right;
    while(1)
    {
        flags_left = 左红外();
        flags_right = 右红外();
        if(flags_left == 1 & flags_right == 1)
        {
            xEventGroupSetBits(EventGroupHandle, Forward);
        }
        else if(flags_left == 0 & flags_right == 1)
        {
            xEventGroupSetBits(EventGroupHandle, Left);
        }
        else if(flags_left ==1 & flags_right == 0)
        {
            xEventGroupSetBits(EventGroupHandle, Right);
        }
    }
}

void 直行task (void)
{
    while(1)
    {
        左前轮( 50 );
        右前轮( 50 );

        xEventGroupWaitBits(EventGroupHandle, Forward | Traction, Forward, pdTRUE, portMAX_DELAY);
    }

}

void 左转task ()
{
    while(1)
    {
        左前轮(0);
        右前轮(100);

        xEventGroupWaitBits(EventGroupHandle, Left  | Traction, Left, pdTRUE, portMAX_DELAY);
    }
}

void 右转task (void)
{
    while(1)
    {
        左前轮(100);
        右前轮(0);

        xEventGroupWaitBits(EventGroupHandle, Right | Traction, Right, pdTRUE, portMAX_DELAY);
    }

}



// 避障

void 超声波侦测task (void)
{
    while(1)
    {
        xEventGroupWaitBits(EventGroupHandle, Avoid, pdFALSE, pdTRUE, portMAX_DELAY);

        if()
        {
            GPIO_WriteBit(1) = 1;
            delay_us(20);
            GPIO_WriteBit(1) = 0;

            while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == RESET);
            
            xEventGroupSetBits(EventGroupHandle, UT_Stop);
        }
    }
}

void 超声波直行task(void)
{
    while(1)
    {
        左前轮(100);
        右前轮(100);
        xEventGroupWaitBits(EventGroupHandle, Avoid, pdFALSE, pdTRUE, portMAX_DELAY);
    }
}


void 前方有障碍_停车task(void)
{
    while(1)
    {
        xEventGroupWaitBits(EventGroupHandle, UT_Stop, pdTRUE, pdTRUE, portMAX_DELAY);
        int x,y,z;
        int left, right;
        x = UT_Distance();
        云台左转();
        y = UT_Distance();
        云台右转();
        z = UT_Distance();
        if(x < 10)
        {
            if(y > z) 左大转();
            else 右大转();
        } 
        else if(x < 20)
        {
            if(y > z) 左中转();
            else 右中转();
        }
        else
        {
            if(y > z) 左小转();
            else 右小转();
        }
    }
}


// 蓝牙控制


void SetMode(void)
{
    int 蓝牙参数, 避障, 寻路, 蓝牙;
    if(蓝牙参数 = 避障) 
    {
        xEventGroupSetBits(EventGroupHandle, Traction);
        xEventGroupClearBits(EventGroupHandle, Avoid | Bluetooth);
    }
    else if(蓝牙参数 = 寻路)
    {
        xEventGroupSetBits(EventGroupHandle, Avoid);
        xEventGroupClearBits(EventGroupHandle, Traction | Bluetooth);
    }
    else if(蓝牙参数 = 蓝牙)
    {
        xEventGroupSetBits(EventGroupHandle, Bluetooth);
        xEventGroupClearBits(EventGroupHandle, Avoid | Traction);
    }

}



void 蓝牙控制task (void)
{

}

void OLED_task(void)
{
    /* 使用Queue传数据 */
}