/**
  ******************************************************************************
  * @file    Lab2/stm32f4xx_it.c
  * @author Rert Li
  * @version 
  * @date    August 2014
  * @brief   
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include <stm32f4xx_rng.h>
#include "eeprom.h"
#include <stdio.h>
// again we added the includes that we thought would be required in this file.

/** @addtogroup Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t currenttime = 0; //This would mark the current time from the TIME_CAPTURE.
int temp = 0;
int currentstate = 0; //We will use this to determine which state of the program we are in.
int totaltime = 0;
int i = 0; // Will be used for itterations.
char printlcd1 [15];
char printlcd2 [15];
uint16_t highscore; //We will use this variable to obtain the highscore from the EEPROM
extern __IO uint16_t CCR1_Val;
extern uint16_t PrescalerValue;
extern __IO uint8_t UBPressed;
uint16_t VirtAddVarTab[NB_OF_VAR] = {0x5555, 0x6666, 0x7777};
__IO uint32_t randomnumber = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}


/******************************************************************************/
/*            STM32F4xx Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */

// as we already resolved before, we have 5 states we will label them states: 0,1,2,3,4 and 5.
// Each state will involve with dealing with a certain problem and lead us to the solution.
// some states will automatically move on to the other state while some will require a user input
// to move from one state to another.

//Timer interrupt handler (Runs once every 1ms)
void TIM3_IRQHandler(void){
	
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET){
		
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1); //We set TIM3 to generate an interrupt every 1 ms.
		//so that each time our code runs we can add another ms.
		currenttime = TIM_GetCapture1(TIM3);
   TIM_SetCompare1(TIM3, currenttime + CCR1_Val);
	}
// As the program first runs when the board is reset, the currentstate value is always 0 so we will always start
// at the state where our LED's are flashing at a low Hz with the main screen we provided.
		
	if (currentstate == 0){ // This is our state (P) --> start state
				i = i + 1;
		if (i == 200){
				STM_EVAL_LEDToggle(LED4);
				STM_EVAL_LEDToggle(LED3);
    /* LED3 and 4 toggling with a low frequency */
		totaltime = 0;
		i = 0;
		}
		// From our main function, the rest of state P is defined and implemented there.
		}

		if (currentstate == 1){ //This is turning off the LEDs and generating a random number
				STM_EVAL_LEDOff(LED3);
				STM_EVAL_LEDOff(LED4); 
				while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET){}
				randomnumber = RNG_GetRandomNumber()%2000;
				currentstate = 2;
				i = 0;
  }
		
		if (currentstate == 2){ // We know the interrupt will run once every 1 ms, so we can make it run for 2000 + rand
			// Amount of mss.
					i = i + 1;
			if (i == (randomnumber + 2000)){ 
								STM_EVAL_LEDOn(LED4);
								STM_EVAL_LEDOn(LED3);
								currentstate = 3;
			}
		}
		
			if (currentstate == 3){
				totaltime = totaltime + 1; //integer of total time taken .
			}
			
			if (currentstate == 4){
						LCD_Clear(LCD_COLOR_WHITE);
				// We use snprintf instead of sprintf in order to provide a maximum upperbound value.
				// We previously set printlcd as our buffer
						snprintf(printlcd1,100,"%d",totaltime);
						//LCD_DisplayStringLine(LINE(10),printlcd); we want to do that but dont know how.
					EE_ReadVariable(VirtAddVarTab[0],&highscore);
				LCD_DisplayStringLine(LINE(8),  (uint8_t *) "Your Score:" );
				LCD_DisplayStringLine(LINE(9),  (uint8_t *) printlcd1 );
				LCD_DisplayStringLine(LINE(11),  (uint8_t *) "Best Score:" );
				if (totaltime < highscore){
					EE_WriteVariable(VirtAddVarTab[0],totaltime); //sets new highscore if our newtime is lower than the highscore.
					LCD_DisplayStringLine(LINE(12),  (uint8_t *) printlcd1 );
				}
					else if (totaltime > highscore){
					snprintf(printlcd2,100,"%d",highscore);
					LCD_DisplayStringLine(LINE(12),  (uint8_t *) printlcd2);
					}
				}
			}
			
//USER BUTTON EXTERNAL HANDLER
void EXTI10_IRQHandler(void) {
//    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
				LCD_DisplayStringLine(LINE(9),  (uint8_t *) "" );
				LCD_DisplayStringLine(LINE(10),  (uint8_t *) "" );
//        /* Do your stuff when PB12 is changed */
       UBPressed =1;
	if (currentstate == 2){
		currentstate = 0;
		LCD_DisplayStringLine(LINE(9),  (uint8_t *) " PRESS USER TO ");
		LCD_DisplayStringLine(LINE(10),  (uint8_t *) "    CONTINUE   ");	
	}
	else {
		currentstate = 4;
        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
    
}
}
		}
// This is the interrupt for the external button.
// Pressing the external button would send us to state 1.
//EXTERNAL BUTTON
void EXTI1_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
    currentstate=0;		// sets state to 0 when external button is pressed
		i =0;	// resets counter
		EXTI_ClearITPendingBit(EXTI_Line1); // clears external button bit
  }
}

//void EXTI1_IRQHandler(void){
//	//if(EXTI_GetITStatus(EXTI_Line1) != RESET){
//	currentstate = 0;
//	//	}
//	EBPressed = 1;
//		EXTI_ClearITPendingBit(EXTI_Line1); // clears external button bit
//  }
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
