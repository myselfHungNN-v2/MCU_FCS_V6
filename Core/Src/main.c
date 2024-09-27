/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {IDLE,State_1,State_2} State_Machine;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ENABLE_Fire_PIN  HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
#define DISABLE_Fire_PIN HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//======= Communication =======//
uint8_t Rx_Buff[10];
uint8_t Rx_Data;
uint8_t Rx_Index;
uint8_t Rx_Copy[10];
uint8_t Tx_Flag;
uint8_t Tx_Buff[10];
// ====== ADC ====== //
uint16_t ADC_Out[3];
GPIO_PinState A,B,C,D;
uint8_t Sig_ADC_Vol;
// ====== Sensor ======//

//uint8_t S0_Sensor ;
//uint8_t S1_Sensor ;
//uint8_t S2_Sensor ;
//uint8_t S3_Sensor ;
//uint8_t Sensor[4];

uint8_t Sig_Sensor[16];
uint8_t Sensor_16;
uint8_t Sensor_17;
//
GPIO_PinState A0_Button_1;
GPIO_PinState A1_Button_1;
GPIO_PinState A2_Button_1;
GPIO_PinState E0_Button_1;

uint8_t A18 =0x00,A1=0x00,A2=0x00,A3=0x00,A4=0x00,A5=0x00,A6=0x00,A7=0x00,A8=0,A9=0,A10=0,A11=0,A12=0,A13=0,A14=0,A15=0,A16=0,A17=0;
uint8_t A01 =0x00, A02=0x00, A03=0x00, A04=0x00, A05=0x00, A06=0x00, A07=0x00, A08=0x00, A09=0x00, A010=0x00, A011=0x00, A012=0x00, A013=0x00, A014=0x00, A015=0x00, A016=0x00, A017=0x00, A018=0x00;

GPIO_PinState A0_Button_2;
GPIO_PinState A1_Button_2;
GPIO_PinState A2_Button_2;
GPIO_PinState E0_Button_2;

uint8_t Button_8;
uint8_t Button_9;
//
uint8_t Allow_F;
uint8_t Allow_F_Fire;
uint8_t Mode_F;
uint8_t Mode_F_Fire;
GPIO_PinState F_A,F_B,F_C,F_D;
uint16_t Read_Timer;
uint16_t Count;

State_Machine State = IDLE;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

CAN_HandleTypeDef hcan;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */
void Ana_ADC(GPIO_PinState A, GPIO_PinState B, GPIO_PinState C, GPIO_PinState D);
void Handle_ADC(void);

void Systick_Delay(unsigned int x);
void Handle_Button(void);

void Allow_Fire(void);
void Mode_Fire(void);
void Do_Fire(int8_t pipeIdx);  // pipeIdx = 1...18
void Ana_Button_Fire(GPIO_PinState F_A, GPIO_PinState F_B, GPIO_PinState F_C, GPIO_PinState F_D);

void Trans_Fire(void);

void Trans2_Led(uint8_t LED_1,uint8_t LED_2);
void Handle_Led(uint8_t X_Led);
void Delay_Timer_1s(void);
void Delay_Timer_Ms(uint32_t x);
void Ana_Sensor(void);
void Handle_Sensor(GPIO_PinState S0,GPIO_PinState S1,GPIO_PinState S2,GPIO_PinState S3);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){	//1ms
 if(htim -> Instance == htim1.Instance){
	// if(Allow_F_Fire == 1)		 
			Count++;
 }
}

void Systick_Delay(unsigned int x){
	SysTick -> LOAD = 8000000/1000*x;
	SysTick -> VAL = 0;
	SysTick -> CTRL =5;
	while((SysTick->CTRL & 1 <<16) == 0);	
}

void Delay_Timer_1s(void){
	__HAL_TIM_SetCounter(&htim1,0);
	if(Count == 260)
	Read_Timer = __HAL_TIM_GetCounter(&htim1);
//	while (__HAL_TIM_GetCounter(&htim1) < 1000)	
}

void Delay_Timer_Ms(uint32_t x){
	while (x){
		Delay_Timer_1s();
		--x;
	}
}
void Ana_ADC(GPIO_PinState A, GPIO_PinState B, GPIO_PinState C, GPIO_PinState D){
	HAL_GPIO_WritePin(S0_ADC_GPIO_Port,S0_ADC_Pin,A);
	HAL_GPIO_WritePin(S1_ADC_GPIO_Port,S1_ADC_Pin,B);
	HAL_GPIO_WritePin(S2_ADC_GPIO_Port,S2_ADC_Pin,C);
	HAL_GPIO_WritePin(S3_ADC_GPIO_Port,S3_ADC_Pin,D);
	Sig_ADC_Vol = HAL_GPIO_ReadPin(Sig_ADC_Vol_GPIO_Port,Sig_ADC_Vol_Pin);
}

void Handle_ADC(void){
		
}
void Do_Fire(int8_t pipeIdx){  // pipeIdx = 1...18 (default -1)
	switch (pipeIdx){
		case 1: // Relay Index 15
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		case 13:
			break;
		case 14:
			break;
		case 15:
			break;
		case 16: 
			break;
		case 17:
			break;
		case 18:
			break;
		default: // -1
			DISABLE_Fire_PIN;
			HAL_GPIO_WritePin(LED_17_GPIO_Port,LED_17_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_16_GPIO_Port,LED_16_Pin,GPIO_PIN_RESET);
			break;		
	}
}
//====== Doc tin hieu Nut Nhan chong ong phong  và hien thi den ong phong======//
void Handle_Button(void){
	HAL_GPIO_WritePin(E1_1_GPIO_Port,E1_1_Pin,GPIO_PIN_RESET);
	E0_Button_1 = HAL_GPIO_ReadPin(E0_Button_1_GPIO_Port,E0_Button_1_Pin);
	HAL_GPIO_WritePin(E1_2_GPIO_Port,E1_2_Pin,GPIO_PIN_RESET);
	E0_Button_2 = HAL_GPIO_ReadPin(E0_Button_2_GPIO_Port,E0_Button_2_Pin);
	
	A0_Button_2 = HAL_GPIO_ReadPin(A0_Button_2_GPIO_Port,A0_Button_2_Pin);
	A1_Button_2 = HAL_GPIO_ReadPin(A1_Button_2_GPIO_Port,A1_Button_2_Pin);
	A2_Button_2 = HAL_GPIO_ReadPin(A2_Button_2_GPIO_Port,A2_Button_2_Pin);
	//	if(E0_Button_1 == 1)
		
	A0_Button_1 = HAL_GPIO_ReadPin(A0_Button_1_GPIO_Port,A0_Button_1_Pin);
	A1_Button_1 = HAL_GPIO_ReadPin(A1_Button_1_GPIO_Port,A1_Button_1_Pin);
	A2_Button_1 = HAL_GPIO_ReadPin(A2_Button_1_GPIO_Port,A2_Button_1_Pin);
	// Doc nut nhan phan 1: (SN74HC148_U3)
	if(A2_Button_1 == 0 && A1_Button_1 == 0 && A0_Button_1 == 0 && Sig_Sensor[0] == 1 && E0_Button_1 ==1 && A1 == 0x00 && A01 == 0x00)A1 = 0x01; // Den 1
	else if( A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[0] == 1 && E0_Button_1 ==0 && A1 == 0x01) A01 = 0x01;
	if(A01 == 0x01 && A2_Button_1 == 0 && A1_Button_1 == 0 && A0_Button_1 == 0 && Sig_Sensor[0] == 1 && E0_Button_1 ==1 && A1 == 0x01){
		A1 = 0x00;
		//A01 = 0x00;
	}
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	//			 
	if(A2_Button_1 == 0 && A1_Button_1 == 0 && A0_Button_1 == 1 && Sig_Sensor[1] == 1 && A2 == 0x00 && A02 == 0x00){
		A2 = 0x02; // Den 2	
		A02 = 0x00;
	}
	if( A2 == 0x02 && A02 == 0x00 &&A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[1] == 1 && E0_Button_1 == 0 )	A02 = 0x02; 
	if(A02 == 0x02 && A2 == 0x02 && A2_Button_1 == 0 && A1_Button_1 == 0 && A0_Button_1 == 1 && Sig_Sensor[1] == 1 && E0_Button_1 == 1 )	A2 = 0x00;
	if(A2==0x00 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[1] == 1 && E0_Button_1 == 0)	A02 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	// OK
	if(A2_Button_1 == 0 && A1_Button_1 == 1 && A0_Button_1 == 0 && Sig_Sensor[2] == 1 && E0_Button_1 ==1 && A3 == 0x00 && A03 ==0x00){
		A3 = 0x04; 
		A03 = 0x00;// Den 3
	}
	if(A3 == 0x04 && A03 ==0x00 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[2] == 1 && E0_Button_1 ==0)				A03 = 0x04; // biet dc trang thai nha nut nhan
	if(A3 == 0x04 && A03 == 0x04 && A2_Button_1 == 0 && A1_Button_1 == 1 && A0_Button_1 == 0 && Sig_Sensor[2] == 1 && E0_Button_1 == 1 ) 	 A3 = 0x00;// nhan lai 1 lan nua
	if(A3 == 0x00 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[2] == 1 && E0_Button_1 ==0)	A03 = 0x00; // biet dc trang thai nha nut nhan		 					 		 
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	//			
	if(A2_Button_1 == 0 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[3] == 1 && A4 == 0x00 && A04 == 0x00){
		A4 = 0x08; // Den 4	
		A04 = 0x00;
	}
	if(A4 == 0x08 && A04 == 0x00 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[3] == 1 && E0_Button_1 ==0 ) A04 = 0x08;
	if(A4==0x08 && A04 == 0x08 && A2_Button_1 == 0 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[3] == 1 && E0_Button_1 == 1 ) A4 = 0x00;
	if(A4==0x00 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[3] == 1 && E0_Button_1 ==0)	A04 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	//		
	if(A2_Button_1 == 1 && A1_Button_1 == 0 && A0_Button_1 == 0 && Sig_Sensor[4] == 1 && A05== 0x00 && A5 ==0x00){	
		A5 = 0x10; // Den 5
		A05 = 0x00;
	}
	if(A5 == 0x10 && A05 == 0x00 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[4] == 1 && E0_Button_1 ==0) A05 = 0x10;
	if(A5 == 0x10 && A05 == 0x10 && A2_Button_1 == 1 && A1_Button_1 == 0 && A0_Button_1 == 0 && Sig_Sensor[4] == 1 && E0_Button_1 ==1) A5 = 0x00;
	if(A5 == 0x00 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[4] == 1 && E0_Button_1 ==0)								A05 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	//			
	if(A2_Button_1 == 1 && A1_Button_1 == 0 && A0_Button_1 == 1 && Sig_Sensor[5] == 1 && A06 == 0x00 && A6 == 0x00){
		A6 = 0x20; // Den 6
		A06 = 0x00;
	}
	if(A6 == 0x20 && A06 == 0x00 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[5] == 1 && E0_Button_1 ==0)	A06 = 0x20;
	if(A06 == 0x20 && A6 == 0x20 && A2_Button_1 == 1 && A1_Button_1 == 0 && A0_Button_1 == 1 && Sig_Sensor[5] == 1 && E0_Button_1 ==1)	A6 = 0x00;
	if(A6 == 0x20 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[5] == 1 && E0_Button_1 ==0)									A06 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	//		
	if(A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 0 && Sig_Sensor[6] == 1 && A07 == 0x00 && A7 == 0x00){
		A7 = 0x40; // Den 7
		A07 = 0x00;
	}
	if(A7 == 0x40 && A07 ==0x00 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[6] == 1 && E0_Button_1 == 0) A07 = 0x40;
	if(A07 == 0x40 && A7 == 0x40 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 0 && Sig_Sensor[6] == 1 && E0_Button_1 ==1)  A7 = 0x00;
	if(A7 == 0x40 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[6] == 1 && E0_Button_1 == 0) A07 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	//		
	if(A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[7] == 1 && E0_Button_1 ==1 && A08 == 0x00 && A8 == 0x00){
		A8 = 0x80; // Den 8
		A08 = 0x00;
	}
	if(A8 == 0x80 && A08 == 0x00 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[7] == 1 && E0_Button_1 ==0)     A08 = 0x80;
	if(A8 == 0x80 && A08 == 0x80 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[7] == 1 && E0_Button_1 ==0)  	  A8 = 0x00;
	if(A8 == 0x80 && A2_Button_1 == 1 && A1_Button_1 == 1 && A0_Button_1 == 1 && Sig_Sensor[7] == 1 && E0_Button_1 ==0)								  	A08 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	
	// Doc nut nhan phan 2:  (SN74HC148_U4)
	if(A2_Button_2 == 0 && A1_Button_2 == 1 && A0_Button_2 ==1 && Sig_Sensor[10] == 1 && E0_Button_2 ==1 && A11 == 0x00 && A011 == 0x00){
		A11 = 0x04; // Den 11
		A011 = 0x00;
	}
	if(A11 == 0x04 && A011 == 0x00 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[10] == 1 && E0_Button_2 ==0 ) 	A011 = 0x04;
	if(A11 == 0x04 && A011 == 0x04 && A2_Button_2 == 0 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[10] == 1 && E0_Button_2 ==1)	  A11 = 0x00;
	if(A11 == 0x04 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[10] == 1 && E0_Button_2 ==0) 									A011 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	//			
	if(A2_Button_2 == 0 && A1_Button_2 == 1 && A0_Button_2 == 0 && Sig_Sensor[11] == 1 && A12 == 0x00 && A012 == 0x00){
		A12 = 0x08; // Den 12
		A012 = 0x00;
	}
	if(A12 == 0x08 && A012 == 0x00 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[11] == 1 && E0_Button_2 ==0) A012 = 0x08;
	if(A12 == 0x08 && A012 == 0x08 && A2_Button_2 == 0 && A1_Button_2 == 1 && A0_Button_2 == 0 && Sig_Sensor[11] == 1 && E0_Button_2 ==1) A12 = 0x00;
	if(A12 == 0x08 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[11] == 1 && E0_Button_2 ==0) 								A012 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	//			
	if(A2_Button_2 == 0 && A1_Button_2 == 0 && A0_Button_2 ==1 && Sig_Sensor[12] == 1 && A13 == 0x00 && A013 == 0x00){
		A13 = 0x10; // Den 13
		A013 = 0x00;
	}
	if(A13 == 0x10 && A013 == 0x00 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[12] == 1 && E0_Button_2 ==0) 	 A013 = 0x10;
	if(A13 == 0x10 && A013 == 0x10 && A2_Button_2 == 0 && A1_Button_2 == 0 && A0_Button_2 ==1 && Sig_Sensor[12] == 1  && E0_Button_2 ==1)		 A13 = 0x00;
	if(A13 == 0x10 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[12] == 1 && E0_Button_2 ==0) 									 A013 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	//			
	if(A2_Button_2 == 0 && A1_Button_2 == 0 && A0_Button_2 ==0 && Sig_Sensor[13] == 1 && A14 == 0x00 && A013 == 0x00){	
		A14 = 0x20; // Den 14
		A014 = 0x00;
	}
	if(A14 == 0x20 && A014 == 0x00 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[13] == 1 && E0_Button_2 ==0)	A014 = 0x20;
	if(A14 == 0x20 && A014 == 0x20 && A2_Button_2 == 0 && A1_Button_2 == 0 && A0_Button_2 == 0 && Sig_Sensor[13] == 1 && E0_Button_2 ==1) A14 =0x00;
	if(A14 == 0x20 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[13] == 1 && E0_Button_2 ==0) 								A014 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);		
	//			
	if(A2_Button_2 == 1 && A1_Button_2 == 0 && A0_Button_2 ==0 && Sig_Sensor[14] == 1 && A15 == 0x00 && A015 == 0x00){	
		A15 = 0x40; // Den 15
		A015 = 0x00;
	}
	if(A15 == 0x40 && A015 == 0x00 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[14] == 1 && E0_Button_2 ==0)A015 = 0x40;
	if(A15 == 0x40 && A015 == 0x40 && A2_Button_2 == 1 && A1_Button_2 == 0 && A0_Button_2 == 0 && Sig_Sensor[14] == 1 && E0_Button_2 ==1)A15 = 0x00;
	if(A15 == 0x40 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[14] == 1 && E0_Button_2 ==0) A015 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	//			
	if(A2_Button_2 == 1 && A1_Button_2 == 0 && A0_Button_2 ==1 && Sig_Sensor[15] == 1 && A16 == 0x00 && A016 == 0x00 ){
		A16 = 0x80; // Den 16
		A016 = 0x00;
	}
	if(A16 == 0x80 && A016 == 0x00 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[15] == 1 && E0_Button_2 ==0) 	 A016 = 0x80;
	if(A16 == 0x80 && A016 == 0x80 && A2_Button_2 == 1 && A1_Button_2 == 0 && A0_Button_2 == 1 && Sig_Sensor[15] == 1 && E0_Button_2 ==1) 	 A16 = 0x00;
	if(A16 == 0x80 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sig_Sensor[15] == 1 && E0_Button_2 ==0) 									 A016 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	//			
	if(A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 ==0 && Sensor_16 == 1 && A17 == 0x00 && A017 == 0x00){
		A17 = 0x01; // Den 17
		A017 = 0x00;
	}
	if(A17 == 0x01 && A017 == 0x00 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sensor_16 == 1 && E0_Button_2 ==0) 	 A017 = 0x01;
	if(A17 == 0x01 && A017 == 0x00 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 0 && Sensor_16 == 1 && E0_Button_2 ==1) 	 A17 = 0x00;
	if(A17 == 0x01 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sensor_16 == 1 && E0_Button_2 ==0) 									 A017 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
	//			
	if(A2_Button_2 == 1 && A2_Button_2 == 1 && A0_Button_2 ==1 && Sensor_17 == 1 && E0_Button_2 ==1 && A18 == 0x00 && A018 == 0x00){
		A18 = 0x02; // Den 18
		A018 = 0x00;
	}
	if(A18 == 0x02 && A018 == 0x00 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sensor_17 == 1 && E0_Button_2 ==0) 	A018 = 0x02;
	if(A18 == 0x02 && A018 == 0x02 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sensor_17 == 1 && E0_Button_2 ==1)	A18 = 0x00;
	if(A18 == 0x02 && A2_Button_2 == 1 && A1_Button_2 == 1 && A0_Button_2 == 1 && Sensor_17 == 1 && E0_Button_2 ==0) 									A018 = 0x00;
	Trans2_Led(A11|A12|A13|A14|A15|A16|A17|A18,A1|A2|A3|A4|A5|A6|A7|A8);
		
	Button_8 = HAL_GPIO_ReadPin(Button_8_GPIO_Port,Button_8_Pin); // den 9
	if(Button_8 == 0 && Sig_Sensor[8] == 1 && A09 == 0 && A9 == 0){			
		A9 = 1;
		A09 = 0;
  }
	if(A9 == 1 && A09 == 0 && Button_8 == 1 && Sig_Sensor[8] == 1)	A09 =1;
	if(A9 == 1 && A09 == 1 && Button_8 == 0 && Sig_Sensor[8] == 1)  A9 = 0;
	if(A9 == 1 && Button_8 == 1 && Sig_Sensor[8] == 1) 							A09 = 0;
	HAL_GPIO_WritePin(LED_16_GPIO_Port,LED_16_Pin,(GPIO_PinState)A9); 					// de y dau day cho den so 8 (dau ra LED16 vao den so 8)
	
	Button_9 = HAL_GPIO_ReadPin(Button_9_GPIO_Port,Button_9_Pin); // den 10
	if(Button_9 == 0 && Sig_Sensor[9] == 1 && A10 == 0 && A010 == 0){
		A10 = 1;
		A010 = 0;
	}
	if(A10 == 1 && A010 == 0 && Button_9 == 1 && Sig_Sensor[9] == 1) A010 = 1;
	if(A10 == 1 && A010 == 1 && Button_9 == 0 && Sig_Sensor[9] == 1) A10 = 0;
	if(A10 == 1 && Button_9 == 1 && Sig_Sensor[9] == 1) 						 A010 = 0;
	HAL_GPIO_WritePin(LED_17_GPIO_Port,LED_17_Pin,(GPIO_PinState)A10);	// de y dau day cho den so 9 (dau ra LED17 vao den so 9)
}
					
void Ana_Button_Fire(GPIO_PinState F_A, GPIO_PinState F_B, GPIO_PinState F_C, GPIO_PinState F_D){
	HAL_GPIO_WritePin(S0_Fire_GPIO_Port,S0_Fire_Pin,F_A);
	HAL_GPIO_WritePin(S1_Fire_GPIO_Port,S1_Fire_Pin,F_B);
	HAL_GPIO_WritePin(S2_Fire_GPIO_Port,S2_Fire_Pin,F_C);
	HAL_GPIO_WritePin(S3_Fire_GPIO_Port,S3_Fire_Pin,F_D);
	/*
	0000 : 0     -- ong phong
	1000 : 1     -- ong phong
	0100 : 2     -- ong phong
	1100 : 3     -- ong phong
	0010 : 4     -- ong phong
	1010 : 5     -- ong phong
	0110 : 6     -- ong phong
	1110 : 7     -- ong phong
	
	0001 : 8
	1001 : 9
	0101 : 10
	1101 : 11
	0011 : 12
	1011 : 13
	0111 : 14
	1111 : 15
	       16 - MCU pin direct
	       17 - MCU pin direct
	*/
}
// ====== Cho phep ban ======//
void Allow_Fire(void){
	Mode_F = HAL_GPIO_ReadPin(Mode_Fire_GPIO_Port,Mode_Fire_Pin);
	Allow_F = HAL_GPIO_ReadPin(Allow_Fire_GPIO_Port,Allow_Fire_Pin);
	DISABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
	
	if(Allow_F == 1) Allow_F_Fire = 1;
	if(Allow_F_Fire == 1 && Mode_F ==0){	
		if(A15 == 0x40){	//15
			// __HAL_TIM_SetCounter(&htim1,0);
			ENABLE_Fire_PIN;// HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1);
			A15 = 0x00;
			Delay_Timer_1s();
			HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}
		if(A4 == 0x08){   //4
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0);
			A4 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN;// HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}
		if(A8 == 0x80){//8
			ENABLE_Fire_PIN;// HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0);
			A8 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}
		if(A11 == 0x04){//11
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)1);
			A11 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; //HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}
		if(A3 == 0x04){//3
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0);
			A3 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; //HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}
		if(A16 == 0x80){//16
			ENABLE_Fire_PIN; //HAL_GPIO_WritePin(Fire_16_GPIO_Port,Fire_16_Pin,GPIO_PIN_SET);	
			A16 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(Fire_16_GPIO_Port,Fire_16_Pin,GPIO_PIN_RESET);
		}
		if(A14 == 0x20){//14
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1);
			A14 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}
		if(A5 == 0x10){//5
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0);
			A5 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}
		if(A2 == 0x02){//2
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0);
			A2 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}
		if(A17 == 0x01){//17
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(Fire_17_GPIO_Port,Fire_17_Pin,GPIO_PIN_SET);
			A17 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(Fire_17_GPIO_Port,Fire_17_Pin,GPIO_PIN_RESET);
		}
		if(A9 == 1){//9
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1);
			A9=0x00;
		  HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}
		if(A12 == 0x08){//12
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)1);
			A12 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
	  }
		if(A7 == 0x40){//7
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0);
			A7 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}
		if(A10 == 1){//10
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1);	
			A10=0x00;
		  HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}

		if(A13 == 0x10){//13
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1);
			A13 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; //HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}
		if(A6 == 0x20){//6
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0);
			A6 = 0x00;
			DISABLE_Fire_PIN; // HAL_Delay(260);
		}

		if(A1 == 0x01){//1
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0); 
			A1 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}

		if(A18 == 0x02){//18
			ENABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1);
			A18 = 0x00;
			HAL_Delay(260);
			DISABLE_Fire_PIN; // HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		}
		
		Allow_F_Fire = 0;
		HAL_GPIO_WritePin(LED_17_GPIO_Port,LED_17_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_16_GPIO_Port,LED_16_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		//	HAL_GPIO_WritePin(Fire_17_GPIO_Port,Fire_17_Pin,GPIO_PIN_RESET);
		//	HAL_GPIO_WritePin(Fire_16_GPIO_Port,Fire_16_Pin,GPIO_PIN_RESET);
	}	
}

void Mode_Fire(void){
	switch(State){
		case IDLE:{
			if(Mode_F == 0) State = State_1;
			else if(Mode_F == 1) State = State_2;
			break;
		}
		case State_1:{ // Ban bang tay
			Handle_Button();
			Allow_Fire();
			if(Mode_F == 1) State = State_2;
			break;
		}
		case State_2:{
		//Index - (so phong phong - gian do ban) 1(17) - nghia là Index = 1, ông phong so 17
		/*
		1(17)		2(9)		3(5)		4(2)		5(8)		6(16)
		7(13)		8(3)		9(11)		10(14)	11(4)		12(12)
		13(15)	14(7)		15(1)		16(6)		17(10)	18(18)
		*/		
		Mode_F = HAL_GPIO_ReadPin(Mode_Fire_GPIO_Port,Mode_Fire_Pin);
		HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
		Allow_F = HAL_GPIO_ReadPin(Allow_Fire_GPIO_Port,Allow_Fire_Pin);
		if(Allow_F == 1)  Allow_F_Fire = 1;
		if(Mode_F == 1 && Allow_F_Fire == 1){		
			HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1);//15
			HAL_Delay(266);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0);//4
			HAL_Delay(266);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0);//8
			HAL_Delay(266);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)1);//11
			HAL_Delay(266);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0);//3
			HAL_Delay(266);
			HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Fire_16_GPIO_Port,Fire_16_Pin,GPIO_PIN_SET);
			HAL_Delay(266);
			HAL_GPIO_WritePin(Fire_16_GPIO_Port,Fire_16_Pin,GPIO_PIN_RESET);
			//
			HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1);//14
			HAL_Delay(266);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0); //5
			HAL_Delay(266);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0);//2
			HAL_Delay(266);
			//
			HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Fire_17_GPIO_Port,Fire_17_Pin,GPIO_PIN_SET);
			HAL_Delay(266);
			HAL_GPIO_WritePin(Fire_17_GPIO_Port,Fire_17_Pin,GPIO_PIN_RESET);		
			//
			HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_RESET);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1); //9
			HAL_Delay(266);

			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)1);//12
			HAL_Delay(266);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0);//7
			HAL_Delay(266);	
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1); //10
			HAL_Delay(266);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1);//13
			HAL_Delay(266);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0); //6
			HAL_Delay(266);
			Ana_Button_Fire((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0); //1
			HAL_Delay(266);
			Ana_Button_Fire((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1);//18
			HAL_Delay(266);
			HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET); 
			Allow_F_Fire =0;
		}
		if(Mode_F == 0) State = State_1;
		}
		break;
	}

}
// ====== Xu ly xuat tin hieu LED ======//
void Handle_Led(uint8_t X_Led)
{
	for(int i =0;i<8;i++){
		if((X_Led & 0x80) == 0x80)
			HAL_GPIO_WritePin(DATA_GPIO_Port,DATA_Pin,GPIO_PIN_SET);
		else 
			HAL_GPIO_WritePin(DATA_GPIO_Port,DATA_Pin,GPIO_PIN_RESET);
		
		HAL_GPIO_WritePin(CLK__GPIO_Port,CLK__Pin,(GPIO_PinState)1); HAL_Delay(1);
		HAL_GPIO_WritePin(CLK__GPIO_Port,CLK__Pin,(GPIO_PinState)0); HAL_Delay(1);
		
		X_Led = X_Led<<1;
	}
}

//====== Xuat tin hieu LED ======//
void Trans2_Led(uint8_t LED_1,uint8_t LED_2){
	Handle_Led(LED_1);
	Handle_Led(LED_2);
	HAL_GPIO_WritePin(EN__GPIO_Port,EN__Pin,(GPIO_PinState)1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(EN__GPIO_Port,EN__Pin,(GPIO_PinState)0);
	HAL_Delay(1);	
}
//====== Dieu khien tin hieu cam bien ======//
void Handle_Sensor(GPIO_PinState S0,GPIO_PinState S1,GPIO_PinState S2,GPIO_PinState S3){
	HAL_GPIO_WritePin(S0_Sensor_GPIO_Port,S0_Sensor_Pin,S0);
	HAL_GPIO_WritePin(S1_Sensor_GPIO_Port,S1_Sensor_Pin,S1);
	HAL_GPIO_WritePin(S2_Sensor_GPIO_Port,S2_Sensor_Pin,S2);
	HAL_GPIO_WritePin(S3_Sensor_GPIO_Port,S3_Sensor_Pin,S3);
}
//====== Doc tin hieu trang thai dan(rocket) trong ong phong(pipe) ======//
void SensorSelection(int32_t PipeIdx){ // PipeIdx = 1..18, default = -1 
	//Index - (so phong phong - gian do ban) 1(17) - nghia là Index = 1, ông phong so 17
	/*
		1(17)		2(9)		3(5)		4(2)		5(8)		6(16)
		7(13)		8(3)		9(11)		10(14)	11(4)		12(12)
		13(15)	14(7)		15(1)		16(6)		17(10)	18(18)
	*/
	switch(PipeIdx){
		case 1:
			Handle_Sensor((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1);//  1111 - 15
			break;
		case 2:
			Handle_Sensor((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0);// 0010 - 4
			break;
		case 3:
			Handle_Sensor((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1);// 0001 - 8
			break;
		case 4:
			Handle_Sensor((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1);// 1101 - 11
			break;
		case 5:
			Handle_Sensor((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0);// 1100 - 3
			break;
		case 6:
			Sensor_16 = HAL_GPIO_ReadPin(Sensor_16_GPIO_Port,Sensor_16_Pin);                   // 16
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12: 
			break;
		case 13:
			break;
		case 14:
			break;
		case 15:
			break;
		case 16:
			break;
		case 17:
			break;
		case 18:
			break;
		default:
			break;
	}
}

void Ana_Sensor(void){
	/*
	
	*/
	Handle_Sensor((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0);// 0
	Sig_Sensor[0]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0);// 1
	Sig_Sensor[1]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0);// 2
	Sig_Sensor[2]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0);// 3 
	Sig_Sensor[3]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0);// 4
	Sig_Sensor[4]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0);// 5
	Sig_Sensor[5]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0);// 6
	Sig_Sensor[6]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0);// 7
	Sig_Sensor[7]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1);// 8
	Sig_Sensor[8]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1);// 9
	Sig_Sensor[9]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1);// 10
	Sig_Sensor[10]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1);// 11
	Sig_Sensor[11]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)0,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)1);// 12
	Sig_Sensor[12]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)1,(GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)1);// 13
	Sig_Sensor[13]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)0,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1);// 14
	Sig_Sensor[14]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Handle_Sensor((GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1,(GPIO_PinState)1);// 15
	Sig_Sensor[15]= HAL_GPIO_ReadPin(Sig_Sensor_GPIO_Port,Sig_Sensor_Pin);
	Sensor_16 = HAL_GPIO_ReadPin(Sensor_16_GPIO_Port,Sensor_16_Pin);                   // 16
	Sensor_17 = HAL_GPIO_ReadPin(Sensor_17_GPIO_Port,Sensor_17_Pin);                   //17
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_CAN_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1,&Rx_Data,1);
	HAL_GPIO_WritePin(DE_485_1_GPIO_Port,DE_485_1_Pin,GPIO_PIN_RESET);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ADC_Out,3);
	HAL_GPIO_WritePin(InHiBit_Fire_GPIO_Port,InHiBit_Fire_Pin,GPIO_PIN_SET);
	HAL_TIM_Base_Start_IT(&htim1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1){
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		Ana_Sensor();	
    Mode_Fire();
		Delay_Timer_1s();
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void){
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK){
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK){
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void){

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 3;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void){

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK){
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void){

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 7999;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xFFFF;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 79;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 99;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, E1_1_Pin|S0_Fire_Pin|CLK__Pin|LED_16_Pin
                          |LED_17_Pin|WRN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, S2_ADC_Pin|S3_ADC_Pin|S0_ADC_Pin|S1_ADC_Pin
                          |DE_485_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, S1_Fire_Pin|S3_Fire_Pin|S2_Fire_Pin|InHiBit_Fire_Pin
                          |Fire_17_Pin|Fire_16_Pin|DATA_Pin|EN__Pin
                          |S3_Sensor_Pin|S2_Sensor_Pin|S1_Sensor_Pin|S0_Sensor_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(E1_2_GPIO_Port, E1_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : Button_9_Pin Button_8_Pin A0_Button_1_Pin A2_Button_1_Pin
                           Mode_Fire_Pin E0_Button_2_Pin A1_Button_2_Pin A2_Button_2_Pin */
  GPIO_InitStruct.Pin = Button_9_Pin|Button_8_Pin|A0_Button_1_Pin|A2_Button_1_Pin
                          |Mode_Fire_Pin|E0_Button_2_Pin|A1_Button_2_Pin|A2_Button_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : E0_Button_1_Pin A1_Button_1_Pin */
  GPIO_InitStruct.Pin = E0_Button_1_Pin|A1_Button_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : E1_1_Pin S0_Fire_Pin WRN_Pin */
  GPIO_InitStruct.Pin = E1_1_Pin|S0_Fire_Pin|WRN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : S2_ADC_Pin S3_ADC_Pin S0_ADC_Pin S1_ADC_Pin
                           DE_485_1_Pin */
  GPIO_InitStruct.Pin = S2_ADC_Pin|S3_ADC_Pin|S0_ADC_Pin|S1_ADC_Pin
                          |DE_485_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Allow_Fire_Pin A0_Button_2_Pin */
  GPIO_InitStruct.Pin = Allow_Fire_Pin|A0_Button_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : S1_Fire_Pin S3_Fire_Pin S2_Fire_Pin InHiBit_Fire_Pin
                           Fire_17_Pin Fire_16_Pin S3_Sensor_Pin S2_Sensor_Pin
                           S1_Sensor_Pin S0_Sensor_Pin */
  GPIO_InitStruct.Pin = S1_Fire_Pin|S3_Fire_Pin|S2_Fire_Pin|InHiBit_Fire_Pin
                          |Fire_17_Pin|Fire_16_Pin|S3_Sensor_Pin|S2_Sensor_Pin
                          |S1_Sensor_Pin|S0_Sensor_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DATA_Pin EN__Pin */
  GPIO_InitStruct.Pin = DATA_Pin|EN__Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : CLK__Pin LED_16_Pin LED_17_Pin */
  GPIO_InitStruct.Pin = CLK__Pin|LED_16_Pin|LED_17_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : E1_2_Pin */
  GPIO_InitStruct.Pin = E1_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(E1_2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Sensor_17_Pin Sensor_16_Pin Sig_Sensor_Pin */
  GPIO_InitStruct.Pin = Sensor_17_Pin|Sensor_16_Pin|Sig_Sensor_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure peripheral I/O remapping */
  __HAL_AFIO_REMAP_PD01_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
