// Copyright (c) Acconeer AB, 2018-2019
// All rights reserved

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_hal_integration.h"
#include "acc_rss.h"
#include "acc_service_power_bins.h"
#include "acc_service_sparse.h"
#include "acc_sweep_configuration.h"
#include "acc_version.h"
#include "main.h"
#include "acc_definitions.h"
/**
 * @brief Example that shows how to use power bin once
 *
 * This is an example on running power bin once on the radar with HAL.
 * The example executes as follows:
 *   - Activate Radar System Services (RSS) with HAL.
 *   - Creates a power bins configuration
 *   - Creates a power bins service using the previously created configuration
 *   - Gets the meta data from the power_bins_handle.
 *   - Runs power bins once and prints the radar data.
 *   - Deactivates and destroys the service
 *   - Deactivates Radar System Services
 */


int acc_example_power_bin_once()
{
	//printf("Acconeer software version %s\n", ACC_VERSION);
	//printf("Acconeer RSS version %s\n", acc_rss_version());

	//Initialize Radar Service System
	acc_hal_t hal = acc_hal_integration_get_implementation();

	if (!acc_rss_activate_with_hal(&hal))
	{
		//printf("acc_rss_activate_with_hal() failed\n");
		return 0;
	}

	acc_service_configuration_t power_bins_configuration = acc_service_power_bins_configuration_create();

	if (power_bins_configuration == NULL)
	{
		//printf("acc_service_power_bins_configuration_create() failed\n");
		acc_rss_deactivate();
		return 0;
	}


	acc_service_power_bins_requested_bin_count_set(power_bins_configuration, 16);//设置数据个数

	 acc_sweep_configuration_t sweep_configuration;
	 sweep_configuration = acc_service_get_sweep_configuration(power_bins_configuration);
	 if (sweep_configuration == NULL) {
	 /* Handle error */
	 }
	 acc_sweep_configuration_requested_range_set(sweep_configuration, 0.12, 0.48);//设置开始距离和测量范围,测量范围<0.9m
	 acc_sweep_configuration_tx_disable_set(sweep_configuration, true);
	 // Decrease receiver gain
//	 float current_gain;
//	 current_gain = acc_sweep_configuration_receiver_gain_get(sweep_configuration);
	 //acc_sweep_configuration_receiver_gain_set(sweep_configuration, 0.8 * current_gain);

	 // Turn off transmitter to only record noise
	 acc_sweep_configuration_tx_disable_set(sweep_configuration, true);//归一化

	acc_service_handle_t power_bins_handle = acc_service_create(power_bins_configuration);

	if (power_bins_handle == NULL)
	{
		//printf("acc_service_create failed\n");
		acc_service_power_bins_configuration_destroy(&power_bins_configuration);
		acc_rss_deactivate();
		return 0;
	}

	acc_service_status_t              status;
	acc_service_power_bins_metadata_t power_bins_metadata;

	acc_service_power_bins_get_metadata(power_bins_handle, &power_bins_metadata);
	int n_bins = power_bins_metadata.actual_bin_count;

	float radar_data[n_bins];

	status = acc_service_power_bins_execute_once(power_bins_handle, radar_data, n_bins, NULL);

	if (status == ACC_SERVICE_STATUS_OK)
	{
		//printf("Number of bins: %u\n", n_bins);
//
//		printf("%u ", (unsigned)n_bins);
//		for (int i = 0; i < n_bins; i++)
//		{
//			printf("%u ", (unsigned)radar_data[i]);
//		}
//		printf("\n");

		printf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n",(unsigned)n_bins,(unsigned)radar_data[0],
				(unsigned)radar_data[1],(unsigned)radar_data[2],(unsigned)radar_data[3],
				(unsigned)radar_data[4],(unsigned)radar_data[5],(unsigned)radar_data[6],(unsigned)radar_data[7],
			  (unsigned)radar_data[8],(unsigned)radar_data[9],(unsigned)radar_data[10],(unsigned)radar_data[11],
			 (unsigned)radar_data[12],(unsigned)radar_data[13],(unsigned)radar_data[14],(unsigned)radar_data[15]);
	}
	else
	{
		//printf("Execute was not successful!\n");
	}

	acc_service_power_bins_configuration_destroy(&power_bins_configuration);
	acc_service_deactivate(power_bins_handle);
	acc_service_destroy(&power_bins_handle);

	acc_rss_deactivate();

	return status == ACC_SERVICE_STATUS_OK;
}

extern float start_valu;//
extern float length_valu;
extern uint8_t A111_bin_nub;//
extern float current_gain;

void reconfigure_sweeps_powerbin(acc_service_configuration_t power_bins_configuration)
{
	acc_sweep_configuration_t sweep_configuration = acc_service_get_sweep_configuration(power_bins_configuration);

	if (sweep_configuration == NULL) {
		printf("Sweep configuration not available\n");
	}
	else {
		float start_m = start_valu;
		float length_m =length_valu;
		float sweep_frequency_hz = 100;

		acc_sweep_configuration_requested_start_set(sweep_configuration, start_m);
		acc_sweep_configuration_requested_length_set(sweep_configuration, length_m);

		//acc_sweep_configuration_repetition_mode_streaming_set(sweep_configuration, sweep_frequency_hz);

		acc_sweep_configuration_repetition_mode_max_frequency_set(sweep_configuration);

		current_gain = acc_sweep_configuration_receiver_gain_get(sweep_configuration);

		acc_sweep_configuration_receiver_gain_set(sweep_configuration, 0.7);


//		acc_sweep_configuration_power_save_mode_set(sweep_configuration,ACC_SWEEP_CONFIGURATION_POWER_SAVE_MODE_B);
//		char pw=acc_sweep_configuration_power_save_mode_get(sweep_configuration);
//		printf("省电模式为“%s”\n",pw);

		current_gain = acc_sweep_configuration_receiver_gain_get(sweep_configuration);
		current_gain=current_gain;
		//acc_sweep_configuration_tx_disable_set(sweep_configuration, false);
		//acc_sweep_configuration_repetition_mode_max_frequency_set(sweep_configuration);
	}
/*	sweep_configuration = acc_service_get_sweep_configuration(power_bins_configuration);

	acc_sweep_configuration_tx_disable_set(sweep_configuration, true);
	sweep_configuration = acc_service_get_sweep_configuration(power_bins_configuration);
	acc_sweep_configuration_tx_disable_set(sweep_configuration, false);*/
}

uint16_t sparse_data[2048];
void reconfigure_sweeps_sparse(acc_service_configuration_t sparse_configuration)
{
	acc_sweep_configuration_t sweep_configuration = acc_service_get_sweep_configuration(sparse_configuration);

	if (sweep_configuration == NULL) {
		printf("Sweep configuration not available\n");
	}
	else {
		float start_m = start_valu;
		float length_m =length_valu;
		float sweep_frequency_hz = 100;

		acc_sweep_configuration_requested_start_set(sweep_configuration, 0.2);
		acc_sweep_configuration_requested_length_set(sweep_configuration, 0.5);

		acc_sweep_configuration_repetition_mode_streaming_set(sweep_configuration, sweep_frequency_hz);

//		acc_sweep_configuration_repetition_mode_max_frequency_set(sweep_configuration);

		current_gain = acc_sweep_configuration_receiver_gain_get(sweep_configuration);

		acc_sweep_configuration_receiver_gain_set(sweep_configuration, 0.5);


		current_gain = acc_sweep_configuration_receiver_gain_get(sweep_configuration);
		current_gain=current_gain;
		//acc_sweep_configuration_tx_disable_set(sweep_configuration, false);
		//acc_sweep_configuration_repetition_mode_max_frequency_set(sweep_configuration);
	}

}

extern uint8_t stop_runing;

void acc_power_bin(void)
{
	printf("Acconeer software version %s\n", ACC_VERSION);
	printf("Acconeer RSS version %s\n", acc_rss_version());


		//Initialize Radar Service System
	acc_hal_t hal = acc_hal_integration_get_implementation();//初始化系统，采用默认hal

	acc_rss_activate_with_hal(&hal);


	acc_service_configuration_t power_bins_configuration = acc_service_power_bins_configuration_create();

	acc_service_power_bins_requested_bin_count_set(power_bins_configuration, A111_bin_nub);


	if (power_bins_configuration == NULL) {
		printf("acc_service_power_bins_configuration_create() failed\n");
		return 0;
	}
	reconfigure_sweeps_powerbin(power_bins_configuration);


	acc_service_handle_t power_bins_handle = acc_service_create(power_bins_configuration);

	if (power_bins_handle == NULL) {
		printf("acc_service_create failed\n");
	  	return 0;
	}

	acc_service_status_t status;
	acc_service_power_bins_metadata_t power_bins_metadata;

	acc_service_power_bins_get_metadata(power_bins_handle, &power_bins_metadata);

	int n_bins = power_bins_metadata.actual_bin_count;

	float  radar_data[n_bins];

	UART_send_data("\nStart running poverbin\n");
	while(stop_runing)
	{
		status = acc_service_power_bins_execute_once(power_bins_handle, radar_data, n_bins, NULL);


		if (status == ACC_SERVICE_STATUS_OK) {
			UART_send_data("\nBin Data: ");
			for (int i=0 ; i < n_bins ; i++) {
				printf_chr((uint16_t)radar_data[i]);
			}
			UART_send_data("\r\n");
			LED1_ON
			LED2_OFF
			HAL_Delay(300);
			LED1_OFF
			LED2_ON
			HAL_Delay(200);

			/*printf("Radar data: %u\n", n_bins);
				for (int i=0 ; i < n_bins ; i++) {
					printf("%u  ", (unsigned)radar_data[i]);
				       /* if (radar_data[i]*normalization_factor > threshold){
				            return true;
				        }*/
			//	}
			//	printf("\n");
		}
	}

	acc_service_destroy(&power_bins_handle);
	acc_rss_deactivate();
	return 1;
}
void acc_sparse(void)
{
	printf("Acconeer software version %s\n", ACC_VERSION);
	printf("Acconeer RSS version %s\n", acc_rss_version());

	//Initialize Radar Service System
	acc_hal_t hal = acc_hal_integration_get_implementation();//初始化系统
	acc_rss_activate_with_hal(&hal);

//	hal.log.log_level =ACC_LOG_LEVEL_ERROR;

/*	if (!acc_rss_activate_with_hal(&hal))
	{
		printf("acc_rss_activate_with_hal() failed\n");
		return 0;
	}
*/

	acc_service_configuration_t sparse_configuration = acc_service_sparse_configuration_create();//

	if (sparse_configuration == NULL) {
		printf("acc_service_sparse_configuration_create() failed\n");
		return 0;
	}
	reconfigure_sweeps_sparse(sparse_configuration);//


	acc_service_handle_t sparse_handle = acc_service_create(sparse_configuration);//创建sparse服务

	if (sparse_handle == NULL) {
		printf("acc_service_create failed\n");
	  	return 0;
	}

	acc_service_status_t status;
	acc_service_sparse_metadata_t sparse_metadata;

	acc_service_sparse_result_info_t sparse_result_info;
	acc_service_status_t service_status = acc_service_activate(sparse_handle);

	acc_service_sparse_get_metadata(sparse_handle, &sparse_metadata);

	int sparse_length = sparse_metadata.data_length;

//	float  radar_data[n_bins];

	UART_send_data("\nStart running sparse\n");

	while(stop_runing)
	{
		status = acc_service_sparse_get_next(sparse_handle, sparse_data, sparse_length, &sparse_result_info);


		if (status == ACC_SERVICE_STATUS_OK) {
			UART_send_data("\nSparse Data: ");
			for (uint_fast16_t extra = 0; extra <sparse_metadata.data_length; extra++)
			{
//				sparse_data[extra]=sparse_data[extra]-32000;
				printf_chr(sparse_data[extra]);
			}
			UART_send_data("\r\n\n");
			LED1_ON
			LED2_OFF
			HAL_Delay(300);
			LED1_OFF
			LED2_ON
			HAL_Delay(200);


		}
	}

	acc_service_sparse_configuration_destroy(&sparse_configuration);//销毁sparse配置
	acc_service_destroy(&sparse_handle);//销毁sparse服务
	acc_rss_deactivate();//停用
	return 1;
	}
