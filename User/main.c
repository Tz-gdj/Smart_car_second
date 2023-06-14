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
#include "UltrasonicSensor.h"
#include "Servo.h"

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

	ServoMid();

    EventGroupHandle = xEventGroupCreate();

    xTaskCreate(vForwardTask, "Forward", 1000, NULL, 1, NULL);
    xTaskCreate(vLeftTask, "Left", 1000, NULL, 1, NULL);
    xTaskCreate(vRightTask, "Right", 1000, NULL, 1, NULL);
    xTaskCreate(vTractionTask, "IRsensor", 1000, NULL, 1, NULL);
    xTaskCreate(vAvoidTask, "Avoid", 1000, NULL, 1, NULL);
    xTaskCreate(vAvoidForwardTask, "AvoidForward", 1000, NULL, 1, NULL);
    xTaskCreate(vAvoidStopTask, "AvoidStop", 1000, NULL, 2, NULL);
	xTaskCreate(蓝牙控制task, "蓝牙", 1000, NULL, 4, NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was not enough heap space to create the
	idle task. */
	return 0;
}
/*                                                         */




static void prvSetupHardware( void )
{
	Motor_Init();
	IRSensor_Init();
	UltrasonicSensor_Init();
	Servo_Init();
	
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
		/* 选择小车行动模式 */
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
	left = 20;
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
	right = 20;
    while(1)
    {
        xEventGroupWaitBits(EventGroupHandle, Right | Traction, Right, pdTRUE, portMAX_DELAY);

		CarContorl(left, right);

    }

}



/****************************************************
 	避障
*/

void vAvoidTask (void)
{
	float length = 0 ;
    while(1)
    {
        xEventGroupWaitBits(EventGroupHandle, Avoid, pdFALSE, pdTRUE, portMAX_DELAY);

		length = Get_UTSensorValue();
        if( length < 30)
        {
            xEventGroupSetBits(EventGroupHandle, UT_Stop);
        }
    }
}

void vAvoidForwardTask(void)
{
	int left, right;
	left = 80;
	right = 80;
    while(1)
    {
        xEventGroupWaitBits(EventGroupHandle, Avoid, pdFALSE, pdTRUE, portMAX_DELAY);
		
		CarContorl(left, right);
    }
}


void vAvoidStopTask(void)
{
	float UT_left = 0;
	float UT_right = 0; 
	float UT_middle = 0;
	int left = 80;
	int right = 80;
	int left_big = 60;
	int right_big = 60;
	int left_mid = 40;
	int right_mid = 40;
	int left_small = 20;
	int right_small = 20;
    while(1)
    {
        xEventGroupWaitBits(EventGroupHandle, UT_Stop, pdTRUE, pdTRUE, portMAX_DELAY);

		/* 停车并后退一段距离 */
		CarStop();
		Delay_ms(100);
		CarBackoff(left, right);
		Delay_ms(500); 

		/* 云台左右扫描，并选择转弯模式 */
        UT_middle = Get_UTSensorValue();
        ServoLeft();
        UT_left = Get_UTSensorValue();
        ServoRight();
        UT_right = Get_UTSensorValue();
		ServoMid();

        if(UT_middle < 10)
        {
            if(UT_left > UT_right) CarContorl(left, right_small);
            else CarContorl(left_small, right);
        } 
        else if(UT_middle < 20)
        {
            if(UT_left > UT_right) CarContorl(left, right_mid);
            else CarContorl(left_mid, right);
        }
        else
        {
            if(UT_left > UT_right) CarContorl(left, right_small);
            else CarContorl(left_small, right);
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