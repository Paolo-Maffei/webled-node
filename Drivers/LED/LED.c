/******************************
*******************************/

#include "Project.h"

#define PWM_PERIOD_COUNT  17570

uint16_t TimerPeriod = 0;

BOOL LED_Init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        TIM_OCInitTypeDef  TIM_OCInitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

        /* GPIO Configuration: LED1 */
	GPIO_InitStructure.GPIO_Pin = LED1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init (GPIOC, &GPIO_InitStructure);
        
        /* GPIO Configuration: LED2、LED3、LED4 */
        GPIO_InitStructure.GPIO_Pin = LED2 | LED3 | LED4;
        GPIO_Init (GPIOB, &GPIO_InitStructure);
        
	// init all LED 
        char io = NodeAttr_GetIO();
        if(0x01 & io)
          LED_TurnOn(LED1);
        else
          LED_TurnOff(LED1);
        
        if(0x02 & io)
          LED_TurnOn(LED2);
        else
          LED_TurnOff(LED2);
        
        if(0x04 & io)
          LED_TurnOn(LED3);
        else
          LED_TurnOff(LED3);
        
        if(0x08 & io)
          LED_TurnOn(LED4);
        else
          LED_TurnOff(LED4);

        //Init PWM 
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
        
        /* GPIOA Configuration: PWM Channel 1 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        /* GPIOC Configuration: PWM Channel 2, 3 and 4 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        
//        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
        TimerPeriod = (SystemCoreClock / PWM_PERIOD_COUNT ) - 1;
        
          /* Time Base configuration */
        TIM_TimeBaseStructure.TIM_Prescaler = 0;
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
        TIM_TimeBaseStructure.TIM_ClockDivision = 0;
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
        
        TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
        TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
        
        /* Channel 1, 2,3 and 4 Configuration in PWM mode */
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
        TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
        /*N is disable,resvered to init  here to TIM_OCInitStructure to avoid bug*/
        TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
        TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
        TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
        
        TIM_OCInitStructure.TIM_Pulse = (uint16_t)(((uint32_t)NodeAttr_GetPWM1() * (TimerPeriod - 1)) / 100);        
        TIM_OC1Init(TIM1, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
        
        TIM_OCInitStructure.TIM_Pulse = (uint16_t)(((uint32_t)NodeAttr_GetPWM2() * (TimerPeriod - 1)) / 100);
        TIM_OC4Init(TIM8, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
        
        TIM_OCInitStructure.TIM_Pulse = (uint16_t)(((uint32_t)NodeAttr_GetPWM3() * (TimerPeriod - 1)) / 100);
        TIM_OC3Init(TIM8, &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
        
        TIM_OCInitStructure.TIM_Pulse = (uint16_t)(((uint32_t)NodeAttr_GetPWM4() * (TimerPeriod - 1)) / 100);
        TIM_OC2Init(TIM8, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
        
        TIM_ARRPreloadConfig(TIM1, ENABLE);
        TIM_ARRPreloadConfig(TIM8, ENABLE);
        
        /* TIM1 counter enable */
        TIM_Cmd(TIM1, ENABLE);
        TIM_Cmd(TIM8, ENABLE);
        
        /* TIM1 Main Output Enable */
        TIM_CtrlPWMOutputs(TIM1, ENABLE);
        TIM_CtrlPWMOutputs(TIM8, ENABLE);

	return TRUE;
}

BOOL LED_TurnOn (UINT16 usLed)
{
        if(LED1 == usLed)
          GPIO_ResetBits (GPIOC, usLed);
        else
          GPIO_ResetBits (GPIOB, usLed);
	return TRUE;
}

BOOL LED_TurnOff(UINT16 usLed)
{
        if(LED1 == usLed)
          GPIO_SetBits (GPIOC, usLed);
        else
          GPIO_SetBits (GPIOB, usLed);
	return TRUE;
}

BOOL LED_SetLumin(char n,char pwm)
{
  switch(n){
  case 1:
    TIM_SetCompare1(TIM1,(uint16_t)(((uint32_t)pwm * (TimerPeriod - 1)) / 100));
    break;
  case 2:
    TIM_SetCompare4(TIM8,(uint16_t)(((uint32_t)pwm * (TimerPeriod - 1)) / 100));
    break;
  case 3:
    TIM_SetCompare3(TIM8,(uint16_t)(((uint32_t)pwm * (TimerPeriod - 1)) / 100));
    break;
  case 4:
    TIM_SetCompare2(TIM8,(uint16_t)(((uint32_t)pwm * (TimerPeriod - 1)) / 100));
    break;
  default:
    break;  
  }
  return 0;
}

static void LED_Delay(unsigned int t) //5000 per ms
{
  __IO uint32_t i = 0;
  for(i ; i < t; i++)
  {
  }
}
//change pwm value gradually
BOOL LED_Transit(char n,char old_val,char new_val)
{
  if(old_val < new_val)   //turn dark
  {
    while(old_val < new_val)
    {
       LED_SetLumin(n,++old_val);
       LED_Delay(20000);  //5000 per ms
     }
  }
  else  //turn bright
  {
    while(old_val > new_val)
    {
      LED_SetLumin(n,--old_val);
      LED_Delay(20000);
    }
  }
  return 0;
}

char led_status[5];
BOOL LED_Update(char *status)
{
        
  if(0x01 & status[0])
    LED_TurnOn(LED1);
  else
    LED_TurnOff(LED1);
  
  if(0x02 & status[0])
    LED_TurnOn(LED2);
  else
    LED_TurnOff(LED2);
  
  if(0x04 & status[0])
    LED_TurnOn(LED3);
  else
    LED_TurnOff(LED3);
  
  if(0x08 & status[0])
    LED_TurnOn(LED4);
  else
    LED_TurnOff(LED4);
      
  for(unsigned char i=1;i<5;i++)
  {
    if(status[i] != node_info.status[i])
    {
//      char *p = AllocMemory(5);  //动态分配邮箱缓冲区
      CopyMemory(led_status,&status[0],5);
      OSMboxPost(led_mbox,(void *)led_status);
      break;                     //如果有变化，发送消息后不继续查找变化
    }
  }
  return 0;
}

BOOL LED_Flash (UINT16 usLed)
{
//	if (GPIO_ReadOutputDataBit(GPIOC, usLed) == Bit_RESET)
//		GPIO_SetBits(GPIOC, usLed);
//	else
//		GPIO_ResetBits(GPIOC, usLed);
//
	return TRUE;
}

