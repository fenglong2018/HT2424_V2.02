/*
 * sleep.h
 *
 *  Created on: 2019年5月13日
 *      Author: Administrator
 */

#ifndef SOURCE_SLEEP_H_
#define SOURCE_SLEEP_H_

#include "system.h"


#define SleepCurrent    1000//0xFFFF-1000//0x8020 //临时修改为400



void IO_SLEEP(void);
void IO_WAKEUP(void);
void SleepMode(void);
void SleepCheck(void);

#endif /* SOURCE_SLEEP_H_ */
