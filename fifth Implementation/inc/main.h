#include "stm32f4xx_it.h"
#include "stm32f4xx_tim.h"
#include "stm32f429i_discovery.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_i2c_ee.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#define REV_TIME	67 //in seconds
#define MOTOR_GPIO	GPIOD
#define MOTOR_GPIO_RCC	RCC_AHB1Periph_GPIOD

extern GPIO_TypeDef* GPIO_PORT[LEDn];
extern const uint16_t GPIO_PIN[LEDn];
extern const uint32_t GPIO_CLK[LEDn];


