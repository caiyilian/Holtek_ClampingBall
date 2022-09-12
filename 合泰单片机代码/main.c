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
#include "ht32.h"
#include "BFTM0.h"
#include "USART0.h"
#include "UART0.h"
#include "UART1.h"
#include "GPTM0.h"

#define START_BIT 0x55
#define FRAME_LEN 0x0e
#define CMD_TYPE 0x01
#define CMD_ID 0x01

#define FRAME_ID 0x00

uint8_t forward[2] = {0x00, 0x10};	//ǰ������
uint8_t rotate[2] = {0,0};					//��ת
uint8_t translate[2] = {0,0};				//ƽ�ƣ��ı������������ֵ���ܿ����ٶȣ�ÿ������ĵ�һ��Ԫ�����ٶȵĸ߰�λ���ڶ���Ԫ���ǵͰ�λ
uint32_t index1;
uint8_t index2=0;
uint16_t abc=0;
uint16_t end_data; //end data

int16_t f_speed=0,r_speed=0,t_speed=0;
uint8_t tx_buff[FRAME_LEN];
uint8_t pawState = 4; //�����״̬
uint8_t carState = 5;
uint8_t clip=1;
uint8_t up = 1;
uint8_t isClip = 0;//�Ƿ�е�����
uint8_t findBall = 1;;//�Ƿ�����Ѱ��С���ģʽ
float Kp=0.55; //PID��P
float Kd=0.25;	// PID��I
int8_t preCenterDistance[2];
int8_t centerDistance;
uint8_t isStraight = 0;
void delay();
void Stop();
void Set_t(int16_t speed);
void Set_r(int16_t speed);
void Set_f(int16_t speed);
void Send_Data();
void carMoveByHand();
void carMoveAuto();
void changeCarMode();
uint8_t SerialMsgChecksum(uint8_t *data, uint8_t len);
void ClawsControl();
void NeckServoControl();
float getDistance();
//-----------------------------------------------------------------------------
int main(void)
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	
	//��ƹ���
	CKCUClock.Bit.PC = 1;
	 CKCUClock.Bit.AFIO       = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
	AFIO_GPxConfig(GPIO_PC, AFIO_PIN_15, AFIO_FUN_GPIO);
	 GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_15, GPIO_DIR_OUT);
	GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_15, 1);
	
	
	
  CKCUClock.Bit.PA = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
	AFIO_GPxConfig(GPIO_PA, AFIO_PIN_3, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GPIO_PA, AFIO_PIN_2, AFIO_FUN_GPIO);
	GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_3, GPIO_DIR_OUT);//TRIG
	GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_2, GPIO_DIR_IN);
	GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_2, GPIO_PR_UP);
	GPIO_InputConfig(HT_GPIOA, GPIO_PIN_2, ENABLE);
	
  BFTM0_Configuration();
  UART0_Configuration();//���
  UART1_Configuration();//esp8266
  USART0_Configuration();//k210
  GPTM0_Configuration();//pwm
	GPTM0_CH0_EnablePWMOutput();//���Ƽ��ӵĶ��
	GPTM0_CH3_EnablePWMOutput();//���Ƹ߶ȵĶ��
	GPTM0_SetFrequency(50);
	GPTM0_CH0_SetOnduty(5);
		

	tx_buff[0]  = START_BIT;
	tx_buff[1]  = FRAME_LEN;
	tx_buff[2]  = CMD_TYPE;
	tx_buff[3]  = CMD_ID;
	tx_buff[12] = FRAME_ID;
	Stop();
	clip = 0;
	up = 0;
	
	NeckServoControl();
	ClawsControl();
	
  while (1)
  {
		
		changeCarMode();

		if(car_mode==1){
			carMoveByHand();
		}
	
		else if(car_mode==0){//�Զ�ģʽ
			carMoveAuto();
		}
	
		
		delay();
	}
	
}

void delay()
{
	uint16_t a = 10000;
	while(a--);
}



//���س�������ȡ�ľ��룬��λΪcm
float getDistance(){
	
		 GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_3, SET);
			
			BFTM_SetCounter(HT_BFTM0, 0);
			BFTM_EnaCmd(HT_BFTM0, ENABLE);
			while(HT_BFTM0->CNTR<500);//��ʱ����10΢��
			BFTM_EnaCmd(HT_BFTM0, DISABLE);
			GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_3, RESET);
			
			index1 = 0;
			while(GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_2)==RESET&&index1<1000000){
				index1++;
			};//�ȴ��ߵ�ƽ
			if(index1==1000000) return -1.0;
			
			 BFTM_SetCounter(HT_BFTM0, 0);
			BFTM_EnaCmd(HT_BFTM0, ENABLE);
				while(GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_2)!=RESET);//�ȴ��͵�ƽ
			BFTM_EnaCmd(HT_BFTM0, DISABLE);
			return (HT_BFTM0->CNTR)/2823.53;	
}

uint8_t SerialMsgChecksum(uint8_t *data, uint8_t len)
{
	uint8_t checksum = 0x00;
	uint8_t i;
	for(i = 0 ; i < (len-1); i++)
		checksum += data[i];
	return checksum;
}

void Send_Data()
{
	uint8_t i;
	for(i=0;i<FRAME_LEN;i++)
	{
		USART_SendData(HT_UART0,tx_buff[i]);
		while(!USART_GetFlagStatus(HT_UART0,USART_FLAG_TXC));//�ȴ��������
	}
}


void Set_f(int16_t speed)
{
	tx_buff[4] = speed>>8;
	tx_buff[5] = speed;
	tx_buff[13] = SerialMsgChecksum(tx_buff,FRAME_LEN);//����У���
	Send_Data();
}
void Set_r(int16_t speed)
{
	tx_buff[6] = speed>>8;
	tx_buff[7] = speed;
	tx_buff[13] = SerialMsgChecksum(tx_buff,FRAME_LEN);//����У���
	Send_Data();
}
void Set_t(int16_t speed)
{
	tx_buff[8] = speed>>8;
	tx_buff[9] = speed;
	tx_buff[13] = SerialMsgChecksum(tx_buff,FRAME_LEN);//����У���
	Send_Data();
}
void Stop()
{
	uint8_t i;
	for(i=4;i<10;i++)
		tx_buff[i]=0;
	tx_buff[13] = SerialMsgChecksum(tx_buff,FRAME_LEN);//����У���
	Send_Data();
	carState=5;
}
//����צ��
void ClawsControl(){
	if(clip){
		GPTM0_CH3_SetOnduty(8);
		USART_SendData(HT_UART1,0x01);
	}
	else{ 
		USART_SendData(HT_UART1,0x02);
		GPTM0_CH3_SetOnduty(5);
	}
}
//����̧������µ��Ǹ����
void NeckServoControl(){
	if(up) {GPTM0_CH0_SetOnduty(5);}
	else {GPTM0_CH0_SetOnduty(10);}
}
//�л�С����ģʽ�����Զ�ģʽ�����ֶ�ģʽ
void changeCarMode(){
	if(wifiData==0x08){ //�ж�С�����ڵ�ģʽ
				wifiData = 0;
				delay();
				car_mode = 1;
				Stop();
				delay();
		}
		
		else if(wifiData==0x09){
			wifiData = 0;
				delay();
				car_mode = 0;
				Stop();
				delay();
		}
}

//�ֶ�ģʽʱִ�еĺ���
void carMoveByHand(){
			if(wifiData==0x01){//forward
				wifiData = 0;	
				if(carState!=1){
					f_speed = 500;
					carState=1;
				}
				else if(f_speed<=1500){
					f_speed += 100;
				}
				t_speed = 0;
				r_speed = 0;
				
			}
			
			else if(wifiData==0x02){//right
				wifiData = 0;	
				if(carState!=2){
					carState=2;
					t_speed = 500;
					
				}
				else if(t_speed<=1500){
					t_speed += 100;
				}
				f_speed = 0;
				f_speed = 0;
				
			}
			
			else if(wifiData==0x03){//back
				wifiData = 0;	
				if(carState!=3){
					f_speed = -500;
					carState=3;
				}
				else if(f_speed>=-1500){
					f_speed -= 100;
				}
				t_speed = 0;
				r_speed = 0;
			}
			
			else if(wifiData==0x04){ //left
				wifiData = 0;	
				if(carState!=4){
					carState=4;
					t_speed = -500;
				}
				else if(t_speed>=-1500){
					t_speed -= 100;
				}
				r_speed = 0;
				f_speed = 0;
				
			}
			else if(wifiData==0x05){//stop
				wifiData = 0;	
				carState=5;
				t_speed = 0;
				f_speed = 0;
				r_speed = 0;
			}
			else if(wifiData==0x06){//left_rotation
				wifiData = 0;	
				if(carState!=6){
					carState=6;
					r_speed = 40;
				}
				else if(r_speed<=150){
					r_speed += 10;
				}
				t_speed = 0;
				f_speed = 0;
			}
			else if(wifiData==0x07){//right_rotation
				wifiData = 0;	
				if(carState!=7){
					carState=7;
					r_speed = -40;
				}
				else if(r_speed>=-150){
					r_speed -= 10;
				}
				f_speed = 0;
				t_speed = 0;
			}
			else if(wifiData==0x11){
				wifiData = 0;
			up = !up;
			NeckServoControl();
		}
			else if(wifiData==0x12){
				wifiData = 0;	
			clip=!clip;
			ClawsControl();
			
		}	
			Set_r(r_speed);
			delay();
			Set_f(f_speed);
			delay();
			Set_t(t_speed);
			delay();
			//wifiData = 0;
}


//�Զ�ģʽʱִ�еĺ���
void carMoveAuto(){
		USART_SendData(HT_USART0,isClip);
		
		if(findBall==isClip){
			for(index1=0;index1<500;index1++) delay();
		}
	//��Ļ��û���κ�Ŀ��	
		if(axis[0]==225&&!isStraight){
			f_speed = 0;
			r_speed *= 0.95;
			preCenterDistance[0]=0;
			preCenterDistance[1]=0;
			Set_r(r_speed);
			delay();
			Set_f(f_speed);
			delay();
			if(isClip==findBall){
				while(axis[0]==225){
						if(car_mode==1){
							goto CarModeChange;
						}
						Set_f(0);
						delay();
						Set_r(30);
						delay();
					}
			}
			
		}
		else{
			/******************Ѱ��С�����ʼ******************/
		if(isClip==0&&!isStraight){
			
			findBall = 1;
			centerDistance = 111-axis[0]-axis[2]/2; //����������x���������Ļ���ľ���
			if((centerDistance>10)||(centerDistance<-10)){ //���С������û�кܿ�����Ļ����
				f_speed = 0;
				r_speed = Kp * centerDistance+Kd * (centerDistance - 2*preCenterDistance[0] + preCenterDistance[1]);
				preCenterDistance[0] = centerDistance; //ǰһ��
				preCenterDistance[1] = preCenterDistance[0]; //ǰ�ڶ���
				

				if((centerDistance<20)||(centerDistance>-20)) f_speed = 800;
				else if((centerDistance<40)||(centerDistance>-40)) f_speed = 600;
				else if((centerDistance<60)||(centerDistance>-60)) f_speed = 500;
				else if((centerDistance<80)||(centerDistance>-80)) f_speed = 300;
				else f_speed = 0;
			}
			else{
				f_speed = 1000;
				r_speed = 0;
			}
			//���С�����צ�ӷ�Χ�ڣ��Ϳ�ʼ��ץסС����̧��
			if(axis[1]>=160&&axis[2]>100) {
			f_speed = 0;
			r_speed = 0;
			isClip = 1;
			clip = 1;
			up = 1;
			ClawsControl();
			for(index1=0;index1<500;index1++) delay();
			NeckServoControl();
			
			while(axis[5]!=isClip){
				USART_SendData(HT_USART0,isClip);
				while(!USART_GetFlagStatus(HT_USART0,USART_FLAG_TXC));
				for(index1=0;index1<10;index1++) delay();
			};//�ȴ��������
		}
			 //��С��������Ӧ���˶�
			Set_r(r_speed);
			delay();
			Set_f(f_speed);
			delay();
			
		}
		/******************Ѱ��С��������******************/
		
		/******************Ѱ��װ�������ʼ******************/	
		else{
			findBall=0;
			centerDistance = 111-axis[0]-axis[2]/2; //����װ��������x���������Ļ���ľ���
			
			if(!isStraight&&((centerDistance>10)||(centerDistance<-10))){ //���װ��������û�кܿ�����Ļ����
				f_speed = 0;
				r_speed = Kp * centerDistance+Kd * (centerDistance - 2*preCenterDistance[0] + preCenterDistance[1]);
				preCenterDistance[0] = centerDistance; //ǰһ��
				preCenterDistance[1] = preCenterDistance[0]; //ǰ�ڶ���
				if((centerDistance<20)||(centerDistance>-20)) f_speed = 500;
				else if((centerDistance<40)||(centerDistance>-40)) f_speed = 400;
				else if((centerDistance<60)||(centerDistance>-60)) f_speed = 250;
				else if((centerDistance<80)||(centerDistance>-80)) f_speed = 100;
				Set_r(r_speed);
				delay();
				Set_f(f_speed);
				delay();
			}
			else{
				f_speed = 1000;
				r_speed = 0;
				Set_r(r_speed);
				delay();
				Set_f(f_speed);
				delay();
				if((axis[0]+axis[2]/2)>=100&&(axis[0]+axis[2]/2)<=125) {
					Stop();
					delay();
					while(1){
						if(axis[2]>=220) {
							Stop();
							delay();
							break;
						}
						
						if(axis[0]+axis[2]/2<105) {
							Set_r(111-axis[0]+axis[2]/2);
							delay();
						}
						
						else if(axis[0]+axis[2]/2>119) {
							Set_r(axis[0]+axis[2]/2-111);
							delay();
						}
						else{
							Stop();
							delay();
							break;
						}
						
					}
					isStraight = 1;
				}
				
				if(CountCSB<8){
					index2++;
					if(index2==11){
						isClip = 0;
						clip = 0;
						up = 0;
						ClawsControl();
						for(index1=0;index1<200;index1++) delay();
						Set_f(-1000);
						for(index1=0;index1<1000;index1++) delay();
						Stop();
						f_speed = 0;
						r_speed = 0;
						NeckServoControl();
						while(axis[5]==!isClip){
						USART_SendData(HT_USART0,isClip);
						while(!USART_GetFlagStatus(HT_USART0,USART_FLAG_TXC));
						for(index1=0;index1<10;index1++) delay();
						};//�ȴ��������
						isStraight = 0;
					}
				}
				else index2=0;
		}
		}
		/******************Ѱ��װ����������******************/
	}
		
		CarModeChange: for(index1=0;index1<10;index1++) delay();
		
}

