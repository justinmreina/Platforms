/*
 *  Copyright (C) 2008-2016, Marvell International Ltd.
 *  All Rights Reserved.
 */
/*
 * WiFi Basics
 *
 * Summary:
 *
 * This application shows how to use wlan_* functions
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


#define MICRO_AP_SSID                "wifi-basics"
#define MICRO_AP_PASSPHRASE          "marvellwm"


void wlan_event_normal_link_lost(void *data)
{
	wmprintf("wlan_event_normal_link_lost() has been invoked \r\n");
}

void wlan_event_normal_connect_failed(void *data)
{
	wmprintf("wlan_event_normal_connect_failed() has been invoked \r\n");
}

/* This function gets invoked when station interface connects to home AP.
 * Network dependent services can be started here.
 */
void wlan_event_normal_connected(void *data)
{
	wmprintf("wlan_event_normal_connected() has been invoked \r\n");
}

int main()
{
	/* initialize the standard input output facility over uart */
	if (wmstdio_init(UART0_ID, 0) != WM_SUCCESS) {
		return -WM_FAIL;
	}

	wmprintf("Build Time: " __DATE__ " " __TIME__ "\r\n");
	wmprintf("\r\n#### WIFI BASICS ####\r\n\r\n");

	/* This api starts micro-AP if device is not configured, else connects
	 * to configured network stored in persistent memory. Function
	 * wlan_event_normal_connected() is invoked on successful connection.
	 */
	wm_wlan_start(MICRO_AP_SSID, MICRO_AP_PASSPHRASE);
	return 0;
}
