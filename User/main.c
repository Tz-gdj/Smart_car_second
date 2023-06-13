#include "stm32f10x.h"
#include "Delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

static void prvSetupHardware( void );



int main(void)
{
	
	prvSetupHardware();


	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was not enough heap space to create the
	idle task. */
	return 0;
}
/*                                                         */




static void prvSetupHardware( void )
{
	
}
