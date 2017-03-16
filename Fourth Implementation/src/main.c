//********************************** //-MOSTAFA OKASHA & SPENCER DEEVY - GROUP 13 -//>>>>[1]
/* Includes ---------------------------------------------------------------------*///|
#include "main.h" //All important files have been included.                        //|
#include "stm32f429i_discovery.h"																									 //|
#include "stm32f429i_discovery_lcd.h"																							 //|
#include <stdio.h>																																 //|
#include <stm32f4xx_rng.h>                                                         //|
//***********************************************************************************>>>>[2]
//All the variables we will be using in this program are defined and explained below.|
/* Private variables ------------------------------------------------------------*///|
int external_button1 = 0; //Check if first external button has been pressed.         |
int external_button2 = 0; //check if second external button has been pressed.        |
int banana1 = 0;           //Dummy variable that might be useful later on.           |
int banana2 = 0;           //Dummy variable that might be useful later on.           |
int state = 0;            //Checks which count our program is in.                    |
int counter0 = 150000;       //Checks which count our program is in.                 |
int fan = 0;              //This value will be used to set the fan speed.            |
int counter1 = 0;         //Checks which count our program is in.                    |
int counter2 = 0;         //Checks which count our program is in.                    |
int counter3 = 0;         //Checks which count our program is in.                    |
int counter4 = 0;         //Checks which count our program is in.                    |
int substate = 0;         //Checks the substate of our program.                      |
int substate1 = 0;        //Checks if first external button is being held.           |
int substate2 = 0;        //Checks if second external button is being held.          |
int user_button = 0;      //Checks if user button is pressed.                        |
int countup= 0;           //Counts for external button held to increase set temp.    |
int countdown = 0;        //Counts for external button held to decrease set temp.    |
double temperature = 0.0;      //This will be the value we use to calculate temp.    |
double settemperature = 0.0;   //This will be the value we use to set the   temp.    |
float tempdiff = 0;      //This value will be usedto calculate temp difference.      |
double ADC3ConvertedTemperature = 0.0;                   										       //|
uint16_t temp1 = 0;        //Dummy variable 1                                        |
uint16_t temp2 = 0;        //Dummy variable 2                                        |
uint16_t PrescalerValue = 0;     //Prescaler value.                	     						 |
uint8_t PrescalerValue_TIM3 = 0;//Prescaler value for tim3                           |
uint8_t Fan_Speed = 0;                                                             //|
__IO uint16_t ADC3ConvertedValue = 0;        	          													 //|
__IO uint16_t CCR1_Val = 250;		 //value set to generate interrupt every 500ms       |
__IO uint8_t UBPressed = 0;      //Checks to see if the user button has been pressed.|
char lcd_buffer_1[14];           //Display Buffer 1 for LCD screen                   |
char lcd_buffer_2[14];           //Display Buffer 2 for LCD screen                   |
uint8_t view_temp[14];   				 //Buffer used to view the temp. and display it.     |
uint8_t set_temp[14];   				 //Buffer used to set the temp. and display it.      |
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;                          					 //|
TIM_OCInitTypeDef  TIM_OCInitStructure;																						 //|
EXTI_InitTypeDef   EXTI_InitStructure;																						 //|
ADC_InitTypeDef       ADC_InitStructure;																					 //|
ADC_CommonInitTypeDef ADC_CommonInitStructure;																		 //|
DMA_InitTypeDef       DMA_InitStructure;																					 //|
GPIO_InitTypeDef      GPIO_InitStructure;																					 //|
//***********************************************************************************>>>>[3]
/* Private define ---------------------------------------------------------------*///|
#define ADC3_DR_ADDRESS    ((uint32_t)0x4001224C) //ADC Address        							 |
#define ADC_CDR_ADDR 				((uint32_t)(ADC_BASE + 0x08)) //ADC address              |
//***********************************************************************************>>>>[4]
/* Private function prototypes --------------------------------------------------*///|
void ADC3_CH12_DMA_Config(void);	//This is the ADC config function  on channel 12   |
void PWM_Config(void);            //Function for configuring the PMW in this program |
void EXTILine1_Config(void);			//First external Button function                   |
void PB_Config(void);             //User Button Function      											 |
void EXTILine2_Config(void);			//Second external Button function                  |
void BUT1ON(void);								//Function that is called when ext BUT1 is pressed |
void BUT2ON(void);								//Function that is called when ext BUT2 is pressed |
void BUT1OFF(void);								//Function that is called when ext BUT1 is released|
void BUT2OFF(void);								//Function that is called when ext BUT2 is released|
void LED_Config(void);            //Function that initiates the LEDs on the board    |
void TIM2_Config(void);           //Function that is used to enable TIM2             |
void GPIO_Config(void);           //Function that is used to enable GPIO pins        |
void NVIC_Config(void);           //Enable NVIC                                      |
//***********************************************************************************>>>>[5]
/* $_$~Main~$_$ -----------------------------------------------------------------*///|
int main(void){																												 						 //|
	/* System init */               																								 //|
	/* LCD initiatization */																												 //|
		LCD_Init();																																		 //|
	/* LCD Layer initiatization */																									 //|
		LCD_LayerInit();																															 //|
	/* Enable the LTDC */																														 //|
		LTDC_Cmd(ENABLE);																															 //|
	/* Set LCD foreground layer */		      																				 //|							
		LCD_SetLayer(LCD_FOREGROUND_LAYER);																						 //|
		LED_Config();																																	 //|
	/* Initialize GPIO */																														 //|
		GPIO_Config(); 																																 //|
	/* Initialize TIM2 Clock */																											 //|
		TIM2_Config();                                                                 //|
	/* Initialize NVIC */ 																													 //|
		NVIC_Config();  																						    							 //|
	/* Enable PWM */																																 //|
		PWM_Config();                                                                  //|
	/* configure ADC device */                                                       //|
		ADC3_CH12_DMA_Config();                                                        //|
	/* Start ADC3 Software Conversion */                                             //|
		ADC_SoftwareStartConv(ADC3);                                                   //|
	/* Configure User Button */                                                      //|
		PB_Config();                                                                   //|
	/* 1st External Button Enable */                                                 //|
		EXTILine1_Config();                                                            //|
	/* 2nd External Button Enable */                                                 //|
		EXTILine2_Config();                                                            //|
	/* Set LCD Color and Display Introduction */                                     //|
		LCD_Clear(LCD_COLOR_RED);								                                       //|
		LCD_SetBackColor(LCD_COLOR_RED);				                                       //|
		LCD_SetTextColor(LCD_COLOR_WHITE);				                                     //|
		LCD_DisplayStringLine(LINE(0),  (uint8_t *) "     LAB-4     ");                //|
		LCD_DisplayStringLine(LINE(1),  (uint8_t *) "   GROUP: 13   ");                //|
		LCD_DisplayStringLine(LINE(2),  (uint8_t *) "PWM Fan control");                //|
		LCD_DisplayStringLine(LINE(3),  (uint8_t *) "---------------");                //|
			ADC3ConvertedTemperature = ADC3ConvertedValue *100.0/4095.0;								 //|
			settemperature = ADC3ConvertedTemperature - 1;				  										 //|
			sprintf((char*)set_temp,"SET = %.2lf C*",settemperature);			               //|
			LCD_DisplayStringLine(LINE(8),set_temp);					  												 //|
			LCD_DisplayStringLine(LINE(10),(uint8_t *)"   EXT-B1 ++");					  			 //|
			LCD_DisplayStringLine(LINE(11),(uint8_t *)"   EXT-B2 --");					  		   //|
//***********************************************************************************>>>>[6]
/* Infinite While Loop ----------------------------------------------------------*///|
	while(1){																																				 //|
		if (counter0 == 150000){   //We do this in order to not rapidly display data   //|
			ADC3ConvertedTemperature = ADC3ConvertedValue *100.0/4095.0;								 //|
			temperature = ADC3ConvertedTemperature;						  												 //|
			sprintf((char*)view_temp,"  T = %.2lf C*",temperature);			                 //|
			LCD_DisplayStringLine(LINE(6),view_temp);					  												 //|
			counter0 = 0;				  																											 //|
		}				  																																		 //|
		else{				  																																 //|
			counter0 = counter0 + 1;				  																					 //|
		}				  																																		 //|
/*-------------------------------------------------------------------------------*///|
			if (external_button1 == 1){																			  					 //|
					BUT1ON();              																								   //|
	}																																							   //|
			if (banana1 == 1){																											     //|
					BUT1OFF();																														   //|
	}														  																									 //|
																																								   //|
/*-------------------------------------------------------------------------------*///|
			if (external_button2 == 1){																			  					 //|
					BUT2ON();              																								   //|
	}																																							   //|
			if (banana2 == 1){																											     //|
					BUT2OFF();																														   //|
	}														  																									 //|
																																								   //|
/*-------------------------------------------------------------------------------*///|
		tempdiff = temperature - settemperature; //Calculate temp difference           //|
		if (tempdiff <= 0){    																												 //|
			STM_EVAL_LEDOn(LED3);              																			     //|
			STM_EVAL_LEDOff(LED4);                  																		 //|
			fan = 0;              																											 //|
		}                  																														 //|
		else{                   																											 //|
			STM_EVAL_LEDOn(LED4);              																			     //|
			STM_EVAL_LEDOff(LED3);              																		     //|
			if (tempdiff < 1){																												   //|
				fan = 100;                                                                 //|
			}                                                                            //|
			else if (tempdiff < 2){																										   //|
				fan = 200;                                                                 //|
			}                                                                            //|
			else if (tempdiff < 3){																										   //|
				fan = 300;                                                                 //|
			}                                                                            //|
			else if (tempdiff < 4){																										   //|
				fan = 400;                                                                 //|
			}                                                                            //|
			else{			                  																							   //|
				fan = 500;                                                                 //|
			}                                                                            //|
		}                   																													 //|
			/* PWM1 Mode configuration: Channel1 */																			 //|
	//This code is run everytime and depending on the value of fan, our fan will     //|
	//Spin with specific power depending on the temperature differemce.              //|
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;															 //|
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;									 //|
		TIM_OCInitStructure.TIM_Pulse = fan;																					 //|
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;											 //|
		TIM_OC1Init(TIM3, &TIM_OCInitStructure);																			 //|
/*-------------------------------------------------------------------------------*///|
 } //END of While Loop
} //END OF MAIN
//***********************************************************************************>>>>[7]
/*-------------------------------------------------------------------------------*///|
void PB_Config(void){                                                              //|
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
}
/*-------------------------------------------------------------------------------*///|
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
/*-------------------------------------------------------------------------------*///|
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
/*-------------------------------------------------------------------------------*///|
void BUT1ON(void){                                                                 //|
		if (substate1 == 0){
			LCD_DisplayStringLine(LINE(4),  (uint8_t *) "INCREASING SET");
			substate1 = 1;
		}
		counter1 = counter1 + 1;
		if (counter1 == 150000){
			settemperature = settemperature + 1;
			counter1 = 0;
		}
}
/*-------------------------------------------------------------------------------*///|
void BUT2ON (void){                                                                //|
		if (substate2 == 0){
			LCD_DisplayStringLine(LINE(5),  (uint8_t *) "DECREASING SET");
			counter2 = 0;
			substate2 = 1;
		}
		counter2 = counter2 + 1;
		if (counter2 == 150000){
			settemperature = settemperature - 1;
			counter2 = 0;
		}
}
/*-------------------------------------------------------------------------------*///|
void BUT1OFF (void){                                                               //|
		LCD_DisplayStringLine(LINE(4),  (uint8_t *) "                ");
		external_button1 = 0;
		substate1 = 0;
		counter1 = 0;
		banana1 = 0;
	/*-------------------------------------------------------------------------------*///|
		//This will update the set temp displayed of settemperature                    //|
			sprintf((char*)set_temp,"SET = %.2lf C*",settemperature);			               //|
			LCD_DisplayStringLine(LINE(8),set_temp);					  									  		 //|
}
/*-------------------------------------------------------------------------------*///|
void BUT2OFF (void){                                                               //|
		LCD_DisplayStringLine(LINE(5),  (uint8_t *) "                 ");
		external_button2 = 0;
		substate2 = 0;
		counter2 = 0;
		banana2 = 0;
	/*-------------------------------------------------------------------------------*///|
		//This will update the set temp displayed of settemperature                    //|
			sprintf((char*)set_temp,"SET = %.2lf C*",settemperature);			               //|
			LCD_DisplayStringLine(LINE(8),set_temp);					  									  		 //|
}
/*-------------------------------------------------------------------------------*///|
void LED_Config(void){                                                             //|
	  /* Initialize Leds mounted on STM32F4-Discovery board */
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
}
/*-------------------------------------------------------------------------------*///|
void ADC3_CH12_DMA_Config(void){                                                   //|
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
  /* Enable ADC3, DMA2 and GPIO clocks ****************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
  /* DMA2 Stream0 channel0 configuration **************************************/
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC3ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 14;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);
  /* Configure ADC3 Channel12 pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);
  /* ADC3 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC3, &ADC_InitStructure);
  /* ADC3 regular channel12 configuration *************************************/
  ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 1, ADC_SampleTime_3Cycles);
 /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
  /* Enable ADC3 DMA */
  ADC_DMACmd(ADC3, ENABLE);
  /* Enable ADC3 */
  ADC_Cmd(ADC3, ENABLE);
}
/*-------------------------------------------------------------------------------*///|
void NVIC_Config(void){                                                            //|
	NVIC_InitTypeDef NVIC_InitStructure;
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/*-------------------------------------------------------------------------------*///|
void TIM2_Config(void){                                                            //|
 /* Compute the prescaler value */
  PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 500000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);
  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
  /* TIM Interrupts enable */
  TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM2, ENABLE);
}
/*-------------------------------------------------------------------------------*///|
void PWM_Config(void){                                                             //|
	/* Compute the prescaler value */
  PrescalerValue_TIM3 = (uint16_t) ((SystemCoreClock /2) / 28000000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 665;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue_TIM3;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM3, ENABLE);
/* TIM Interrupts enable */
  TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}
/*-------------------------------------------------------------------------------*///|
void GPIO_Config(void){                                                            //|
  GPIO_InitTypeDef GPIO_InitStructure;
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  /* GPIOC and GPIOB clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  /* GPIOC Configuration: TIM3 CH1 (PB4) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
	/* Connect TIM3 pins to AF2 */  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
}
/*-------------------------------------------------------------------------------*///|
//***********************************************************************************>>>>[END]
