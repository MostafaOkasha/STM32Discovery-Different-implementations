//********************************** //-MOSTAFA OKASHA & SPENCER DEEVY - GROUP 13 -//>>>>[1]
/* Includes ---------------------------------------------------------------------*///|
#include "main.h" //All important files have been included.                        //|
#include "stm32f429i_discovery.h"																									 //|
#include "stm32f429i_discovery_lcd.h"																							 //|
#include <stdio.h>																																 //|
#include <stm32f4xx_rng.h>                                                         //|
																														                       //|
//***********************************************************************************>>>>[2]
//All the variables we will be using in this program are defined and explained below.|
/* Private variables ------------------------------------------------------------*///|
int user_button = 0;      //Checks if user button is pressed.                        |
int external_button1 = 0; //Check if first external button has been pressed.         |
int external_button2 = 0; //check if second external button has been pressed.        |
int external_button3 = 0; //check if third external button has been pressed.         |
int banana1 = 0;           //Dummy variable that might be useful later on.           |
int banana2 = 0;           //Dummy variable that might be useful later on.           |
int banana3 = 0;           //Dummy variable that might be useful later on.           |
int state = 0;            //Checks which count our program is in.                    |
int substate1 = 0;        //Checks if first external button is being held.           |
int substate2 = 0;        //Checks if second external button is being held.          |
int substate3 = 0;        //Checks if third external button is being held.           |
int substate = 0;         //Checks the substate of our program.                      |
int countup= 0;           //Counts for external button held to increase set temp.    |
int countdown = 0;        //Counts for external button held to decrease set temp.    |
uint16_t PrescalerValue = 0;     //Prescaler value.                	     						 |
uint8_t PrescalerValue_TIM3 = 0;//Prescaler value for tim3                           |
__IO uint16_t CCR1_Val = 31901;		 //value set to generate interrupt every 500ms       |
__IO uint8_t UBPressed = 0;      //Checks to see if the user button has been pressed.|
char lcd_buffer_1[14];           //Display Buffer 1 for LCD screen                   |
char lcd_buffer_2[14];           //Display Buffer 2 for LCD screen                   |
uint8_t view_speed[14];   				 //Buffer used to view the temp. and display it.   |
uint8_t set_speed[14];   				 //Buffer used to set the temp. and display it.      |
int16_t i = 0;
int16_t HalfSwitch = 0; //Variable to Switch CW & CCW in Half Steps
int16_t FullSwitch = 0; //Variable to Switch CW & CCW in Full Steps
int16_t TypeSwitch = 0; //Variable to Switch between Interupts & Polling
int16_t count = 2;      //Variable for increments
uint16_t intTIMCapture = 0; 

//Private function definitions:
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;                          					 //|
TIM_OCInitTypeDef  TIM_OCInitStructure;																						 //|
EXTI_InitTypeDef   EXTI_InitStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
EXTI_InitTypeDef   EXTI_InitStructure;
GPIO_InitTypeDef  GPIO_InitStructure;






void NVIC_Config(void);
void PB_Config(void);
void LED_Config(void);
void EXTILine3_Config(void);
void EXTILine2_Config(void);
void EXTILine1_Config(void);
void GPIO_Config(void);
void Timer_Config(void);

int main(void){             																								       //|
	/* LCD initiatization */																												 //|
		LCD_Init();																																		 //|
	/* LCD Layer initiatization */																									 //|
		LCD_LayerInit();																															 //|
	/* Enable the LTDC */																														 //|
		LTDC_Cmd(ENABLE);																															 //|
	/* Set LCD foreground layer */		      																				 //|							
		LCD_SetLayer(LCD_FOREGROUND_LAYER);																						 //|
		LED_Config();	
	/* Configure User Button */                                                      //|
		PB_Config();                                                                   //|
	/* 1st External Button Enable */                                                 //|
		EXTILine1_Config();                                                            //|
	/* 2nd External Button Enable */                                                 //|
		EXTILine2_Config();                                                            //|	
		/* 3rd External Button Enable */                                                 //|
		EXTILine3_Config();  
		NVIC_Config();
		GPIO_Config();
		Timer_Config();

		/* Set LCD Color and Display Introduction */                                     //|

		LCD_Clear(LCD_COLOR_WHITE);								                                       //|
					GPIO_SetBits(GPIOB, GPIO_Pin_5|GPIO_Pin_6);

				GPIO_ResetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_7);
	while(1){
		
		intTIMCapture = TIM_GetCounter( TIM3 );
 		if(intTIMCapture > CCR1_Val) {
			if (TypeSwitch == 1){		
				//Conditions for the 4 modes		
				if (count == 1){
				CCR1_Val = 31901;
				if (i == 8){i = 0;}
				}
				if (count == -1){
				CCR1_Val = 31901;
				if (i == 1){i = 9;}
				}
				if (count == 2){
				CCR1_Val = 63802;
				if (i == 8){i = 0;}
				}
				if (count == -2){
				CCR1_Val = 63802;
				if (i == 1){i = 9;}
				}
				//8 steps for the coils, with simulation of LED's			
				i = i + count;
				if(i == 1){
				GPIO_SetBits(GPIOB, GPIO_Pin_4);
				GPIO_ResetBits(GPIOB, GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
				TIM_SetCounter( TIM3, 0 );
				}
				if(i == 2){
				GPIO_SetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_6);
				GPIO_ResetBits(GPIOB, GPIO_Pin_5|GPIO_Pin_7);
				TIM_SetCounter( TIM3, 0 );
				}
				if(i == 3){
				GPIO_SetBits(GPIOB, GPIO_Pin_6);
				GPIO_ResetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7);
				TIM_SetCounter( TIM3, 0 );
				}
		 		if(i == 4){
				GPIO_SetBits(GPIOB, GPIO_Pin_5|GPIO_Pin_6);
				GPIO_ResetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_7);
				TIM_SetCounter( TIM3, 0 );
				}
				if(i == 5){
				GPIO_SetBits(GPIOB, GPIO_Pin_5);
				GPIO_ResetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7);
				TIM_SetCounter( TIM3, 0 );
				}
				if(i == 6){
				GPIO_SetBits(GPIOB, GPIO_Pin_5|GPIO_Pin_7);
				GPIO_ResetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_6);
				TIM_SetCounter( TIM3, 0 );
				}
				if(i == 7){
				GPIO_SetBits(GPIOB, GPIO_Pin_7);
				GPIO_ResetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);
				TIM_SetCounter( TIM3, 0 );
				}
				if(i == 8){
				GPIO_SetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_7);
				GPIO_ResetBits(GPIOB, GPIO_Pin_5|GPIO_Pin_6);
				TIM_SetCounter( TIM3, 0 );
				}
			}
		}
	}
}
// Configure the GPIO for output to the motor
void GPIO_Config(void){
	/* GPIOB_Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  /* Configure PB4, PB5, PB6, PB7 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
}
//Configure the TIM3 in output compare mode
void Timer_Config(void){
	/*
 * TIM3 Prescales System clock to 62500Hz
 * CCR1_Val = 28125;
 * 62500Hz/28125 = 2.222Hz = 0.45s/cycle
 * 96(halfsteps)*0.45s = 43s
 */
	  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 62500) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);
  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
  /* TIM Interrupts enable */
  TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}
void PB_Config(void){                                                              //|
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
}
void EXTILine1_Config(void){	//PB2																							   //|
		//Configure Pin PB2
		GPIO_InitTypeDef   GPIO_InitStructure; //Our constructors for button 1
		EXTI_InitTypeDef   EXTI_InitStructure;
		NVIC_InitTypeDef   NVIC_InitStructure;
		/* Enable GPIOA clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		/* Enable SYSCFG clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		// Configure pin 12 as input with internal pull up resister and  I/O speed at 50 MHz. (PB12)
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed= GPIO_Speed_100MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		//Configuration for interrupt at pin 12 port B.
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource2);
		EXTI_InitStructure.EXTI_Line = EXTI_Line2;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  //Interrupt at Rising edge (When button is pressed)
		EXTI_Init(&EXTI_InitStructure); //Creates an interrupt handler
		/* Enable and set EXTI Line1 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //Set priority to highest priority.
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; //Set sub priotiy to lowest priority.
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //Turn on the handler
		NVIC_Init(&NVIC_InitStructure); //Creates the interrupt
}
void EXTILine2_Config(void){ 	//PB12																							 //|

		//Configure Pin PB12
		GPIO_InitTypeDef   GPIO_InitStructure; //Our constructors for button 1
		EXTI_InitTypeDef   EXTI_InitStructure;
		NVIC_InitTypeDef   NVIC_InitStructure;
		/* Enable GPIOA clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		/* Enable SYSCFG clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		// Configure pin 12 as input with internal pull up resister and  I/O speed at 50 MHz. (PB12)
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed= GPIO_Speed_100MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		//Configuration for interrupt at pin 10 port B.
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);
		EXTI_InitStructure.EXTI_Line = EXTI_Line12;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  //Interrupt at Rising edge (When button is pressed)
		EXTI_Init(&EXTI_InitStructure); //Creates an interrupt handler
		/* Enable and set EXTI Line1 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; //For pins 10 - 15 you use EXTI15_10.
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //Set priority to highest priority.
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; //Set sub priotiy to lowest priority.
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //Turn on the handler
		NVIC_Init(&NVIC_InitStructure); //Creates the interrupt
}


void EXTILine3_Config(void){ 	//PB1																							 //|

		//Configure Pin PB1
		GPIO_InitTypeDef   GPIO_InitStructure; //Our constructors for button 1
		EXTI_InitTypeDef   EXTI_InitStructure;
		NVIC_InitTypeDef   NVIC_InitStructure;
		/* Enable GPIOA clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		/* Enable SYSCFG clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		// Configure pin 12 as input with internal pull up resister and  I/O speed at 50 MHz. (PB12)
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed= GPIO_Speed_100MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		//Configuration for interrupt at pin 1 port B.
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
		EXTI_InitStructure.EXTI_Line = EXTI_Line1;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  //Interrupt at Rising edge (When button is pressed)
		EXTI_Init(&EXTI_InitStructure); //Creates an interrupt handler
		/* Enable and set EXTI Line1 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //Set priority to highest priority.
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; //Set sub priotiy to lowest priority.
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //Turn on the handler
		NVIC_Init(&NVIC_InitStructure); //Creates the interrupt
}
void LED_Config(void){                                                             //|
	  /* Initialize Leds mounted on STM32F4-Discovery board */
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
}
void NVIC_Config(void){
	NVIC_InitTypeDef NVIC_InitStructure;
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
//---~~~~~~~~~~~~~END~~~~~~~~~~~~~---\\