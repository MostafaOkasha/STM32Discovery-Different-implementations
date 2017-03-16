/**
  ******************************************************************************
  * @file    Lab2/main.c
  * @author  Robert Li, modified by Keybo Q.
  * @version 
  * @date    August 2014
  * @brief   this starter project demonstrates: 
						1. timer configuration
						2. timer output compare interrupt set up
						3. LCD configuration
						4. EEPROM emulator config
						5. RNG config
	* @projectfunction  pressing the RESET BUTTON:some strings, chars, and numbers will be displayed on LCD, and LED3 will be on, LED4 will blink
											Each time when the USER BUTTON is pressed:  
												1. LED3 will toggle 
												2. LED4 will toggle the blink rate
												3. "hello world" will move down one line
												4. a new random number will be displayed on line 12
												5  an increasing count number written to and read from EEPROM will be displayed on line 13, the last line.
  
  ******************************************************************************
 * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h" 
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "eeprom.h"
#include <stdio.h>
#include <stm32f4xx_rng.h>
#include "stm32f429i_discovery_sdram.h"  //we added all the files which would be needed in main.c.
// And continued to unclude all the functions and variabls that would be needed to connect the program together.

/* Private typedef -----------------------------------------------------------*/
EXTI_InitTypeDef   EXTI_InitStructure; // This line of code was added from the interrupt example.
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
/* Private define ------------------------------------------------------------*/
#define COLUMN(x) ((x) * (((sFONT *)LCD_GetFont())->Width))    //see font.h, for defining LINE(X)
/* Private macro -------------------------------------------------------------*/
__IO uint32_t TimingDelay;
/* Private variables ---------------------------------------------------------*/

__IO uint16_t CCR1_Val = 500;
uint16_t PrescalerValue = 0;
__IO uint8_t UBPressed = 0;
__IO uint8_t EBPressed = 0;
uint16_t line; // This would later be used to determine where the values would be placed onthe LCD screen.
char lcd_buffer[14];    // LCD display buffer
/* Private variables END ---------------------------------------------------------*/
/* Virtual address defined by the user: 0xFFFF value is prohibited
 * This global variable stores the EEPROM addresses for NB_OF_VAR(=3) variables
 * where the user can write to. To increase the number of variables written to EEPROM
 * modify this variable and NB_OF_VAR in eeprom.h
 */
uint16_t VirtAddVarTab2[NB_OF_VAR] = {0x5555, 0x6666, 0x7777};
// No need to redefine VirtAddVarTab as it was previously defined in stm32f4xx_it.c
/* Private function prototypes -----------------------------------------------*/
void PB_Config(void); //Here we are to initiate our Functions.
void TIM_Config(void);
void EXTILine1_Config(void);
void LED_Config(void);
void TIM3_Config(void);
void TIM3_OCConfig(void);
//void EXTILine1_Config(void);

//static void EXTILine0_Config(void); // this function and the one below was added from the interrupt example also.
//static void EXTILine13_Config(void); // see above comment.

void LCD_DisplayString(uint16_t LineNumber, uint16_t ColumnNumber, uint8_t *ptr);
void LCD_DisplayInt(uint16_t LineNumber, uint16_t ColumnNumber, int Number);
void LCD_DisplayFloat(uint16_t LineNumber, uint16_t ColumnNumber, float Number, int DigitAfterDecimalPoint);
void RNG_Config(void);
void Configure_PB1(void);
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void) //This is our main function that will be run at the start of the code.
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */
	
//initiate user button
		PB_Config();
	//Initialize random number generator.
		RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
		RNG_Cmd(ENABLE);
	//initiate LEDs and turn them on
		LED_Config();	
	//initiate the external button.
		Configure_PB1();

  /* -----------------------------------------------------------------------
    TIM3 Configuration: Output Compare Timing Mode:
    
    In this example TIM3 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM3CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM3CLK = HCLK / 2 = SystemCoreClock /2
          
    To get TIM3 counter clock at 50 MHz, the prescaler is computed as follows:
       Prescaler = (TIM3CLK / TIM3 counter clock) - 1
       Prescaler = ((SystemCoreClock /2) /0.5 MHz) - 1
                                              
    CC1 update rate = TIM3 counter clock / CCR1_Val = 10.0 Hz
    ==> Toggling frequency = 5 Hz

    Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.    
		 ----------------------------------------------------------------------- */ 	
	
	//=======================Configure and init Timer======================
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 500000) - 1;
 /* TIM Configuration */
  TIM3_Config();

	// configure the output compare
	TIM3_OCConfig();

  /* TIM Interrupts enable */
  TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
	
//======================================configure and init LCD  ======================	
	 /* LCD initiatization */
  LCD_Init();
  
  /* LCD Layer initiatization */
  LCD_LayerInit();
    
  /* Enable the LTDC */
  LTDC_Cmd(ENABLE);
  
  /* Set LCD foreground layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
	
//================EEPROM init====================================

/* Unlock the Flash Program Erase controller */
		FLASH_Unlock();
		/* EEPROM Init */
		EE_Init();
	EE_WriteVariable(VirtAddVarTab2[0], 36466); //This writes our score into the EEPROM.
//============ Set up for random number generation==============
	RNG_Config();
	//with the default font, LCD can display  12 lines of chars, they are LINE(0), LINE(1)...LINE(11) 
	//with the default font, LCD can display  15 columns, they are COLUMN(0)....COLUMN(14)
LCD_Clear(LCD_COLOR_WHITE);
				//SystemInit();
		LCD_DisplayStringLine(LINE(0),  (uint8_t *) " REACTION-TIME ");  //the line will not wrap
		LCD_DisplayStringLine(LINE(1),  (uint8_t *) "    TESTER    ");
		LCD_DisplayStringLine(LINE(3),  (uint8_t *) "    LAB# 2    ");
		LCD_DisplayStringLine(LINE(5),  (uint8_t *) "   GROUP#13   ");
		LCD_DisplayStringLine(LINE(6),  (uint8_t *) "---------------");
		LCD_DisplayStringLine(LINE(9),  (uint8_t *) " PRESS USER TO ");
		LCD_DisplayStringLine(LINE(10),  (uint8_t *) "    CONTINUE   ");	
  while (1){

			}
	}
/**
  * @brief  Configure the TIM IRQ Handler.
  * @param  No
  * @retval None
  */


void PB_Config(void)
{
/* Initialize User_Button on STM32F4-Discovery
   * Normally one would need to initialize the EXTI interrupt
   * to handle the 'User' button, however the function already
   * does this.
   */
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
}


void LED_Config(void)
{
 /* Initialize Leds mounted on STM32F429-Discovery board */
  STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4); 

  /* Turn on LED3, LED4 */
  STM_EVAL_LEDOn(LED3);
	STM_EVAL_LEDOn(LED4);
}

// we are going to configure our external button here.
void Configure_PB1(void) {
	 
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  /* Enable GPIOA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure PA1 pin as input floating */

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Connect EXTI Line1 to PA1 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);

  /* Configure EXTI Line1 */

  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
//    /* Set variables used */
//    GPIO_InitTypeDef GPIO_InitStruct;
//    EXTI_InitTypeDef EXTI_InitStruct;
//    NVIC_InitTypeDef NVIC_InitStruct;
//    
//    /* Enable clock for GPIOB */
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//    /* Enable clock for SYSCFG */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//    
//    /* Set pin as input */
//    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
//    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
//    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
//    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStruct);
//    
//    /* Tell system that you will use PB12 for EXTI_Line12 */
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
//    
//    /* PB12 is connected to EXTI_Line12 */
//   // EXTI_InitStruct.EXTI_Line = EXTI_Line12;
//    /* Enable interrupt */
//   // EXTI_InitStruct.EXTI_LineCmd = ENABLE;
//    /* Interrupt mode */
//    //EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
//    /* Triggers on rising and falling edge */
//    //EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//    /* Add to EXTI */
//    //EXTI_Init(&EXTI_InitStruct);
// 
//    /* Add IRQ vector to NVIC */
//    /* PB12 is connected to EXTI_Line12, which has EXTI15_10_IRQn vector */
//    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
//    /* Set priority */
//    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
//    /* Set sub priority */
//    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
//    /* Enable interrupt */
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//    /* Add to NVIC */
//    NVIC_Init(&NVIC_InitStruct);
}
 

void TIM3_Config(void) 
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//since TIMER 3 is on APB1 bus, need to enale APB1 bus clock first
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	//====================================================
	//Enable TIM3 global interrupt ====does this part need to be done before TIM_BaseStructure set up?
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0X00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	//================================================
	
	TIM_TimeBaseStructure.TIM_Period=65535; // need to be larger than CCR1_VAL, has no effect on the Output compare event.
	TIM_TimeBaseStructure.TIM_Prescaler=PrescalerValue;    //why all the example make this one equal 0, and then use 
					//function TIM_PrescalerConfig() to re-assign the prescaller value?
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	//TIM_PrescalerConfig(TIM3, TIM3Prescaler, TIM_PSCReloadMode_Immediate);
}


void TIM3_OCConfig(void) {
	TIM_OCInitTypeDef TIM_OCInitStructure;

	

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_Timing;

	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;

	TIM_OCInitStructure.TIM_Pulse=CCR1_Val;

	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;

	

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);

	

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable); //if disabled, 

	//the TIMx_CCRx register can be updated at any time by software to control the output

	//waveform---from the reference manual
}	
	
void TIM_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* Enable the TIM2 global Interrupt
   * THis is used to capture the button push.
   */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}


void LCD_DisplayString(uint16_t LineNumber, uint16_t ColumnNumber, uint8_t *ptr)
{  
  //here the LineNumber and the ColumnNumber are NOT  pixel numbers!!!
		while (*ptr!=NULL)
    {
				LCD_DisplayChar(LINE(LineNumber), COLUMN(ColumnNumber), *ptr);
				ColumnNumber++;
			 //to avoid wrapping on the same line and replacing chars 
				if (ColumnNumber*(((sFONT *)LCD_GetFont())->Width)>=LCD_PIXEL_WIDTH ){
					ColumnNumber=0;
					LineNumber++;
				}
					
				ptr++;
		}
}

void LCD_DisplayInt(uint16_t LineNumber, uint16_t ColumnNumber, int Number)
{  
  //here the LineNumber and the ColumnNumber are NOT  pixel numbers!!!
		char lcd_buffer[15];
		sprintf(lcd_buffer,"%d",Number);
	
		LCD_DisplayString(LineNumber, ColumnNumber, (uint8_t *) lcd_buffer);
}

void LCD_DisplayFloat(uint16_t LineNumber, uint16_t ColumnNumber, float Number, int DigitAfterDecimalPoint)
{  
  //here the LineNumber and the ColumnNumber are NOT  pixel numbers!!!
		char lcd_buffer[15];
		
		sprintf(lcd_buffer,"%.*f",DigitAfterDecimalPoint, Number);  //6 digits after decimal point, this is also the default setting for Keil uVision 4.74 environment.
	
		LCD_DisplayString(LineNumber, ColumnNumber, (uint8_t *) lcd_buffer);
}

void RNG_Config(void){
	//Enable RNG controller clock
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	
	//activate the RNG peripheral 
	RNG_Cmd(ENABLE);
	
	// to get a random number, need to continue steps: 3. Wait until the 32 bit Random number Generator 
	//contains a valid random data (using polling/interrupt mode). For more details, 
	//refer to Section 20.2.4: "Interrupt and flag management" module description.
	//4. Get the 32 bit Random number using RNG_GetRandomNumber() function
	//5. To get another 32 bit Random number, go to step 3.
}	




#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while (1)
  {}
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
