/*
 * sleep.h
 *
 *  Created on: 2019��5��13��
 *      Author: Administrator
 */

#ifndef SOURCE_SLEEP_H_
#define SOURCE_SLEEP_H_

#include "system.h"


#define SleepCurrent    1000//0xFFFF-1000//0x8020 //��ʱ�޸�Ϊ400



void IO_SLEEP(void);
void IO_WAKEUP(void);
void SleepMode(void);
void SleepCheck(void);

#endif /* SOURCE_SLEEP_H_ */
