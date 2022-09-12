 /************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/
//Build by HT32init V1.09.20.506Beta
//-----------------------------------------------------------------------------
#include "GPTM0.h"

//-----------------------------------------------------------------------------
#define GPTM0_TIMER_BASE            ((long long)SystemCoreClock / 50)

//-----------------------------------------------------------------------------
void GPTM0_Configuration(void)
{
  u32 wCRR = 0, wPSCR = 0;
  TM_TimeBaseInitTypeDef TM_TimeBaseInitStruct;
  TM_OutputInitTypeDef TM_OutputInitStruct;
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.GPTM0      = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_4, AFIO_FUN_MCTM_GPTM);  //Config AFIO mode
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_7, AFIO_FUN_MCTM_GPTM);  //Config AFIO mode
  wCRR = GPTM0_TIMER_BASE - 1;
  while ((wCRR / (wPSCR + 1)) > 0xFFFF)
  {
    wPSCR++;
  }
  wCRR = wCRR / (wPSCR + 1);

  TM_TimeBaseInitStruct.CounterReload = wCRR;
  TM_TimeBaseInitStruct.Prescaler = wPSCR;
  TM_TimeBaseInitStruct.RepetitionCounter = 0;
  TM_TimeBaseInitStruct.CounterMode = TM_CNT_MODE_UP;
  TM_TimeBaseInitStruct.PSCReloadTime = TM_PSC_RLD_IMMEDIATE;
  TM_TimeBaseInit(HT_GPTM0, &TM_TimeBaseInitStruct);

  TM_OutputInitStruct.Channel = TM_CH_0;
  TM_OutputInitStruct.OutputMode = TM_OM_PWM2;
  TM_OutputInitStruct.Control = TM_CHCTL_ENABLE;
  TM_OutputInitStruct.ControlN = TM_CHCTL_DISABLE;//TM_CHCTL_ENABLE;
  TM_OutputInitStruct.Polarity = TM_CHP_NONINVERTED;
  TM_OutputInitStruct.PolarityN = TM_CHP_NONINVERTED;
  TM_OutputInitStruct.IdleState = MCTM_OIS_LOW;
  TM_OutputInitStruct.IdleStateN = MCTM_OIS_HIGH;
  TM_OutputInitStruct.Compare = ((wCRR + 1) * (100 - 50)) / 100;
  TM_OutputInit(HT_GPTM0, &TM_OutputInitStruct);

  TM_OutputInitStruct.Channel = TM_CH_3;
  TM_OutputInitStruct.OutputMode = TM_OM_PWM2;
  TM_OutputInitStruct.Control = TM_CHCTL_ENABLE;
  TM_OutputInitStruct.ControlN = TM_CHCTL_DISABLE;//TM_CHCTL_ENABLE;
  TM_OutputInitStruct.Polarity = TM_CHP_NONINVERTED;
  TM_OutputInitStruct.PolarityN = TM_CHP_NONINVERTED;
  TM_OutputInitStruct.IdleState = MCTM_OIS_LOW;
  TM_OutputInitStruct.IdleStateN = MCTM_OIS_HIGH;
  TM_OutputInitStruct.Compare = ((wCRR + 1) * (100 - 50)) / 100;
  TM_OutputInit(HT_GPTM0, &TM_OutputInitStruct);

  TM_Cmd(HT_GPTM0, ENABLE);
}

//-----------------------------------------------------------------------------
void GPTM0_SetFrequency(u32 freq)
{
  TM_TimeBaseInitTypeDef TM_TimeBaseInitStruct;
  uint32_t wCRR = 0, wPSCR = 0;

  wCRR = (SystemCoreClock / freq) - 1;
  while ((wCRR / (wPSCR + 1)) > 0xFFFF)
  {
    wPSCR++;
  }
  wCRR = wCRR / (wPSCR + 1);

  TM_TimeBaseInitStruct.CounterReload = wCRR;
  TM_TimeBaseInitStruct.Prescaler = wPSCR;
  TM_TimeBaseInitStruct.RepetitionCounter = 0;
  TM_TimeBaseInitStruct.CounterMode = TM_CNT_MODE_UP;
  TM_TimeBaseInitStruct.PSCReloadTime = TM_PSC_RLD_IMMEDIATE;
  TM_TimeBaseInit(HT_GPTM0, &TM_TimeBaseInitStruct);

  TM_SetCaptureCompare0(HT_GPTM0, 0); //set on duty to 0
  TM_SetCaptureCompare3(HT_GPTM0, 0); //set on duty to 0
}

//-----------------------------------------------------------------------------
void GPTM0_CH0_SetOnduty(u16 percent)
{
  u16 cmp;
  if(percent > 100)
    percent = 100;
	if(percent>9){
		cmp = HT_GPTM0->CRR * (100-10.2) / 100;
	}
	else{
	cmp = HT_GPTM0->CRR * (100-percent) / 100;
}
  
  TM_SetCaptureCompare0(HT_GPTM0, cmp);
}

//-----------------------------------------------------------------------------
void GPTM0_CH0_EnablePWMOutput(void)
{
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_4, AFIO_FUN_MCTM_GPTM);  //Config AFIO mode
}

//-----------------------------------------------------------------------------
void GPTM0_CH0_DisablePWMOutput(u8 IO_State)
{
  if(IO_State)
    GPIO_SetOutBits  (HT_GPIOA, AFIO_PIN_4);
  else
    GPIO_ClearOutBits(HT_GPIOA, AFIO_PIN_4);
  GPIO_DirectionConfig(HT_GPIOA, AFIO_PIN_4, GPIO_DIR_OUT);
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_4, AFIO_FUN_GPIO);  //Config AFIO mode
}


//-----------------------------------------------------------------------------
void GPTM0_CH3_SetOnduty(u16 percent)
{
	u16 cmp;
  if(percent > 100)
    percent = 100;
	else if(percent>9){
		cmp = HT_GPTM0->CRR * (100-10.3) / 100;
	}
	else{
		cmp = HT_GPTM0->CRR * (100-percent) / 100;
	}
  TM_SetCaptureCompare3(HT_GPTM0, cmp);
//  u16 cmp;
//  if(percent > 100)
//    percent = 100;
//  cmp = HT_GPTM0->CRR * (100-percent) / 100;
//  TM_SetCaptureCompare3(HT_GPTM0, cmp);
}

//-----------------------------------------------------------------------------
void GPTM0_CH3_EnablePWMOutput(void)
{
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_7, AFIO_FUN_MCTM_GPTM);  //Config AFIO mode
}

//-----------------------------------------------------------------------------
void GPTM0_CH3_DisablePWMOutput(u8 IO_State)
{
  if(IO_State)
    GPIO_SetOutBits  (HT_GPIOA, AFIO_PIN_7);
  else
    GPIO_ClearOutBits(HT_GPIOA, AFIO_PIN_7);
  GPIO_DirectionConfig(HT_GPIOA, AFIO_PIN_7, GPIO_DIR_OUT);
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_7, AFIO_FUN_GPIO);  //Config AFIO mode
}


//-----------------------------------------------------------------------------


