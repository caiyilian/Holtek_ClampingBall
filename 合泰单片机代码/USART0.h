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
#ifndef __USART0_H
#define __USART0_H


//-----------------------------------------------------------------------------
#include "ht32.h"

//-----------------------------------------------------------------------------
#define USART0_BUF_SIZE 256
#define USART0_FIFO_LEN 1
extern uint8_t axis[6];
extern uint8_t car_mode;
extern uint8_t haveBall;
extern int16_t CountCSB;
//-----------------------------------------------------------------------------
typedef struct
{
  u8 buffer[USART0_BUF_SIZE];
  u16 write_pt;
  u16 read_pt;
  u16 cnt;
}_USART0_STRUCT;

//-----------------------------------------------------------------------------
void USART0_Configuration(void);
void USART0_init_buffer (void);
void USART0_analyze_data(void);
void USART0_tx_data(u8 *pt, u8 len);
void USART0_test(void);

//-----------------------------------------------------------------------------
#endif

