#ifndef __MOTOR_H_
#define __MOTOR_H_

void Motor_Init();
void CarForward(void);
void CarBackward(void);
void CarLeft(int left, int right);
void CarRight(int left, int right);
void CarBackoff(int left, int right);
#endif
