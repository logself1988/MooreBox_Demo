/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
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
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "hard_init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void LED_Blink(void);


void LED_Blink(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_2|GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);
	delay_ms(500);
	GPIO_ResetBits(GPIOC, GPIO_Pin_2);
	delay_ms(500);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void) {
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */
	hardware_init();
	// turn off buffers, so IO occurs immediately
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
   
	GPIO_SetBits(GPIOC, GPIO_Pin_2|GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);

	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST)!=RESET){
		GPIO_ResetBits(GPIOC, GPIO_Pin_0);
		RCC_ClearFlag();
	}
  /* Infinite loop */
  while (1) {
	LED_Blink();
	IWDG_ReloadCounter();       //feed the dog
	dbg_printf("Hello\r\n");
}
}


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line) {
  /* User can add his own implementation to report the file name and line
     number,
     ex: dbg_printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	while(1){}
}

#endif







/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
