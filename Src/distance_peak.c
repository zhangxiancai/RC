/*
 * distance_peak.c
 *
 *  Created on: 2019年2月21日
 *      Author: Administrator
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "usart.h"
#include "acc_service_power_bins.h"
#include "acc_sweep_configuration.h"
#include "acc_hal_integration.h"
#include "acc_version.h"
#include "acc_rss.h"

#include "acc_service_envelope.h"
#include "acc_detector_distance_peak.h"



//////////////////////////////////////////////////////
void printf_chr(uint16_t data)
{
	uint8_t temp[6];
	if(data>9999){temp[0]=data/10000+0x30;temp[1]=data/1000%10+0x30;temp[2]=data/100%10+0x30;temp[3]=data/10%10+0x30;temp[4]=data%10+0x30;}
	else if(data>999){temp[0]=' ';temp[1]=data/1000+0x30;temp[2]=data/100%10+0x30;temp[3]=data/10%10+0x30;temp[4]=data%10+0x30;}
	else if(data>99){temp[0]=' ';temp[1]=' ';temp[2]=data/100+0x30;temp[3]=data/10%10+0x30;temp[4]=data%10+0x30;}
	else if(data>9){temp[0]=' ';temp[1]=' ';temp[2]=' ';temp[3]=data/10+0x30;temp[4]=data%10+0x30;}
	else {temp[0]=' ';temp[1]=' ';temp[2]=' ';temp[3]=' ';temp[4]=data%10+0x30;}
	temp[5]=' ';

	HAL_UART_Transmit(&huart1,temp,6,10000);

}
/////////////////////////////////////////////////////

int peak_data=0;
int length_data=0;
int peak_index(uint16_t data[], uint16_t data_length)
{
	uint16_t max = 0;
	int peak_idx = 0;
	for (int i = 30 ; i<data_length ; i++)
	{
		if (data[i] > max )
		{
			max = data[i];
			peak_idx = i;
			peak_data=i;
		}
	}
//	length_data=peak_data/25;
	return peak_idx;
}

float current_gain;
void reconfigure_sweeps(acc_service_configuration_t envelope_configuration,float start,float length)
{
	acc_sweep_configuration_t sweep_configuration = acc_service_get_sweep_configuration(envelope_configuration);

	if (sweep_configuration == NULL) {
		printf("Sweep configuration not available\n");
	}
	else {
		float start_m = start;
		float length_m = length;
		float sweep_frequency_hz = 100;

//		acc_sweep_configuration_tx_disable_set(sweep_configuration, true);

		acc_sweep_configuration_requested_start_set(sweep_configuration, 0.5);
		acc_sweep_configuration_requested_length_set(sweep_configuration, 0.5);


		//acc_sweep_configuration_repetition_mode_streaming_set(sweep_configuration, sweep_frequency_hz);
		acc_sweep_configuration_repetition_mode_max_frequency_set(sweep_configuration);

		current_gain = acc_sweep_configuration_receiver_gain_get(sweep_configuration);

		acc_sweep_configuration_receiver_gain_set(sweep_configuration, 0.85);


		current_gain = acc_sweep_configuration_receiver_gain_get(sweep_configuration);
		current_gain=current_gain;
	}
}


acc_sensor_id_t ID=1;
acc_calibration_context_t context;
uint8_t temp_flag=0;

void set_context(void)
{
	context.data[0]=50;
	context.data[4]=2;
	context.data[6]=4;
	context.data[8]=47;
	context.data[10]=5;

	context.data[12]=30;
	context.data[16]=81;
	context.data[18]=12;
	context.data[20]=4;
	context.data[22]=4;

	context.data[26]=252;
	context.data[28]=252;

	context.data[30]=84;
	context.data[32]=4;
	context.data[34]=4;
	context.data[36]=252;
	context.data[38]=252;
	context.data[40]=252;
	context.data[42]=28;
	context.data[44]=104;
	context.data[46]=2;

	context.data[47]=144;
	context.data[48]=129;
	context.data[49]=77;
	context.data[50]=4;

}

uint16_t envelope_data[6024];
int big=0;
extern UART_HandleTypeDef huart2;
int length=0;
float dist_length=0;
uint8_t buf[5];
acc_service_configuration_t envelope_configuration;

extern uint8_t stop_runing;

float get_dist(void)
{
	acc_service_envelope_metadata_t envelope_metadata;
	int peak_idx = peak_index(envelope_data, envelope_metadata.data_length);
	uint16_t min_amplitude = 700;
	if (envelope_data[peak_idx] > min_amplitude)
	{
		float dist =  peak_idx *envelope_metadata.actual_length_m;// /(envelope_metadata.data_length – 1);// – envelope_metadata.free_space_absolute_offset;
		float temp=envelope_metadata.data_length-1;
		dist=dist / temp;
		dist = envelope_metadata.actual_start_m +dist;
		return dist;
	}
	return 0;
}

acc_service_status_t dist_blocking_calls(acc_service_configuration_t envelope_configuration)
{
	uint8_t time_count=0;


	acc_service_handle_t handle = acc_service_create(envelope_configuration);

	if (handle == NULL) {
		printf("acc_service_create failed\n");
		return ACC_SERVICE_STATUS_FAILURE_UNSPECIFIED;
	}

	acc_service_envelope_metadata_t envelope_metadata;
	acc_service_envelope_get_metadata(handle, &envelope_metadata);

	temp_flag= acc_rss_calibration_context_get(ID,&context);
//	acc_service_envelope_running_average_factor_set(envelope_configuration, 0.95);//平均值

	//uint16_t envelope_data[envelope_metadata.data_length];

	acc_service_envelope_result_info_t result_info;
	acc_service_status_t service_status = acc_service_activate(handle);

	if (service_status == ACC_SERVICE_STATUS_OK) {
		UART_send_data("\nStart running distance\n");
		while (stop_runing) {
			service_status = acc_service_envelope_execute_once(handle, envelope_data, envelope_metadata.data_length, &result_info);

			if (service_status == ACC_SERVICE_STATUS_OK) {

				big=peak_index(envelope_data,envelope_metadata.data_length);

				dist_length =  big *envelope_metadata.actual_length_m;// /(envelope_metadata.data_length – 1);// – envelope_metadata.free_space_absolute_offset;
				float temp=envelope_metadata.data_length-1;
				dist_length=dist_length / temp;
				dist_length = envelope_metadata.actual_start_m +dist_length;
				length = (dist_length*1000);
				if(envelope_data[big]<700)
				{
					length=0;
				}
				UART_send_data("\r\nDistance:");
				printf_chr(length);
				UART_send_data("MM\r\n");
			}
			 // HAL_UART_Transmit(&huart2,"123",3,1000);
			LED1_ON
			LED2_OFF
			HAL_Delay(100);
			LED1_OFF
			LED2_ON
			HAL_Delay(300);
		}
		service_status = acc_service_deactivate(handle);
	}

	acc_rss_deactivate();
	acc_service_destroy(&handle);

	return service_status;
}

extern float start_valu;//
extern float length_valu;


void acc_dist_funtion(void)
{
	uint16_t  i;
	printf("Acconeer software version %s\n", ACC_VERSION);
	printf("Acconeer RSS version %s\n", acc_rss_version());


		//Initialize Radar Service System
	acc_hal_t hal = acc_hal_integration_get_implementation();
	acc_rss_activate_with_hal(&hal);


	envelope_configuration = acc_service_envelope_configuration_create();

	reconfigure_sweeps(envelope_configuration,start_valu,length_valu);

	if (envelope_configuration == NULL) {
		printf("acc_service_envelope_configuration_create() failed\n");
		return EXIT_FAILURE;
	}
	acc_service_status_t service_status;

	acc_service_envelope_profile_t profile = ACC_SERVICE_ENVELOPE_PROFILE_MAXIMIZE_SNR;
	acc_service_envelope_profile_set(envelope_configuration, profile);

	service_status = dist_blocking_calls(envelope_configuration);
}


acc_service_status_t execute_envelope_with_blocking_calls(acc_service_configuration_t envelope_configuration)
{
	uint8_t time_count=0;


	UART_send_data("11\r\n\n");
	acc_service_handle_t handle = acc_service_create(envelope_configuration);
	UART_send_data("22\r\n\n");
	if (handle == NULL) {
		printf("acc_service_create failed\n");
		return ACC_SERVICE_STATUS_FAILURE_UNSPECIFIED;
	}


	acc_service_envelope_metadata_t envelope_metadata;
	acc_service_envelope_get_metadata(handle, &envelope_metadata);

//	acc_service_envelope_running_average_factor_set(envelope_configuration, 0.95);//平均值

	//uint16_t envelope_data[envelope_metadata.data_length];

	acc_service_envelope_result_info_t result_info;
	acc_service_status_t service_status = acc_service_activate(handle);

	if (service_status == ACC_SERVICE_STATUS_OK) {

		UART_send_data("\nStart running envelope\n");
		while (stop_runing) {
			service_status = acc_service_envelope_execute_once(handle, envelope_data, envelope_metadata.data_length, &result_info);

			if (service_status == ACC_SERVICE_STATUS_OK) {
				UART_send_data("\n\nEnvelope Data: \r\n");
				for (uint_fast16_t index = 0; index < envelope_metadata.data_length; index++)
				{
					printf_chr(envelope_data[index]);
				}
				UART_send_data("\r\n\n");
			}
			else
			{
				break;
			}

			 // HAL_UART_Transmit(&huart2,"123",3,1000);
			LED1_ON
			LED2_OFF
			HAL_Delay(500);
			LED1_OFF
			LED2_ON
			HAL_Delay(500);
		}
	}

	acc_rss_deactivate();
	acc_service_destroy(&handle);

	return service_status;
}



void acc_example_funtion(void)
{
	uint16_t  i;
	printf("Acconeer software version %s\n", ACC_VERSION);
	printf("Acconeer RSS version %s\n", acc_rss_version());


	//Initialize Radar Service System
	acc_hal_t hal = acc_hal_integration_get_implementation();
	while(temp_flag==0)
	{
		temp_flag=acc_rss_activate_with_hal(&hal);
		HAL_Delay(500);
	}
/*	temp_flag=0;
	while(temp_flag==0)
	{
		temp_flag=acc_rss_calibration_context_get(ID,&context);
		HAL_Delay(500);
	}

*/

	envelope_configuration = acc_service_envelope_configuration_create();
	reconfigure_sweeps(envelope_configuration,start_valu,length_valu);

	if (envelope_configuration == NULL) {
		printf("acc_service_envelope_configuration_create() failed\n");
		return EXIT_FAILURE;
	}

	acc_service_status_t service_status;

	service_status = execute_envelope_with_blocking_calls(envelope_configuration);
}

static void print_distances(uint16_t                                      reflection_count,
                            const acc_detector_distance_peak_reflection_t *reflections,
                            float                                         sensor_offset)
{
	if (reflection_count == 0)
	{
		printf("No peaks above threshold\n");
	}
	else
	{
		for (int i = 0; i < reflection_count; i++)
		{
			unsigned int dist = (unsigned int)((reflections[i].distance - sensor_offset) * 1000.0f + 0.5f);
			unsigned int ampl = (unsigned int)(reflections[i].amplitude + 0.5f);

			printf("Peak %d, distance: %3u mm, amplitude: %u\n", i + 1, dist, ampl);
		}
	}

	printf("\n");
}
void acc_dist_peak(void)
{
	////SPI初始化//////////
	acc_hal_t hal = acc_hal_integration_get_implementation();
	acc_rss_activate_with_hal(&hal);
	//创建设备配置
	acc_detector_distance_peak_configuration_t distance_configuration;
	distance_configuration=acc_detector_distance_peak_configuration_create();
/*	if(distance_configuration==NULL)
	{
		acc_rss_deactivate();
		return 0;
	}

	////添加设备配置///////////////////
	acc_detector_distance_peak_status_t detector_status;
	acc_detector_distance_peak_handle_t handle =acc_detector_distance_peak_create(distance_configuration);

	//读限检测参数
	acc_detector_distance_peak_metadata_t   metadata;
	acc_detector_distance_peak_get_metadata(handle, &metadata);
		float start_m  = metadata.actual_start_m;
		float length_m = metadata.actual_length_m;
		float end_m    = metadata.actual_start_m + metadata.actual_length_m;
	//激活设备
	detector_status = acc_detector_distance_peak_activate(handle);
	if (detector_status != ACC_DETECTOR_DISTANCE_PEAK_STATUS_SUCCESS)
	{
		printf("acc_service_activate() failed\n");
		acc_detector_distance_peak_destroy(&handle);
		return 0;
	}

	uint_fast16_t                           reflection_count_max = 2;
	uint_fast16_t                           reflection_count;
	acc_detector_distance_peak_reflection_t reflections[reflection_count_max];
	acc_detector_distance_peak_result_info_t result_info;
	for (int measurement = 0; measurement < 100; measurement++)
		{
			reflection_count = reflection_count_max;

			detector_status = acc_detector_distance_peak_get_next(handle,
			                                                      reflections,
			                                                      &reflection_count,
			                                                      &result_info);

			if (detector_status == ACC_DETECTOR_DISTANCE_PEAK_STATUS_SUCCESS)
			{
				printf("Seq. nr: %u, Range: %u-%u mm, Threshold %u\n",
				       (unsigned int)result_info.sequence_number,
				       (unsigned int)(start_m * 1000.0f + 0.5f),
				       (unsigned int)(end_m * 1000.0f + 0.5f),
				       500);

				print_distances(reflection_count, reflections, metadata.free_space_absolute_offset);
			}
			else
			{
				printf("Reflection data not properly retrieved\n");
			}
		}

/*	acc_detector_distance_peak_result_info_t result_info;
	detector_status = acc_distance_set_detector_threshold_mode_fixed(distance_configuration,500);
	reconfigure_sweeps(envelope_configuration,0.08,1.2);
	uint_fast16_t reflection_count = 10;
	acc_detector_distance_peak_reflection_t reflections[reflection_count];
*/


//	detector_status = acc_detector_distance_peak_get_next(handle,reflections,&reflection_count,
}


uint16_t one_seven_buf[6][2];//五组数据保存各段峰值和distance

acc_service_status_t dist_one_sven_calls(acc_service_configuration_t envelope_configuration,uint8_t count)
{
	uint8_t time_count=0;
	HAL_UART_Transmit(&huart1,"-5-",3,1000);
	set_context();
	temp_flag=0;
	temp_flag= acc_rss_calibration_context_set(ID,&context);
	if(temp_flag==0){return 0;}

	HAL_UART_Transmit(&huart1,"-6-",3,1000);
	acc_service_handle_t handle = acc_service_create(envelope_configuration);
	HAL_UART_Transmit(&huart1,"-7-",3,1000);
	if (handle == NULL) {
		printf("acc_service_create failed\n");
		return ACC_SERVICE_STATUS_FAILURE_UNSPECIFIED;
	}
	HAL_UART_Transmit(&huart1,"-8-",3,1000);
	acc_service_envelope_metadata_t envelope_metadata;
	acc_service_envelope_get_metadata(handle, &envelope_metadata);
	HAL_UART_Transmit(&huart1,"-9-",3,1000);
//	acc_service_envelope_running_average_factor_set(envelope_configuration, 0.95);//平均值

	//uint16_t envelope_data[envelope_metadata.data_length];

	acc_service_envelope_result_info_t result_info;
	acc_service_status_t service_status = acc_service_activate(handle);

	if (service_status == ACC_SERVICE_STATUS_OK) {
//		UART_send_data("\nStart running distance\n");
//		while (stop_runing)
		{
			HAL_UART_Transmit(&huart1,"-10-",4,1000);
			service_status = acc_service_envelope_execute_once(handle, envelope_data, envelope_metadata.data_length, &result_info);

			if (service_status == ACC_SERVICE_STATUS_OK)
			{
				HAL_UART_Transmit(&huart1,"-11-",4,1000);
				big=peak_index(envelope_data,envelope_metadata.data_length);

				dist_length =  big *envelope_metadata.actual_length_m;// /(envelope_metadata.data_length – 1);// – envelope_metadata.free_space_absolute_offset;
				float temp=envelope_metadata.data_length-1;
				dist_length=dist_length / temp;
				dist_length = envelope_metadata.actual_start_m +dist_length;
				length = (dist_length*1000);

				one_seven_buf[count][0]=envelope_data[big];
				one_seven_buf[count][1]=length;
				HAL_UART_Transmit(&huart1,"-12-",4,1000);
				/*
				if(envelope_data[big]<700)
				{
					length=0;
				}
				UART_send_data("\r\nDistance:");
				printf_chr(length);
				UART_send_data("MM\r\n");
				*/
			}
			 // HAL_UART_Transmit(&huart2,"123",3,1000);
			LED1_tog;

		}

	}

//	service_status = acc_service_deactivate(handle);
//	acc_rss_deactivate();
	acc_service_destroy(&handle);

	return service_status;
}


void test_one_to_seven(void)
{

	uint8_t i,j;
	float start_vl[6]={0.200f,2.00f,3.80f,5.30f,};
	float length_vl[6]={2.00f,1.80f,1.50f,1.30f,};

	acc_hal_t hal = acc_hal_integration_get_implementation();
	acc_rss_activate_with_hal(&hal);

	while(1){
	for(i=0;i<4;i++)
	{
		HAL_UART_Transmit(&huart1,"-1-",3,1000);

		envelope_configuration = acc_service_envelope_configuration_create();
		HAL_UART_Transmit(&huart1,"-2-",3,1000);
		reconfigure_sweeps(envelope_configuration,start_vl[i],length_vl[i]);
		HAL_UART_Transmit(&huart1,"-3-",3,1000);
		acc_service_status_t service_status;

		acc_service_envelope_profile_t profile = ACC_SERVICE_ENVELOPE_PROFILE_MAXIMIZE_SNR;
		acc_service_envelope_profile_set(envelope_configuration, profile);
		HAL_UART_Transmit(&huart1,"-4-",3,1000);
		service_status = dist_one_sven_calls(envelope_configuration,i);
	}

	UART_send_data("\r\nBBB");

	for(i=0;i<6;i++)
	{
		printf_chr(one_seven_buf[i][0]);
		//printf_chr(one_seven_buf[i][1]);
	}
	UART_send_data("\r\nTTT");
	for(i=0;i<6;i++)
		{
			//printf_chr(one_seven_buf[i][0]);
			printf_chr(one_seven_buf[i][1]);
		}
	UART_send_data("\r\n     ");
	HAL_Delay(100);
	LED1_tog;
	HAL_Delay(100);
	}
}







