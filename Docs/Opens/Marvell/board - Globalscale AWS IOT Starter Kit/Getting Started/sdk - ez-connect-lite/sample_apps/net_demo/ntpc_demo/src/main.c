/*
 *  Copyright (C) 2008-2016, Marvell International Ltd.
 *  All Rights Reserved.
 */
/*
 * Simple  NTP client for time sync using UDP
 *
 * Summary:
 * Sample Application demonstrating a simple time sync operation using NTP
 *
 * The serial console is set on UART-0.
 *
 * A serial terminal program like HyperTerminal, putty, or
 * minicom can be used to see the program output.
 */
#include <wm_os.h>
#include <wmstdio.h>
#include <wmtime.h>
#include <wmsdk.h>
#include <led_indicator.h>
#include <board.h>
#include <push_button.h>
#include <aws_utils.h>

/*-----------------------Global declarations----------------------*/
#define SYNC_INTERVAL 60000

#define MICRO_AP_SSID                "aws_starter-ntpc"
#define MICRO_AP_PASSPHRASE          "marvellwm"

#define RESET_TO_FACTORY_TIMEOUT 5000


/* Thread handle */
static os_thread_t time_sync_thread;
/* Buffer to be used as stack */
static os_thread_stack_define(time_sync_stack, 4 * 1024);

int ntpc_sync(const char *ntp_server, uint32_t max_num_pkt_xchange);

static char *month_names[] = { "Jan", "Feb", "Mar",
				     "Apr", "May", "Jun",
				     "Jul", "Aug", "Sep",
				     "Oct", "Nov", "Dec" };
static char *day_names[] = { "Sun", "Mon", "Tue", "Wed",
				   "Thu", "Fri", "Sat" };


static void _time_sync()
{
	while (1) {
		ntpc_sync("pool.ntp.org", 2);
		struct tm c_time;
		wmtime_time_get(&c_time);
		wmprintf("%s %d %s %.2d %.2d:%.2d:%.2d\r\n",
			 day_names[c_time.tm_wday],
			 c_time.tm_year + 1900, month_names[c_time.tm_mon],
			 c_time.tm_mday, c_time.tm_hour,
			 c_time.tm_min, c_time.tm_sec);
		os_thread_sleep(os_msec_to_ticks(SYNC_INTERVAL));
	}
	os_thread_self_complete(NULL);
	return;

}

static int time_sync()
{
	int ret = os_thread_create
		(&time_sync_thread, /* thread handle */
		"time-sync",/* thread name */
		_time_sync,  /* entry function */
		0,       /* argument */
		&time_sync_stack,  /* stack */
		OS_PRIO_2);  /* priority - medium low */
	return ret;
}

void wlan_event_normal_link_lost(void *data)
{
	wmprintf("Link Loss\r\n");
}

void wlan_event_normal_connect_failed(void *data)
{
	wmprintf("Connection Failed\r\n");
}

/* This function gets invoked when station interface connects to home AP.
 * Network dependent services can be started here.
 */
void wlan_event_normal_connected(void *data)
{
	static bool is_time_sync_started;

	wmprintf("Connected successfully to the configured network\r\n");
	if (!is_time_sync_started) {

		/* Start the server after device is connected */
		time_sync();
		is_time_sync_started = true;
	}
}

int main()
{
	/* initialize the standard input output facility over uart */
	if (wmstdio_init(UART0_ID, 0) != WM_SUCCESS) {
		return -WM_FAIL;
	}

	/* initialize gpio driver */
	if (gpio_drv_init() != WM_SUCCESS) {
		wmprintf("gpio_drv_init failed\r\n");
		return -WM_FAIL;
	}

	wmprintf("Build Time: " __DATE__ " " __TIME__ "\r\n");
	wmprintf("\r\n#### NTPC DEMO ####\r\n\r\n");

	/* This api starts micro-AP if device is not configured, else connects
	 * to configured network stored in persistent memory. Function
	 * wlan_event_normal_connected() is invoked on successful connection.
	 */
	wm_wlan_start(MICRO_AP_SSID, MICRO_AP_PASSPHRASE);
	return 0;
}
