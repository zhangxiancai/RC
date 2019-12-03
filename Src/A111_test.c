/*
 * A111_test.c
 *
 *  Created on: 2019年2月27日
 *      Author: Administrator
 */
#include "example_power_bin_once.h"
#include "distance_peak_fun.h"
#include "stm32l4xx_hal.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

uint8_t A111_mode_flag=0;//测试模块选择  1为power bin 2为distance 3 为envelope
extern UART_HandleTypeDef huart1;
uint8_t size=0;
uint16_t A111_start=60;
uint16_t A111_length=100;
uint16_t A111_bin_nub=10;
void UART_send_data(uint8_t *pData)
{
	uint8_t size=0;
	while(pData[size]!=0)
	{
		size++;
	}
	//size=sizeof(pData);
	HAL_UART_Transmit(&huart1,pData,size,1000);
}

extern uint8_t USART_RX_BUF[200];
uint16_t return_data_ch(uint8_t start,uint8_t nub)
{
	uint16_t temp;
	switch (nub)
	{
		case 1:temp=USART_RX_BUF[start]-0x30;
		break;
		case 2:temp=(USART_RX_BUF[start]-0x30)*10+USART_RX_BUF[start+1]-0x30;
		break;
		case 3:temp=(USART_RX_BUF[start]-0x30)*100+(USART_RX_BUF[start+1]-0x30)*10+USART_RX_BUF[start+2]-0x30;
		break;
		case 4:temp=(USART_RX_BUF[start]-0x30)*1000+(USART_RX_BUF[start+1]-0x30)*100+(USART_RX_BUF[start+2]-0x30)*10+USART_RX_BUF[start+3]-0x30;
		break;
		default:break;
	}
	return temp;
}
uint8_t  send_valu_OK_flag=0;
uint8_t stop_runing=1;
void get_cmd(void)
{
	uint8_t i,j,k;
	uint8_t start,nub;

	if ((strncmp(&USART_RX_BUF[0], "A111 run mode",13) == 0)&&(A111_mode_flag==0))
	{
		switch(USART_RX_BUF[14])
		{
			case '1':
				A111_mode_flag=1;
				UART_send_data("\nRun Mode 1\n");
				break;
			case '2':
				A111_mode_flag=2;
				UART_send_data("\nRun Mode 2\n");
				break;
			case '3':
				A111_mode_flag=3;
				UART_send_data("\nRun Mode 3\n");
				break;
			default:
				A111_mode_flag=0;
				UART_send_data("\nPlease select the correct mode\n");
				break;
		}
	}
	if(A111_mode_flag!=0)
	{
		if (strncmp(&USART_RX_BUF[0], "Stop runing",11) == 0)
		{
			stop_runing=0;
			A111_mode_flag=0;
			send_valu_OK_flag=0;
			UART_send_data("\r\nSTOP\r\n");
			NVIC_SystemReset();
			return;
		}
		else
		{
			for(i=0;i<50;i++)
			{
				if(USART_RX_BUF[i]=='s')
				{
					if (strncmp(&USART_RX_BUF[i], "start=",6) == 0)
					{
						for(j=(i+6);j<(i+25);j++)
						{
							if(USART_RX_BUF[j]==',')
							{
								start=i+6;
								nub=j-2-start;
								A111_start=return_data_ch(start,nub);
								send_valu_OK_flag+=1;
								break;
							}
						}
					}
				}
				if(USART_RX_BUF[i]=='l')
				{
					if (strncmp(&USART_RX_BUF[i], "length=",7) == 0)
					{
						for(j=(i+7);j<(i+25);j++)
						{
							if(USART_RX_BUF[j]==',')
							{
								start=i+7;
								nub=j-2-start;
								A111_length=return_data_ch(start,nub);
								send_valu_OK_flag+=1;
								break;
							}
						}
					}
				}
				if((USART_RX_BUF[i]=='b')&&(A111_mode_flag==1))
							{
								if (strncmp(&USART_RX_BUF[i], "bin=",4) == 0)
								{
									for(j=(i+4);j<(i+25);j++)
									{
										if(USART_RX_BUF[j]==',')
										{
											start=i+4;
											nub=j-start;
											A111_bin_nub=return_data_ch(start,nub);
											send_valu_OK_flag+=1;
											break;
										}
									}
								}
							}

			}
		}
	}

}

float start_valu=0.006f,length_valu=0.009f;
void run_distance(void)
{
	start_valu=(float)((float)(A111_start)/(1000.00));
	length_valu=(float)((float)(A111_length)/(1000.00));
	UART_send_data("\nSet OK \n");
	while(1)
	{
		stop_runing=1;
		acc_dist_funtion();
		break;
	}
}
void run_power_bin(void)
{
	start_valu=(float)((float)(A111_start)/(1000.00));
	length_valu=(float)((float)(A111_length)/(1000.00));
	UART_send_data("\nSet OK \n");
	while(1)
	{
		stop_runing=1;
		acc_power_bin();
		break;
	}
}

void run_example(void)
{
	start_valu=(float)((float)(A111_start)/(1000.00));
	length_valu=(float)((float)(A111_length)/(1000.00));
	UART_send_data("\nSet OK \n");
	while(1)
	{
		stop_runing=1;
		acc_example_funtion();
		break;
	}
}

uint8_t ti_count=0;

void select_mode(void)
{
	UART_send_data("\n\n\nInput A111 run mode,1 is power bin,2 is distance,3 is envelope. \n");
	UART_send_data("Example:A111 run mode 1\n");
	/*
	HAL_UART_Transmit(&huart1,"Input A111 run mode,1 is power bin,2 is distance,3 is envelope. \n",64,10000);
	HAL_Delay(200);
	HAL_UART_Transmit(&huart1,"Example:A111 run mode 1\n",24,10000);
	*/
	while(A111_mode_flag==0)
	{
		ti_count++;
		LED1_ON
		HAL_Delay(200);
		LED1_OFF
		HAL_Delay(200);
		if(ti_count>25)
		{
			ti_count=0;
			UART_send_data("\n\n\nInput A111 run mode,1 is power bin,2 is distance,3 is envelope. \n");
			UART_send_data("Example:A111 run mode 1\n");
		}
	}
	switch(A111_mode_flag)
	{
		case 1:
			UART_send_data("\nInput power bin start valu 、 length valu and bin nub\n");
			UART_send_data("\nExample:start=100MM,length=330MM,bin=5, \n");
			break;
		case 2:
			UART_send_data("\nInput distance start valu and length valu \n");
			UART_send_data("\nExample:start=100MM,length=330MM, \n");
			break;
		case 3:
			UART_send_data("\nInput envelope start valu and length valu \n");
			UART_send_data("\nExample:start=100MM,length=330MM, \n");
			break;
		default :break;
	}
	send_valu_OK_flag=0;
	while(send_valu_OK_flag<1)
	{
		HAL_Delay(800);
	}
		switch(A111_mode_flag)
		{
			case 1:
				run_power_bin();
				break;
			case 2:
				run_distance();
				break;
			case 3:
				run_example();
				break;
			default :break;
		}
		HAL_Delay(800);
}







