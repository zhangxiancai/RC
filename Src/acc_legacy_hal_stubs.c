// Copyright (c) Acconeer AB, 2018-2019
// All rights reserved

#include <stdbool.h>
#include <stdint.h>

#include "acc_definitions.h"
#include "acc_types.h"

typedef void (*acc_board_isr_t)(acc_sensor_id_t); // currently defined in acc_board.h move to acc_definitions.h?

bool acc_board_init(void);
void acc_board_start_sensor(acc_sensor_id_t sensor);
void acc_board_stop_sensor(acc_sensor_id_t sensor);
void acc_board_stop_sensor(acc_sensor_id_t sensor);
uint32_t acc_board_get_sensor_count(void);
float acc_board_get_ref_freq(void);
bool acc_board_wait_for_sensor_interrupt(acc_sensor_id_t sensor_id, uint32_t timeout_ms);
void acc_board_sensor_transfer(acc_sensor_id_t sensor_id, uint8_t *buffer, size_t buffer_length);
bool acc_board_gpio_init(void);
size_t acc_device_spi_get_max_transfer_size(void);
acc_os_thread_id_t acc_os_get_thread_id(void);
acc_os_semaphore_t acc_os_semaphore_create(void);
void *acc_os_mem_alloc_debug(size_t size, const char *file, uint16_t line);
void acc_os_sleep_us(uint32_t time_usec);
void acc_os_mem_free(void *ptr);
void acc_os_get_time(uint32_t *time_usec);
acc_os_mutex_t acc_os_mutex_create(void);
void acc_os_mutex_lock(acc_os_mutex_t mutex);
void acc_os_mutex_unlock(acc_os_mutex_t mutex);
void acc_os_mutex_destroy(acc_os_mutex_t mutex);
acc_os_thread_handle_t acc_os_thread_create(void (*func)(void *param), void *param, const char *name);
void acc_os_thread_exit(void);
void acc_os_thread_cleanup(acc_os_thread_handle_t handle);
bool acc_os_semaphore_wait(acc_os_semaphore_t sem, uint16_t timeout_ms);
void acc_os_semaphore_signal(acc_os_semaphore_t sem);
void acc_os_semaphore_signal_from_interrupt(acc_os_semaphore_t sem);
void acc_os_semaphore_destroy(acc_os_semaphore_t sem);
uint16_t acc_os_ntohs(uint16_t value);
bool acc_os_multithread_support(void);
void acc_os_init(void);
acc_os_thread_id_t acc_os_get_thread_i;
uint16_t acc_os_htons(uint16_t value);
uint32_t acc_os_ntohl(uint32_t value);
uint32_t acc_os_htonl(uint32_t value);


/**
 *  There are places in RSS where we assume that acc_os_mutex_create will return something != NULL
 *  To fully support implementations where mutexes are not needed, we return an address that is not
 *  a real address
 */
#define MAGIC_MUTEX_ADDRESS ((acc_os_mutex_t)(0x1))

/**
 * See comment about mutexes above
 */
#define MAGIC_SEMAPHORE_ADDRESS ((acc_os_semaphore_t)(0x2))


bool acc_board_init(void)
{
	return false;
}


void acc_board_start_sensor(acc_sensor_id_t sensor)
{
	ACC_UNUSED(sensor);
}


void acc_board_stop_sensor(acc_sensor_id_t sensor)
{
	ACC_UNUSED(sensor);
}


uint32_t acc_board_get_sensor_count(void)
{
	return 0;
}


float acc_board_get_ref_freq(void)
{
	return 0;
}


bool acc_board_wait_for_sensor_interrupt(acc_sensor_id_t sensor_id, uint32_t timeout_ms)
{
	ACC_UNUSED(sensor_id);
	ACC_UNUSED(timeout_ms);
	return false;
}


void acc_board_sensor_transfer(acc_sensor_id_t sensor_id, uint8_t *buffer, size_t buffer_length)
{
	ACC_UNUSED(sensor_id);
	ACC_UNUSED(buffer);
	ACC_UNUSED(buffer_length);
}


// gpio and spi following
bool acc_board_gpio_init(void)
{
	return false;
}


size_t acc_device_spi_get_max_transfer_size(void)
{
	return 4095;
}


// os following
acc_os_semaphore_t acc_os_semaphore_create(void)
{
	return MAGIC_SEMAPHORE_ADDRESS;
}


void *acc_os_mem_alloc_debug(size_t size, const char *file, uint16_t line)
{
	ACC_UNUSED(size);
	ACC_UNUSED(file);
	ACC_UNUSED(line);	
	return NULL;
}


void acc_os_sleep_us(uint32_t time_usec)
{
	ACC_UNUSED(time_usec);
}


void acc_os_mem_free(void *ptr)
{
	ACC_UNUSED(ptr);
}


void acc_os_get_time(uint32_t *time_usec){
	ACC_UNUSED(time_usec);
}


acc_os_mutex_t acc_os_mutex_create(void)
{
	return MAGIC_MUTEX_ADDRESS;
}


void acc_os_mutex_lock(acc_os_mutex_t mutex)
{
	ACC_UNUSED(mutex);
}


void acc_os_mutex_unlock(acc_os_mutex_t mutex)
{
	ACC_UNUSED(mutex);
}


void acc_os_mutex_destroy(acc_os_mutex_t mutex)
{
	ACC_UNUSED(mutex);
}


acc_os_thread_handle_t acc_os_thread_create(void (*func)(void *param), void *param, const char *name)
{
	ACC_UNUSED(param);
	ACC_UNUSED(name);
	ACC_UNUSED(func);
	return NULL;
}


void acc_os_thread_exit(void)
{
}


void acc_os_thread_cleanup(acc_os_thread_handle_t handle)
{
	ACC_UNUSED(handle);
}


bool acc_os_semaphore_wait(acc_os_semaphore_t sem, uint16_t timeout_ms)
{
	ACC_UNUSED(sem);
	ACC_UNUSED(timeout_ms);

	return -1;
}


void acc_os_semaphore_signal(acc_os_semaphore_t sem)
{
	ACC_UNUSED(sem);
}


void acc_os_semaphore_signal_from_interrupt(acc_os_semaphore_t sem)
{
	ACC_UNUSED(sem);
}


void acc_os_semaphore_destroy(acc_os_semaphore_t sem)
{
	ACC_UNUSED(sem);
}


uint16_t acc_os_ntohs(uint16_t value)
{
	uint8_t *char_value = (uint8_t *)&value;

	return ((uint32_t)char_value[0] << 8) |
	       (uint32_t)char_value[1];
}


bool acc_os_multithread_support(void)
{
	return false;
}


void acc_os_init(void)
{
}


acc_os_thread_id_t acc_os_get_thread_id(void)
{
	acc_os_thread_id_t result = {0};

	return result;
}


uint16_t acc_os_htons(uint16_t value)
{
	uint16_t result      = 0;
	uint8_t  *char_value = (uint8_t *)&result;

	char_value[0] = (uint8_t)((value >> 8) & 0xff);
	char_value[1] = (uint8_t)(value & 0xff);
	return result;
}


uint32_t acc_os_ntohl(uint32_t value)
{
	uint8_t *char_value = (uint8_t *)&value;

	return ((uint32_t)char_value[0] << 24) |
	       ((uint32_t)char_value[1] << 16) |
	       ((uint32_t)char_value[2] << 8) |
	       (uint32_t)char_value[3];
}


uint32_t acc_os_htonl(uint32_t value)
{
	uint32_t result      = 0;
	uint8_t  *char_value = (unsigned char *)&result;

	char_value[0] = (uint8_t)((value >> 24) & 0xff);
	char_value[1] = (uint8_t)((value >> 16) & 0xff);
	char_value[2] = (uint8_t)((value >> 8) & 0xff);
	char_value[3] = (uint8_t)(value & 0xff);
	return result;
}
