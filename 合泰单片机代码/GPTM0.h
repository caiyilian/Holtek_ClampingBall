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
#ifndef __GPTM0_H
#define __GPTM0_H


//-----------------------------------------------------------------------------
#include "ht32.h"

//-----------------------------------------------------------------------------
void GPTM0_Configuration(void);
void GPTM0_SetFrequency(u32 freq);
void GPTM0_CH0_SetOnduty(u16 percent);
void GPTM0_CH0_EnablePWMOutput(void);
void GPTM0_CH0_DisablePWMOutput(u8 IO_State);
void GPTM0_CH3_SetOnduty(u16 percent);
void GPTM0_CH3_EnablePWMOutput(void);
void GPTM0_CH3_DisablePWMOutput(u8 IO_State);

//-----------------------------------------------------------------------------
#endif

