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
#include "USART0.h"

//-----------------------------------------------------------------------------
__ALIGN4 _USART0_STRUCT rxd_scomm0;
__ALIGN4 _USART0_STRUCT txd_scomm0;
uint8_t axis[6]={225, 225, 225, 225,50,2};
uint8_t inex=0;
uint8_t haveBall=0;
uint8_t car_mode;
uint8_t readData;
uint8_t z=0;
int16_t CountCSB=0;
//-----------------------------------------------------------------------------
void USART0_Configuration(void)
{
  USART_InitTypeDef USART_InitStruct;
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.USART0   = 1;
  CKCUClock.Bit.AFIO     = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_8, AFIO_FUN_USART_UART);  // Config AFIO mode
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_10, AFIO_FUN_USART_UART);  // Config AFIO mode

  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_WordLength = USART_WORDLENGTH_8B;
  USART_InitStruct.USART_StopBits = USART_STOPBITS_1;
  USART_InitStruct.USART_Parity = USART_PARITY_NO;
  USART_InitStruct.USART_Mode = USART_MODE_NORMAL;
  USART_Init(HT_USART0, &USART_InitStruct);
  USART_RxCmd(HT_USART0, ENABLE);
  USART_TxCmd(HT_USART0, ENABLE);

  USART_RXTLConfig(HT_USART0, USART_RXTL_04);
  USART_TXTLConfig(HT_USART0, USART_TXTL_04);
  USART_IntConfig(HT_USART0, USART_INT_RXDR | USART_INT_TOUT, ENABLE);
  HT_USART0->TPR = 0x80 | 40;

  NVIC_EnableIRQ(USART0_IRQn);
  USART0_init_buffer();
}
void delay1()
{
	uint16_t a = 5000;
	while(a--);
}
//-----------------------------------------------------------------------------
void USART0_init_buffer (void)
{
  rxd_scomm0.read_pt = 0;
  rxd_scomm0.write_pt = 0;
  rxd_scomm0.cnt = 0;

  txd_scomm0.read_pt = 0;
  txd_scomm0.write_pt = 0;
  txd_scomm0.cnt = 0;
}

//-----------------------------------------------------------------------------
void USART0_IRQHandler(void)
{
  // Tx, move data from buffer to UART FIFO
//  if ((HT_USART0->SR) & USART_FLAG_TXC)
//  {
//    if (!txd_scomm0.cnt)
//    {
//      USART_IntConfig(HT_USART0, USART_INT_TXC, DISABLE);
//    }
//    else
//    {
//      u16 i;
//      for (i = 0; i < USART0_FIFO_LEN; i++)   //have FIFO?
//      {
//        USART_SendData(HT_USART0, txd_scomm0.buffer[txd_scomm0.read_pt]);
//        txd_scomm0.read_pt = (txd_scomm0.read_pt + 1) % USART0_BUF_SIZE;
//        txd_scomm0.cnt--;
//        if (!txd_scomm0.cnt)
//          break;
//      }
//    }
//  }

  // Rx, move data from UART FIFO to buffer
	
	
  if ((HT_USART0->SR) & USART_FLAG_RXDR)
  {
		haveBall = 1;
		while((HT_USART0->SR) & USART_FLAG_RXDR){
			readData = USART_ReceiveData(HT_USART0);
			if (readData==0xff)
				for(z=0;z<6;z++){
				axis[z] = USART_ReceiveData(HT_USART0);
					delay1();
			}
				CountCSB = axis[4];
		}


  }

//  if ((HT_USART0->SR) & USART_FLAG_TOUT)
//  {
//    USART_ClearFlag(HT_USART0, USART_FLAG_TOUT);
//    while(USART_GetFIFOStatus(HT_USART0, USART_FIFO_RX))
//    {
//      rxd_scomm0.buffer[rxd_scomm0.write_pt] = USART_ReceiveData(HT_USART0);

//      rxd_scomm0.write_pt = (rxd_scomm0.write_pt + 1) % USART0_BUF_SIZE;
//      rxd_scomm0.cnt++;
//    }
//  }
}

//-----------------------------------------------------------------------------
void USART0_analyze_data(void)
{
  u8 tmp;
  if(!rxd_scomm0.cnt)
    return;

  //...to be add
  tmp = rxd_scomm0.buffer[rxd_scomm0.read_pt];
  if( (tmp != 0xAA) )            //for example
  {
    NVIC_DisableIRQ(USART0_IRQn);
    rxd_scomm0.cnt--;  //throw invalid data
    NVIC_EnableIRQ(USART0_IRQn);
    rxd_scomm0.read_pt = (rxd_scomm0.read_pt + 1) % USART0_BUF_SIZE;
    return;
  }
  else if(rxd_scomm0.cnt >= 8)    //for example
  {
    //add your code here

  }
}

//-----------------------------------------------------------------------------
void USART0_tx_data(u8 *pt, u8 len)
{
  while(len--)
  {
    txd_scomm0.buffer[txd_scomm0.write_pt] = *pt++;
    txd_scomm0.write_pt = (txd_scomm0.write_pt + 1) % USART0_BUF_SIZE;
    NVIC_DisableIRQ(USART0_IRQn);
    txd_scomm0.cnt++;
    NVIC_EnableIRQ(USART0_IRQn);
  }

  if(txd_scomm0.cnt)
    USART_IntConfig(HT_USART0,  USART_INT_TXC , ENABLE);
}

//-----------------------------------------------------------------------------
void USART0_test(void)
{
  u8 i,test_array[8];
  for(i=0; i<8; i++)
  {
    test_array[i] = i;
  }
  USART0_tx_data(test_array, 8);
}

//-----------------------------------------------------------------------------


