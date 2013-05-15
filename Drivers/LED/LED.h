/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#ifndef __LED_H__
#define __LED_H__
//
// Macros definitions
//

// Alias name
#define LED1			GPIO_Pin_6
#define LED2			GPIO_Pin_15
#define LED3			GPIO_Pin_14
#define LED4			GPIO_Pin_13

extern uint16_t TimerPeriod;

//
// Exported functions declare
//
BOOL LED_Init (void);
BOOL LED_TurnOn (UINT16 usLed);
BOOL LED_TurnOff (UINT16 usLed);
BOOL LED_Update(char *status);
BOOL LED_Transit(char n,char old_val,char new_val);
BOOL LED_Flash (UINT16 usLed);

#endif // __LED_H__

