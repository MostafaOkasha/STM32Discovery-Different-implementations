#include "main.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_i2c_ee.h"

/*The program will switch beween it's many states depending on what the environment has externally interacted with the system.
 We will move between many states of which the program will be run through and continuously looped through.
 Our first state will display the current time.
 S0 = Initial state: only time is displayed
 s1 = not a stable state, data is immediately stored onto EEPROM and we return to state s0
 s2 = In this state both time and date are displayed on the lcd while the UB is pressed.
 s3 = the last 2 times for the button presses are displayed when the external putton is held.
 s4 = first screen to change the hours for time.
 s5 = second screen to change the minutes for time.
 s6 = third screen to change the seconds for time.
 s7 = fourth screen to change the date for date.
 s8 = fifth screen to change the month for date.
 s9 = sixths screen to change the year for date. if set is pressed again we return to s0. */


//memory location to write to in the device

//**********************************************************************************************************************
//**********************************************************************************************************************
//code for writing into eeprom
uint8_t sEE_WRITE_ADDRESS = 0x50;
uint8_t sEE_READ_ADDRESS;
uint8_t Tx1_Buffer; //buffer used to write into eeprom
uint8_t Rx1_Buffer; //buffer used to read from eeprom

uint16_t NumDataRead = 3;
//**********************************************************************************************************************
//**********************************************************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0; //Used in the RTC_Config() function.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern int state;
int banana = 0;
int state = 0;
int substate = 0;
int seconds1 = 0;
int seconds2 = 0;
int minutes1 = 0;
int minutes2 = 0;
int hours1 = 0;
int hours2 = 0;
int minutes = 0;
int seconds = 0;
int ubstate = 0;
int year = 0;
int month = 0;
int day = 0;
int hours = 0;
char timebuffer[14];
char tempbuffer[3];
char datebuffer[14];
char lcd_buffer_1[14];
char lcd_buffer_2[14];
int userbuttonhold = 0; //set initial button states to 0
int external_button1 = 0;
int external_button2 = 0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RTC_InitTypeDef RTC_InitStructure; //The following RTC code was obtained from the example in the reference manual (21.4) and
RTC_TimeTypeDef RTC_TimeStructure; // From the prepheril examples provided.
RTC_DateTypeDef RTC_DateStructure;
RTC_AlarmTypeDef  RTC_AlarmStructure; //Setting up the alarm interrupt to interrupt every second. We choose alarm A.
EXTI_InitTypeDef   EXTI_InitStructure; //External interrupt
extern NVIC_InitTypeDef NVIC_InitStructure; //Alarm code obtained from RTC_Alarm example from the perepheril examples.
void LCD_DisplayString(uint16_t LineNumber, uint16_t ColumnNumber, uint8_t *ptr);
void LCD_DisplayInt(uint16_t LineNumber, uint16_t ColumnNumber, int Number);
void LCD_DisplayFloat(uint16_t LineNumber, uint16_t ColumnNumber, float Number, int DigitAfterDecimalPoint);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//We were told to use the LSI clock, this code was taken from the RTC Example.
#define RTC_CLOCK_SOURCE_LSI /* LSI used as RTC source clock. The RTC Clock
                                      may varies due to LSI frequency dispersion. */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RTC_TimeShow(void);
void RTC_DateShow(void);
void UBHELD(void);
void UBRELEASED(void);
void EBHELD(void);
void EBRELEASED(void);
void RTC_changedate(); //int month, int year, int day
void RTC_changetime(); //int hours, int minutes, int seconds
void settimedate();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void){
	/* System init */
	SystemInit();
	
	//configures: User Button, 3 external push-buttons and their significant interrupts
	PB_Config(); //User button config.
  EXTButton_1(); //First external button config
  EXTButton_2(); //Second External Button config

	 /* LCD initiatization */ //LCD stuff
  LCD_Init();
  /* LCD Layer initiatization */
  LCD_LayerInit();
	
	LCD_Clear(LCD_COLOR_YELLOW);
  LCD_SetBackColor(LCD_COLOR_YELLOW);
  LCD_SetTextColor(LCD_COLOR_RED);
  /* Enable the LTDC */
  LTDC_Cmd(ENABLE);
  /* Set LCD foreground layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
	//configure real-time clock
	RTC_Config();
	sEE_Init(); //initialize sEE 
	//void i2c_init(); //initialize the i2c chip	

while(1){ //Main Loop
	
	if (state == 0){
	RTC_TimeShow();
	LCD_DisplayStringLine(LINE(6),  (uint8_t *) "B1 -> see old T");
	LCD_DisplayStringLine(LINE(7),  (uint8_t *) "B2 -> To set T");
	if (userbuttonhold == 1){
		UBHELD();
	}
	else if (substate == 1){
		if(!(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))){
	UBRELEASED();
	}
}
	if (external_button1 == 1){
		external_button1 = 0; 
		if (banana == 1){
			banana = 0;
			LCD_DisplayStringLine(LINE(10), (uint8_t *) "                   ");
			LCD_DisplayStringLine(LINE(6),  (uint8_t *) "B1 -> see old T");
			LCD_DisplayStringLine(LINE(11), (uint8_t *) "                    ");
			LCD_DisplayStringLine(LINE(12), (uint8_t *) "                    ");
		}
		else if (banana == 0){
			LCD_DisplayStringLine(LINE(6),  (uint8_t *) "                     ");
			EBHELD();
			banana = 1;
		}
	}

	if (external_button2 == 1){
		state = 2;
		external_button2 = 0;
		}
}
	if (state == 2){ //state for changing time/date
		
		/* Get the Time */ //At the beginning of each code run
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
		hours = RTC_TimeStructure.RTC_Hours;
		minutes = 	RTC_TimeStructure.RTC_Minutes;
		seconds =	RTC_TimeStructure.RTC_Seconds;

		/* Get the Date */ //At the beginning of each code run
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
		month =	RTC_DateStructure.RTC_Month;
		day =	RTC_DateStructure.RTC_Date;
		year =	RTC_DateStructure.RTC_Year;
		
		RTC_changedate(); //month,year,day
		RTC_changetime(); //hours,minutes,seconds
		
		substate = 0;
		/////////////////////////////////////////////////////////////////////////////////
		while(substate == 0){ //hour set
		RTC_changetime();
		LCD_DisplayStringLine(LINE(2),  (uint8_t *) "Set hour    ");
		//code for setting hour
		if (external_button1 == 1){
		settimedate();
		}	
		//
		if (external_button2 == 1){
		substate = 1;
		external_button2 = 0;
		}
	}
		/////////////////////////////////////////////////////////////////////////////////
		while(substate == 1){ //minute set
		RTC_changetime();
		LCD_DisplayStringLine(LINE(2),  (uint8_t *) "Set minute      ");
		//code for setting minute
		if (external_button1 == 1){
		settimedate();
		}
		//
		if (external_button2 == 1){
		substate = 2;
		external_button2 = 0;
		}
		}
		/////////////////////////////////////////////////////////////////////////////////
		while(substate == 2){ //second set
		RTC_changetime();
		LCD_DisplayStringLine(LINE(2),  (uint8_t *) "Set seconds      ");
		//code for setting minute
		if (external_button1 == 1){
		settimedate();
		}
		//
		if (external_button2 == 1){
		substate = 3;
		external_button2 = 0;
		LCD_DisplayStringLine(LINE(2),  (uint8_t *) "                ");
		}
		}
		/////////////////////////////////////////////////////////////////////////////////
		while(substate == 3){ //set day
		RTC_changedate();
		LCD_DisplayStringLine(LINE(5),  (uint8_t *) "Set day      ");
		//code for setting minute
		if (external_button1 == 1){
		settimedate();
		}
		//
		if (external_button2 == 1){
		substate = 4;
		external_button2 = 0;
		}
		}
		/////////////////////////////////////////////////////////////////////////////////
		while(substate == 4){ //set month
		RTC_changedate();
		LCD_DisplayStringLine(LINE(5),  (uint8_t *) "Set month      ");
		//code for setting minute
		if (external_button1 == 1){
		settimedate();
		}	
		//
		if (external_button2 == 1){
		substate = 5;
		external_button2 = 0;
		}
		}
		/////////////////////////////////////////////////////////////////////////////////
		while(substate == 5){ //set year
		RTC_changedate();
		LCD_DisplayStringLine(LINE(5),  (uint8_t *) "Set year      ");
		//code for setting minute
		if (external_button1 == 1){
		settimedate();
		}
		//
		if (external_button2 == 1){
		substate = 0;
		external_button2 = 0;
		state = 0;
		LCD_DisplayStringLine(LINE(5),  (uint8_t *) "                   ");
		LCD_DisplayStringLine(LINE(3),  (uint8_t *) "                   ");
		LCD_DisplayStringLine(LINE(4),  (uint8_t *) "                   ");
		}
		}
		/////////////////////////////////////////////////////////////////////////////////
	/* Set the Time */
				RTC_TimeStructure.RTC_Hours = hours;
				RTC_TimeStructure.RTC_Minutes = minutes;
				RTC_TimeStructure.RTC_Seconds = seconds;
				RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);

	/* Set the Date */
				RTC_DateStructure.RTC_Month = month;
				RTC_DateStructure.RTC_Date = day;
				RTC_DateStructure.RTC_Year = year;
				RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
		/////////////////////////////////////////////////////////////////////////////////
	}
	
}
	} //END OF MAIN//

void settimedate(){
	if (substate == 0){ //set the hour
		hours++;
		if (hours == 24){
			hours = 0;
		}
	}
	if (substate == 1){ //set the minute
		minutes++;
		if (minutes == 60){
			minutes = 0;
		}
		}
	if (substate == 2){ //set the second
		seconds++;
		if (seconds == 60){
			seconds = 0;
		}
	}
	if (substate == 3){ //set the day
		day++;
		if (day == 32){
			day = 1;
		}
	}
	if (substate == 4){ //set the month
		month++;
		if (month == 13){
			month = 1;
		}
	}
	if (substate == 5){ //set the year
		year++;	
	}
	external_button1 = 0;
}
void EBHELD(void){ //When external button is held.
	  ///////////////////////////////////////////////////////////////////////////read seconds1
		sEE_READ_ADDRESS = sEE_WRITE_ADDRESS - 3;
  	sEE_ReadBuffer(&Rx1_Buffer,sEE_READ_ADDRESS, (uint16_t *)(&NumDataRead));
  	seconds1 = Rx1_Buffer;
		sEE_READ_ADDRESS = sEE_READ_ADDRESS - 3;
		///////////////////////////////////////////////////////////////////////////read minutes1
  	sEE_ReadBuffer(&Rx1_Buffer,sEE_READ_ADDRESS, (uint16_t *)(&NumDataRead));
  	minutes1 = Rx1_Buffer;
		sEE_READ_ADDRESS = sEE_READ_ADDRESS - 3;
		///////////////////////////////////////////////////////////////////////////read hours1
  	sEE_ReadBuffer(&Rx1_Buffer,sEE_READ_ADDRESS, (uint16_t *)(&NumDataRead));
  	hours1 = Rx1_Buffer;
		sEE_READ_ADDRESS = sEE_READ_ADDRESS - 3;
		///////////////////////////////////////////////////////////////////////////read seconds2
  	sEE_ReadBuffer(&Rx1_Buffer,sEE_READ_ADDRESS, (uint16_t *)(&NumDataRead));
  	seconds2 = Rx1_Buffer;
		sEE_READ_ADDRESS = sEE_READ_ADDRESS - 3;
		///////////////////////////////////////////////////////////////////////////read minutes2
  	sEE_ReadBuffer(&Rx1_Buffer,sEE_READ_ADDRESS, (uint16_t *)(&NumDataRead));
  	minutes2 = Rx1_Buffer;
		sEE_READ_ADDRESS = sEE_READ_ADDRESS - 3;
		///////////////////////////////////////////////////////////////////////////read hours2
  	sEE_ReadBuffer(&Rx1_Buffer,sEE_READ_ADDRESS, (uint16_t *)(&NumDataRead));
		hours2 = Rx1_Buffer;
		///////////////////////////////////////////////////////////////////////////
		snprintf(lcd_buffer_1,100,"1:   %d:%d:%d    ", hours1,minutes1,seconds1);
		snprintf(lcd_buffer_2,100,"2:   %d:%d:%d    ", hours2,minutes2,seconds2);
		LCD_DisplayStringLine(LINE(10), (uint8_t *) "Previous Times:");
		LCD_DisplayStringLine(LINE(11), (uint8_t *) lcd_buffer_1);
		LCD_DisplayStringLine(LINE(12), (uint8_t *) lcd_buffer_2);
}

void UBRELEASED(void){
	LCD_DisplayStringLine(LINE(3),  (uint8_t *) "                 ");
	LCD_DisplayStringLine(LINE(4), (uint8_t *) "                   ");
	RTC_TimeShow();
	userbuttonhold = 0;
	ubstate = 0;
}

void UBHELD(void){
	//Checks to see if user button is pressed and held in state s0.
	if(state==0){
				RTC_TimeShow();
				RTC_DateShow();
		if (ubstate == 0){
		//User button is pressed, time has to be recorded.
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
		//////////////////////////////////////////////////HOURS
		Tx1_Buffer = RTC_TimeStructure.RTC_Hours;
		sEE_WriteBuffer(&Tx1_Buffer,sEE_WRITE_ADDRESS,3);
		sEE_WaitEepromStandbyState();
		sEE_WRITE_ADDRESS = sEE_WRITE_ADDRESS + 3;
		//////////////////////////////////////////////////MINUTES
		Tx1_Buffer = RTC_TimeStructure.RTC_Minutes;
		sEE_WriteBuffer(&Tx1_Buffer,sEE_WRITE_ADDRESS,3);
		sEE_WaitEepromStandbyState();
		sEE_WRITE_ADDRESS = sEE_WRITE_ADDRESS + 3;
		//////////////////////////////////////////////////SECONDS
		Tx1_Buffer = RTC_TimeStructure.RTC_Seconds;
		sEE_WriteBuffer(&Tx1_Buffer,sEE_WRITE_ADDRESS,3);
		sEE_WaitEepromStandbyState();
		sEE_WRITE_ADDRESS = sEE_WRITE_ADDRESS + 3;
		//////////////////////////////////////////////////
		ubstate = 1;
	}
}
	userbuttonhold = 0;
	substate = 1;
}

void PB_Config(void){
/* Initialize User_Button on STM32F4-Discovery
   * Normally one would need to initialize the EXTI interrupt
   * to handle the 'User' button, however the function already
   * does this.
   */
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
}


/**
 * Use this function to configure the GPIO to handle input from
 * external pushbuttons and configure them so that you will handle
 * them through external interrupts.
 */

void EXTButton_1(void){ //PB2 config - First external Button B1
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
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //Interrupt at Rising edge (When button is pressed)

		EXTI_Init(&EXTI_InitStructure); //Creates an interrupt handler

		/* Enable and set EXTI Line1 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //Set priority to highest priority.
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; //Set sub priotiy to lowest priority.
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //Turn on the handler
		NVIC_Init(&NVIC_InitStructure); //Creates the interrupt
}

void EXTButton_2(void){ //PB12 Config - Second external Button B2
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
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //Interrupt at Rising edge (When button is pressed)

		EXTI_Init(&EXTI_InitStructure); //Creates an interrupt handler

		/* Enable and set EXTI Line1 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; //For pins 10 - 15 you use EXTI15_10.
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //Set priority to highest priority.
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; //Set sub priotiy to lowest priority.
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //Turn on the handler
		NVIC_Init(&NVIC_InitStructure); //Creates the interrupt
}

/**
 * Configure the RTC to operate based on the LSI (Internal Low Speed oscillator)
 * and configure one of the alarms (A or B) to trigger an external interrupt every second
 * (e.g. EXTI line 17 for alarm A).
 */
void RTC_Config(void){ //Code obtained from timestamp example of the perepheril RTC.
	/* Enable write access to the RTC ****************************/
 /* Enable the PWR clock */
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
 /* Allow access to RTC */
 PWR_BackupAccessCmd(ENABLE);
/* Configure the RTC clock source ****************************/
 /* Enable the LSI OSC */
	//Enable the RTC clock using LSI
  RCC_LSICmd(ENABLE);
	/* Wait till LSE is ready */
 while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) // Code Obtained from 21.4. Changed LSE to LSI. 
 { // Wait for LSI to be ready before moving on.
 }
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI); //LSI is the clock source.

  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;
 
  /* Enable the RTC Clock */
 RCC_RTCCLKCmd(ENABLE);
 
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
 //This is it for initializing the RTC.
 
 //Now we need to initialize the Alarm and the interrupt it will cause. --Code from RTC_Timer Perepheril example.
 
 /* EXTI configuration */
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  /* Enable the RTC Alarm A Interrupt */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
		 
	/* Enable the alarm  A */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
}



void RTC_TimeShow(void){
	 /* Get the current Time and Date */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	// format time to display on lcd
	snprintf(timebuffer,100,"    %d:%d:%d    ",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
	LCD_DisplayStringLine(LINE(0),  (uint8_t *) "Current Time:");
	LCD_DisplayStringLine(LINE(1), (uint8_t *) timebuffer);
	/* Unfreeze the RTLCD_DisplayStringLine DR Register */
	(void)RTC->DR;
}


void RTC_changetime(){ //int hours, int minutes, int seconds
		 /* Get the current Time and Date */
  //RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	// format time to display on lcd
	snprintf(timebuffer,100,"    %d:%d:%d    ",hours,minutes,seconds);
	LCD_DisplayStringLine(LINE(0),  (uint8_t *) "Set Time:         ");
	LCD_DisplayStringLine(LINE(6),  (uint8_t *) "                     ");
	LCD_DisplayStringLine(LINE(7),  (uint8_t *) "                   ");
	LCD_DisplayStringLine(LINE(1), (uint8_t *) timebuffer);
	/* Unfreeze the RTLCD_DisplayStringLine DR Register */
	//(void)RTC->DR;
}
void RTC_DateShow(void){
	  /* Get the current Date */
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	snprintf(datebuffer,100,"     %d/%d/%d    ",RTC_DateStructure.RTC_Date, RTC_DateStructure.RTC_Month, RTC_DateStructure.RTC_Year);
	LCD_DisplayStringLine(LINE(3),  (uint8_t *) "Current Date:");
	LCD_DisplayStringLine(LINE(4), (uint8_t *) datebuffer);
}

void RTC_changedate(){ //int month, int year, int day
  //RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	snprintf(datebuffer,100,"     %d/%d/%d    ",day,month,year);
	LCD_DisplayStringLine(LINE(3),  (uint8_t *) "Set Date:");
	LCD_DisplayStringLine(LINE(4), (uint8_t *) datebuffer);
}
