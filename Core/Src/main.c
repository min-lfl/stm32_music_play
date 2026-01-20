/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "OLED_IIC_Config.h"
#include "OLED_Function.h"
#include "OLED_Front.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint8_t Date1[] = {0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x02, 0xEF};	//选择SD卡
uint8_t Date2[] = {0x7E, 0xFF, 0x06, 0x0F, 0x00, 0x01, 0x01, 0xEF};	//选择01文件夹内的第一首
uint8_t Date3[] = {0x7E, 0xFF, 0x06, 0x06, 0x00, 0x00, 0x11, 0xEF};	//调节音量为，最大为30
uint8_t Date4[] = {0x7E, 0xFF, 0x06, 0x0D, 0x00, 0x00, 0x00, 0xEF};	//开始播放
uint8_t Date5[] = {0x7E, 0xFF, 0x06, 0x01, 0x00, 0x00, 0x00, 0xEF};	//上一首
uint8_t Date6[] = {0x7E, 0xFF, 0x06, 0x02, 0x00, 0x00, 0x00, 0xEF};  //下一首
uint8_t Date7[] = {0x7E, 0xFF, 0x06, 0x17, 0x00, 0x01, 0x02, 0xEF};  //文件夹01循环播放
uint8_t Date8[] = {0x7E, 0xFF, 0x06, 0x04, 0x00, 0x00, 0x00, 0xEF};  //提高音量
uint8_t Date9[] = {0x7E, 0xFF, 0x06, 0x05, 0x00, 0x00, 0x00, 0xEF};  //降低音量
uint8_t Date10[] = {0x7E, 0xFF, 0x06, 0x0E, 0x00, 0x00, 0x00, 0xEF};  //暂停播放
uint8_t Date11[] = {0x7E, 0xFF, 0x06, 0x0C, 0x00, 0x00, 0x00, 0xEF}; //芯片重置
uint8_t key_old[] = {0x01,0x01,0x01,0x01};//4个按键状态缓存
uint8_t key_new[] = {0x01,0x01,0x01,0x01};//4个按键状态缓存
uint8_t Date_Volume[8] = {0x7E, 0xFF, 0x06, 0x06, 0x00, 0x00, 0x09, 0xEF};
uint8_t Date_Music[8]  = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x01, 0xEF};

uint8_t Volume=0x11;				//音量
uint8_t Music=0x01;					//当前音乐
static uint8_t Play=0x00;		//播放标志位,0x00是未播放，0x01是播放
uint8_t i=0;								//临时变量
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void USART1_Volume_Update(uint8_t Volume){
	Date_Volume[6] = Volume;
	HAL_UART_Transmit_IT(&huart1,Date_Volume,(sizeof(Date_Volume)/sizeof(Date_Volume[0])));
}

void USART1_Music_Update(uint8_t Music){
	Date_Music[6] = Music;
	HAL_UART_Transmit_IT(&huart1,Date_Music,(sizeof(Date_Music)/sizeof(Date_Music[0])));
//	HAL_Delay(100);
//	HAL_UART_Transmit_IT(&huart1,Date4,(sizeof(Date4)/sizeof(Date4[0])));//开始播放
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
  MX_USART1_UART_Init();
  MX_I2C1_Init();
	
  /* USER CODE BEGIN 2 */
	OLED_Init();  //OLED初始化
	
	//音乐播放模块初始化(加一点延时给反应时间)
	HAL_UART_Transmit_IT(&huart1,Date11,(sizeof(Date11)/sizeof(Date11[0])));//播放器重启
	HAL_Delay(300);
	HAL_UART_Transmit_IT(&huart1,Date1,(sizeof(Date1)/sizeof(Date1[0])));//选择SD卡
	HAL_Delay(100);
	HAL_UART_Transmit_IT(&huart1,Date2,(sizeof(Date2)/sizeof(Date2[0])));//选择01文件夹内的第一首
	HAL_Delay(100);
  HAL_UART_Transmit_IT(&huart1,Date7,(sizeof(Date7)/sizeof(Date7[0])));//循环播放:
	HAL_Delay(200);
	HAL_UART_Transmit_IT(&huart1,Date3,(sizeof(Date3)/sizeof(Date3[0])));//调节音量为，最大为30
	HAL_Delay(100);
	HAL_UART_Transmit_IT(&huart1,Date3,(sizeof(Date3)/sizeof(Date3[0])));//调节音量为，最大为30
	HAL_Delay(100);
	HAL_UART_Transmit_IT(&huart1,Date3,(sizeof(Date3)/sizeof(Date3[0])));//调节音量为，最大为30
	HAL_Delay(100);
	Play=0x00;
	
	//初始化音量显示
	OLED_ShowChinese(27,1,(unsigned char*)"：");
	OLED_ShowChinese(0,1,(unsigned char*)"音");
	OLED_ShowChinese(13,1,(unsigned char*)"量");
  OLED_ShowVolumeBar(34, 5, Volume, 30);
	OLED_ShowNum(97,1,Volume,2,2);
	
	//歌曲名显示部分
//	OLED_DrawBox(0,29,130,18,1);
	
	OLED_DrawBox(13,56,100,4,2);	//进度条
	OLED_PlayPauseIcon(55,48,0);	//播放键
	
	Show_Music_Name(16,24,1);			//歌曲名显示
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    //按键1:上一首
		key_old[0]=key_new[0];
		key_new[0]=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11);
		if(key_new[0]!=key_old[0]){
			
			if(key_new[0] == 0){				//按下时
				//播放上一首:
				if(Music>0)Music--;
        USART1_Music_Update(Music);
				Show_Music_Name(16,24,Music);
			}else if(key_new[0] == 1){	//松开时
				if(Play == 0x00){
					HAL_UART_Transmit_IT(&huart1,Date4,(sizeof(Date4)/sizeof(Date4[0])));//开始播放
					Play=0x01;							//置位为1表示开始播放
					OLED_PlayPauseIcon(55,48,1);//刷新播放ui
				}
			}
      HAL_Delay(10);              //简单消抖
		}

    //按键2：下一首
    key_old[1]=key_new[1];
    key_new[1]=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10);
    if(key_new[1]!=key_old[1]){
			
			if(key_new[1] == 0){				//按下时
				//播放下一首:
        if(Music<15)Music++;
        USART1_Music_Update(Music);
				Show_Music_Name(16,24,Music);
			}else if(key_new[1] == 1){	//松开时
				if(Play == 0x00){
					HAL_UART_Transmit_IT(&huart1,Date4,(sizeof(Date4)/sizeof(Date4[0])));//开始播放
					Play=0x01;							//置位为1表示开始播放
					OLED_PlayPauseIcon(55,48,1);//刷新播放ui
					
				}
			}
      HAL_Delay(10);              //简单消抖
		}
		
    //按键3：音量减
    key_old[2]=key_new[2];
    key_new[2]=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2);
    if(key_new[2]!=key_old[2]){
      if(key_new[2] == 0){        //按下时

      }else if(key_new[2] == 1){ //松开时
        //音量加:
				if(Volume>0)Volume--;
        USART1_Volume_Update(Volume);
				//更新显示
				OLED_ShowVolumeBar(34, 5, Volume, 30);
				OLED_ShowNum(97,1,Volume,2,2); 
      }
      HAL_Delay(10);              //简单消抖
    }

    //按键4：音量加
    key_old[3]=key_new[3];
    key_new[3]=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
    if(key_new[3]!=key_old[3]){
      if(key_new[3] == 0){        //按下时
        
      }else if(key_new[3] == 1){ //松开时
        //音量减:
				if(Volume<30)Volume++;
        USART1_Volume_Update(Volume);
				//更新显示
				OLED_ShowVolumeBar(34, 5, Volume, 30);
				OLED_ShowNum(97,1,Volume,2,2);    
      }
      HAL_Delay(10);              //简单消抖
    }
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  huart1.Init.BaudRate = 9600;
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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PA0 PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
