/**
  ******************************************************************************
  * @file    TIM_TimeBase/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
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
#include "stm32f4xx_it.h"
#include "stm32f429i_discovery.h"
#include "main.h"
#include <stdio.h>
#include <stm32f4xx_rng.h>
#include "stm32f429i_discovery_lcd.h"

/* Private typedef -----------------------------------------------------------*/
extern int external_button1;
extern int external_button2;
extern int banana1;
extern int banana2;
/* Private typedef -----------------------------------------------------------*/
extern TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
extern TIM_OCInitTypeDef  TIM_OCInitStructure;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern int16_t i;
extern int16_t HalfSwitch;
extern int16_t FullSwitch;
extern int16_t count;
extern int16_t TypeSwitch;
uint16_t capture = 0;
extern __IO uint16_t CCR1_Val;
extern uint16_t PrescalerValue;
void EXTILine1_Config(void);
void EXTILine2_Config(void);
void EXTILine3_Config(void);
void Timer_Config(void);
__IO uint32_t RTCAlarmCount = 0;
extern __IO uint8_t AlarmUp;
/**
  * @brief  Converts a 2 digit decimal to BCD format.
  * @param  Value: Byte to be converted.
  * @retval Converted byte
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
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
  {}
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
  {}
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
  {}
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
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void){
	//uncomment me if you want to handle systicks
	//TimingDelay_Decrement();
}

/******************************************************************************/
/*            STM32F4xx Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
  {
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
		if(TypeSwitch == 0){
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
			}
			if(i == 2){
				GPIO_SetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_6);
				GPIO_ResetBits(GPIOB, GPIO_Pin_5|GPIO_Pin_7);
			}
			if(i == 3){
				GPIO_SetBits(GPIOB, GPIO_Pin_6);
				GPIO_ResetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7);
			}
		 	if(i == 4){
				GPIO_SetBits(GPIOB, GPIO_Pin_5|GPIO_Pin_6);
				GPIO_ResetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_7);
			}
			if(i == 5){
				GPIO_SetBits(GPIOB, GPIO_Pin_5);
				GPIO_ResetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7);
			}
			if(i == 6){
				GPIO_SetBits(GPIOB, GPIO_Pin_5|GPIO_Pin_7);
				GPIO_ResetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_6);
			}
			if(i == 7){
				GPIO_SetBits(GPIOB, GPIO_Pin_7);
				GPIO_ResetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);
			}
			if(i == 8){
				GPIO_SetBits(GPIOB, GPIO_Pin_4|GPIO_Pin_7);
				GPIO_ResetBits(GPIOB, GPIO_Pin_5|GPIO_Pin_6);
			}
		}
		capture = TIM_GetCapture1(TIM3);
		TIM_SetCompare1(TIM3, capture + CCR1_Val);
	}
}

/**
 * External interrupt channel 0 Interrupt Handler. This handles
 * the user button.
 */

void EXTI0_IRQHandler(void){ //Userbutton External Handler
	STM_EVAL_LEDToggle(LED3);
	if (HalfSwitch == 0){
		HalfSwitch = 1;
		i = 0;
		count = 1;
	}
	else if (HalfSwitch == 1){
		HalfSwitch = 0;
		i = 9;
		count = -1;
	}
	EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
}
//External Pushbutton Switches Between CW&CCW in FullSteps
void EXTI2_IRQHandler(void){ //First external button external handler
	STM_EVAL_LEDToggle(LED3);
		if (FullSwitch == 0){
		FullSwitch = 1;
		i = 0;
		count = 2;
	}
	else if (FullSwitch == 1){
		FullSwitch = 0;
		i = 9;
		count = -2;
	}
	EXTI_ClearITPendingBit(EXTI_Line2);
}

//External Pushbutton Switches Between Interrupts & Polling
void EXTI15_10_IRQHandler(void){ //second external button external handler
	STM_EVAL_LEDToggle(LED3);
			if (TypeSwitch == 0){
		i = 0;
		HalfSwitch = 0;
		count = 1;  
		TypeSwitch = 1;
	}
	else if (TypeSwitch == 1){
		i = 0;
		HalfSwitch = 0;
		count = 1;  
		TypeSwitch = 0;
	}
	EXTI_ClearITPendingBit(EXTI_Line12);
}

void EXTI11_IRQHandler(void){ //third external button external handler
	STM_EVAL_LEDToggle(LED3);
	EXTI_ClearITPendingBit(EXTI_Line1);
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
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

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
