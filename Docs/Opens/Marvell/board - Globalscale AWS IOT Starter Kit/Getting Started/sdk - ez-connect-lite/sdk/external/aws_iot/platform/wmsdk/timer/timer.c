/*
 *  Copyright 2008-2016 Marvell International Ltd.
 *  All Rights Reserved.
 */

/**
 * @file timer.c
 * @brief Linux implementation of the timer interface.
 */
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>

#include <timer_interface.h>

bool has_timer_expired(Timer *timer)
{
	if (left_ms(timer) > 0)
		return false;
	else
		return true;
}

void countdown_ms(Timer *timer, uint32_t timeout)
{
	timer->timeout = timeout;
	timer->start_timestamp = os_ticks_get();
}

uint32_t left_ms(Timer *timer)
{
	uint32_t current_timestamp = os_ticks_get();
	uint32_t time_diff_ms = current_timestamp - timer->start_timestamp;
	if (timer->timeout > time_diff_ms)
		return timer->timeout - time_diff_ms;
	else
		return 0;
}

void countdown_sec(Timer *timer, uint32_t timeout)
{
	/* converting timeout in milliseconds */
	timer->timeout = timeout * 1000;
	timer->start_timestamp = os_ticks_get();
}

void init_timer(Timer *timer)
{
	timer->timeout = 0;
	timer->start_timestamp = 0;
}
