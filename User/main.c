#include "stm32f10x.h"

/* System */
#include "Delay.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

/* Hardware */
#include "Motor.h"
#include "IRSensor.h"

static void prvSetupHardware( void );

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


int main(void)
{
	
	prvSetupHardware();

    EventGroupHandle = xEventGroupCreate();

    xTaskCreate(vForwardTask, "Forward", 1000, NULL, 1, NULL);
    xTaskCreate(vLeftTask, "Left", 1000, NULL, 1, NULL);
    xTaskCreate(vRightTask, "Right", 1000, NULL, 1, NULL);
    xTaskCreate(vTractionTask, "IRsensor", 1000, NULL, 1, NULL);
    xTaskCreate(超声波侦测task, "超声波", 1000, NULL, 1, NULL);
    xTaskCreate(蓝牙控制task, "蓝牙", 1000, NULL, 4, NULL);
    xTaskCreate(超声波直行task, "超声波直行", 1000, NULL, 1, NULL);\
    xTaskCreate(前方有障碍_停车task, "停车", 1000, NULL, 2, NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was not enough heap space to create the
	idle task. */
	return 0;
}
/*                                                         */




static void prvSetupHardware( void )
{
	TIM3_PWM_Init();
	Motor_Init();
	IRSensor_Init();
}



/*********************************************************************
	循迹
*/


void vTractionTask(void)
{
    xEventGroupWaitBits(EventGroupHandle, Traction, pdFALSE, pdTRUE, portMAX_DELAY);

    int IRsensor_left = 0;
	int IRsensor_right = 0;
    while(1)
    {
        IRsensor_left = IRSensor_Left();
        IRsensor_right = IRSensor_Right();
        if(IRsensor_left == 0 & IRsensor_right == 0)
        {
            xEventGroupSetBits(EventGroupHandle, Forward);
        }
        else if(IRsensor_left == 1 & IRsensor_right == 0)
        {
            xEventGroupSetBits(EventGroupHandle, Left);
        }
        else if(IRsensor_left == 0 & IRsensor_right == 1)
        {
            xEventGroupSetBits(EventGroupHandle, Right);
        }
    }
}

void vForwardTask (void)
{
	int left, right;
	left = 50;
	right = 50;
    while(1)
    {
       xEventGroupWaitBits(EventGroupHandle, Forward | Traction, Forward, pdTRUE, portMAX_DELAY);
	
	   CarContorl(left, right);
    }

}

void vLeftTask ()
{
	int left, right;
	left = 0;
	right = 80;
    while(1)
    {
        xEventGroupWaitBits(EventGroupHandle, Left  | Traction, Left, pdTRUE, portMAX_DELAY);
		
		CarContorl(left, right);
    }
}

void vRightTask (void)
{
	int left, right;
	left = 80;
	right = 0;
    while(1)
    {
        xEventGroupWaitBits(EventGroupHandle, Right | Traction, Right, pdTRUE, portMAX_DELAY);

		CarContorl(left, right);

    }

}



/****************************************************
 	避障
*/

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


/**********************************************************
 	蓝牙
*/


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