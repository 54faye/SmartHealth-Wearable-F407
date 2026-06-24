#ifndef __SETTIME_H
#define __SETTIME_H

#include <stdint.h>

void SetTime_Init(void);
uint8_t SetTime_HandleKey(uint8_t KeyNum);
void SetTime_RefreshDisplay(void);

#endif
