#ifndef __STM32_CHIPSET_H
#define __STM32_CHIPSET_H

#define DEBUG
#ifdef  DEBUG
#define dbg_printf(FORMAT, ...) \
    printf("%s() in %s, line %i: " FORMAT, \
      __func__, __FILE__, __LINE__, ##__VA_ARGS__)
#define dbg_puts(MSG) dbg_printf("%s", MSG)
#else
#define dbg_printf(FORMAT, ...) ((void)0)
#define dbg_puts(MSG) ((void)0)
#endif

typedef unsigned char  u8;
typedef unsigned short u16;

extern void IWDG_Configuration(void);
extern void RCC_Configuration(void);
extern void NVIC_Configuration(void);
extern void GPIO_Configuration(void);
extern void USART_Configuration(void);
extern void Init_TIM2(void);
#endif/* __STM32_CHIPSET_H */
