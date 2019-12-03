// Copyright (c) Acconeer AB, 2018-2019
// All rights reserved

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include "acc_detector_distance_peak.h"
#include "acc_hal_integration.h"
#include "acc_rss.h"
#include "acc_sweep_configuration.h"
#include "acc_version.h"

#include "example_detector_distance_peak_fixed_threshold.h"


/**
 * @brief Example that shows how to use the distance peak detector
 *
 * This is an example on how the distance peak detector can be used.
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create a distance peak detector configuration (with blocking mode as default)
 *   - Create a distance peak detector using the previously created configuration
 *   - Activate the distance peak detector
 *   - Get the result and print it 100 times
 *   - Deactivate and destroy the distance peak detector
 *   - Destroy the distance peak detector configuration
 *   - Deactivate Radar System Software
 */


#define FIXED_THRESHOLD_VALUE (500)
#define SENSOR_ID             (1)
#define RANGE_START_M         (0.2f)
#define RANGE_LENGTH_M        (0.94f)
#define UPDATE_RATE_HZ        (0.5f)


static void configure_detector(acc_detector_distance_peak_configuration_t distance_configuration)
{
	acc_sweep_configuration_t sweep_configuration;

	acc_detector_distance_peak_profile_set(distance_configuration, ACC_DETECTOR_DISTANCE_PEAK_PROFILE_MAXIMIZE_DEPTH_RESOLUTION);
	acc_detector_distance_peak_set_absolute_amplitude(distance_configuration, true);
	acc_detector_distance_peak_set_threshold_mode_fixed(distance_configuration, FIXED_THRESHOLD_VALUE);
	acc_detector_distance_peak_set_sort_by_amplitude(distance_configuration, true);

	sweep_configuration = acc_detector_distance_peak_get_sweep_configuration(distance_configuration);

	if (sweep_configuration == NULL)
	{
		printf("Sweep configuration not available\n");
	}
	else
	{
		acc_sweep_configuration_sensor_set(sweep_configuration, SENSOR_ID);
		acc_sweep_configuration_requested_start_set(sweep_configuration, RANGE_START_M);
		acc_sweep_configuration_requested_length_set(sweep_configuration, RANGE_LENGTH_M);
		acc_sweep_configuration_repetition_mode_streaming_set(sweep_configuration, UPDATE_RATE_HZ);
	}
}


static void print_distances(uint16_t                                      reflection_count,
                            const acc_detector_distance_peak_reflection_t *reflections)
{
	if (reflection_count == 0)
	{
		printf("No peaks above threshold\n");
	}
	else
	{
		for (int i = 0; i < reflection_count; i++)
		{
			unsigned int dist = (unsigned int)((reflections[i].distance) * 1000.0f + 0.5f);
			unsigned int ampl = (unsigned int)(reflections[i].amplitude + 0.5f);

			printf("Peak %d, distance: %3u mm, amplitude: %u\n", i + 1, dist, ampl);
		}
	}

	printf("\n");
}


static int  distance_peak_detect_with_blocking_calls(acc_detector_distance_peak_configuration_t distance_configuration)
{
	acc_detector_distance_peak_status_t     detector_status = ACC_DETECTOR_DISTANCE_PEAK_STATUS_SUCCESS;
	acc_detector_distance_peak_metadata_t   metadata;
	uint_fast16_t                           reflection_count_max = 2;
	uint_fast16_t                           reflection_count;
	acc_detector_distance_peak_reflection_t reflections[reflection_count_max];

	printf("Running distance peak detector in blocking mode\n");

	configure_detector(distance_configuration);

	acc_detector_distance_peak_handle_t handle = acc_detector_distance_peak_create(distance_configuration);

	if (handle == NULL)
	{
		printf("acc_service_create failed()\n");
		return 0;
	}

	acc_detector_distance_peak_get_metadata(handle, &metadata);
	float start_m  = metadata.actual_start_m;
	float length_m = metadata.actual_length_m;
	float end_m    = metadata.actual_start_m + metadata.actual_length_m;

	printf("Actual start: %u mm\n", (unsigned int)(start_m * 1000.0f + 0.5f));
	printf("Actual length: %u mm\n", (unsigned int)(length_m * 1000.0f + 0.5f));
	printf("Actual end: %u mm\n", (unsigned int)(end_m * 1000.0f + 0.5f));
	printf("\n");

	acc_detector_distance_peak_result_info_t result_info;

	detector_status = acc_detector_distance_peak_activate(handle);

	if (detector_status != ACC_DETECTOR_DISTANCE_PEAK_STATUS_SUCCESS)
	{
		printf("acc_service_activate() failed\n");
		acc_detector_distance_peak_destroy(&handle);
		return 0;
	}

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
			       FIXED_THRESHOLD_VALUE);

			print_distances(reflection_count, reflections);
		}
		else
		{
			printf("Reflection data not properly retrieved\n");
		}
	}

	acc_detector_distance_peak_deactivate(handle);
	acc_detector_distance_peak_destroy(&handle);

	return 1;
}


int acc_example_detector_distance_peak_fixed_threshold(void)
{
	printf("Acconeer software version %s\n", ACC_VERSION);
	printf("Acconeer RSS version %s\n", acc_rss_version());

	acc_hal_t hal = acc_hal_integration_get_implementation();

	if (!acc_rss_activate_with_hal(&hal))
	{
		printf("acc_rss_activate_with_hal() failed\n");
		return 0;
	}

	//Create the detector configuration
	acc_detector_distance_peak_configuration_t distance_configuration = acc_detector_distance_peak_configuration_create();

	if (distance_configuration == NULL)
	{
		printf("acc_service_distance_configuration_create() failed\n");
		acc_rss_deactivate();
		return 0;
	}

	//Run distance peak detection in blocking mode
	int status = distance_peak_detect_with_blocking_calls(distance_configuration);

	acc_detector_distance_peak_configuration_destroy(&distance_configuration);

	acc_rss_deactivate();

	return status;
}
