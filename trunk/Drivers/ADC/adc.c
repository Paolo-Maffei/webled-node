/******************************
*******************************/

#include "Project.h"

void Init_ADC(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
//  DMA_InitTypeDef   DMA_InitStructure;
  ADC_InitTypeDef   ADC_InitStructure;
  
  /*Set and  Enable ADC1 and GPIOC clock */
  RCC_ADCCLKConfig(RCC_PCLK2_Div8); //72MHz/8
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
  /* Configure PC.0-3(chanel 10\11\12\13)  as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  
  
//  //scan mode must use DMA 
//  /* DMA1 channel1 configuration ----------------------------------------------*/
//  DMA_DeInit(DMA1_Channel1);
//  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
//  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_RegularConvertedValueTab;
//  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//  DMA_InitStructure.DMA_BufferSize = 64;
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
//  
//  /* Enable DMA1 channel1 */
//  DMA_Cmd(DMA1_Channel1, ENABLE);
//  
    /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_13Cycles5);
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_13Cycles5);
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_13Cycles5);
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_13Cycles5);
    
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);  

  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));  
}

void ADC_GetValue(char *ret)
{
  /* Start ADC1 Software Conversion */ 
  unsigned char i = 0;
  for(i=0;i<4;i++)
  {
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10+i, 1, ADC_SampleTime_13Cycles5);
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);    
    while(SET != ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    *(ret+2*i) = ADC_GetConversionValue(ADC1);
  }
}