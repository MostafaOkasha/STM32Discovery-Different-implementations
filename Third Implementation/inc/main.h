#include "stm32f4xx_it.h"
#include "stm32f4xx_tim.h"
#include "stm32f429i_discovery.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_i2c_ee.h"

extern RTC_InitTypeDef RTC_InitStructure;
extern RTC_TimeTypeDef RTC_TimeStructure;
extern RTC_DateTypeDef RTC_DateStructure;

extern __IO uint16_t memLocation;

void RTC_Config(void);
void PB_Config(void);
void EXTButton_1(void);
void EXTButton_2(void);
