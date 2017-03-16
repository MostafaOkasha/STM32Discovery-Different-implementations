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

/**
  * @brief  Converts a 2 digit decimal to BCD format.
  * @param  Value: Byte to be converted.
  * @retval Converted byte
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//These variables will be initialized in main.c or in this file if required.
extern int external_button1;
extern int external_button2;
extern int userbuttonhold;
extern int banana;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void RTC_TimeShow(void);
void RTC_DateShow(void);
void RTC_Config(void);
void PB_Config(void);
void EXTButton_1(void);
void EXTButton_2(void);
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
void TIM3_IRQHandler(void) //We will use this to toggle LEDS.
{
  
}

/**
 * External interrupt channel 0 Interrupt Handler. This handles
 * the user button.
 */
void EXTI0_IRQHandler(void){ //User button handler. - handles interrupts connected to line 0
	//clear the pending bit otherwise the handler will fire continually
	userbuttonhold = 1;
	EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
	//this is configured to handle the push-button
}

void EXTI2_IRQHandler(void){ //External Button 1 handler -  - handles interrupts connected to line 2
	if (EXTI_GetITStatus(EXTI_Line2) != RESET) {
	external_button1 = 1;
	//clear pending bit
	}
	EXTI_ClearITPendingBit(EXTI_Line2);
}

void EXTI15_10_IRQHandler(void){ //External Button 2 handler -  - handles interrupts connected to line 10-15
	if (EXTI_GetITStatus(EXTI_Line12) != RESET) { // if the state of the button changes do this:
	external_button2 = 1;
	}
	//clear pending bit
	EXTI_ClearITPendingBit(EXTI_Line12);
}
void RTC_Alarm_IRQHandler(void){
	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
  {
    RTC_ClearITPendingBit(RTC_IT_ALRA);
    EXTI_ClearITPendingBit(EXTI_Line17);
  }
}
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
