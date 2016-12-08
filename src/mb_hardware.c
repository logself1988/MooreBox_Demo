/**************************************
 * @file:mb_hardware.c
 * @brief:Fuctions related to the hardware layer.
 * ***********************************/

#include "mb_hardware.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "hard_init.h"


/****************************************
 * @fn:KeyboardGPIO_Config
 * @brief:initialize the keyboard pin map
 * @keymap:
 *   4 5 6
 *   - - -
 * 0|1 2 3
 * 1|4 5 6
 * 2|7 8 9
 * 3|s 0 g
 *
 * s--store  g--get
****************************************/
static void KeyboardGPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_SetBits(GPIOC, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
}

/***************************************
 * @fn:KeyboardDetect
 * @brief:Scan the keyboard and get the key value
 * ************************************/
uint8_t KeyboardDetect(void)
{
	uint8_t KeyValue=0x70;
	KeyboardGPIO_Config();

	if((GPIO_ReadInputData(GPIOC)&0x7F) != 0x70){
		Delay_ms(10);
		if((GPIO_ReadInputData(GPIOC)&0x7F) !=0x70){
			GPIO_SetBits(GPIOC, GPIO_Pin_4);
			GPIO_ResetBits(GPIOC, GPIO_Pin_5|GPIO_Pin_6);
			switch(GPIO_ReadInputData(GPIOC)&0x0F)
			{
				case 0x01: KeyValue = 1; break;
				case 0x02: KeyValue = 4; break;
				case 0x04: KeyValue = 7; break;
				case 0x08: KeyValue = 's'; break;
				default: KeyValue = 0x70; break;
			}

			GPIO_SetBits(GPIOC, GPIO_Pin_5);
			GPIO_ResetBits(GPIOC, GPIO_Pin_4|GPIO_Pin_6);
			switch(GPIO_ReadInputData(GPIOC)&0x7F)
			{
				case 0x01: KeyValue = 2; break;
				case 0x02: KeyValue = 5; break;
				case 0x04: KeyValue = 8; break;
				case 0x08: KeyValue = 0; break;
				default: KeyValue = 0x70; break;
			}

			GPIO_SetBits(GPIOC, GPIO_Pin_6);
			GPIO_ResetBits(GPIOC, GPIO_Pin_4|GPIO_Pin_5);
			switch(GPIO_ReadInputData(GPIOC)&0x7F)
			{
				case 0x01: KeyValue = 3; break;
				case 0x02: KeyValue = 6; break;
				case 0x04: KeyValue = 9; break;
				case 0x08: KeyValue = 'g'; break;
				default: KeyValue = 0x70; break;
			}

			GPIO_SetBits(GPIOC, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);
			GPIO_ResetBits(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
			while((GPIO_ReadInputData(GPIOA)&0x7F) != 0x70);
			return KeyValue;
		}
	}
	
}

/*******************************************
 * @fn  ScanBarCode
 * @brief Scan the bar code and pass the code to the MooreBox.
 * ****************************************/



