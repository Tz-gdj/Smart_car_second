#ifndef UltrasonicSensor_H_
#define UltrasonicSensor_H_

void UltrasonicSensor_Init(void);
float Get_UTSensorValue(void);
void TIM1_UP_IRQHandler(void);


#endif
