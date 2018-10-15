/*
 *  Copyright (C) 2008-2016, Marvell International Ltd.
 *  All Rights Reserved.
 */
/*
 * Connected Maraca Application
 *
 * Summary:
 *
 * Device publishes the changed state of the maraca on
 * Thing Shadow.
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
#include <aws_iot_mqtt_client_interface.h>
#include <aws_iot_shadow_interface.h>
#include <aws_utils.h>
#include <mdev_gpio.h>
#include <mdev_pinmux.h>
#include <lowlevel_drivers.h>
#include <wm_os.h>
#include <mdev_i2c.h>
#include <push_button.h>
#include <stdlib.h>

/* configuration parameters */
#include <aws_iot_config.h>

#include "aws_starter_root_ca_cert.h"
#include "sensor_acc_drv.h"

enum state {
	AWS_CONNECTED = 1,
	AWS_RECONNECTED,
	AWS_DISCONNECTED
};

/*-----------------------Global declarations----------------------*/

static AWS_IoT_Client mqtt_client;
static enum state device_state;

/* Thread handle */
static os_thread_t aws_starter_thread;
/* Buffer to be used as stack */
static os_thread_stack_define(aws_starter_stack, 12 * 1024);
/* aws iot url */
static char url[128];

#define MICRO_AP_SSID                "aws_starter"
#define MICRO_AP_PASSPHRASE          "marvellwm"
#define AMAZON_ACTION_BUF_SIZE  100
#define RESET_TO_FACTORY_TIMEOUT 5000
#define BUFSIZE                  200
#define THRESHOLD_ACC            2
#define DEVICE_ID                "<INSERT_YOUR_DEVICE_ID>"
#define MAX_MAC_BYTES            6

/* callback function invoked on reset to factory */
static void device_reset_to_factory_cb()
{
	/* Clears device configuration settings from persistent memory
	 * and reboots the device.
	 */
	invoke_reset_to_factory();
}

/* board_button_2() is configured to perform reset to factory,
 * when pressed for more than 5 seconds.
 */
static void configure_reset_to_factory()
{
	input_gpio_cfg_t pushbutton_reset_to_factory = {
		.gpio = board_button_2(),
		.type = GPIO_ACTIVE_LOW
	};
	push_button_set_cb(pushbutton_reset_to_factory,
			   device_reset_to_factory_cb,
			   RESET_TO_FACTORY_TIMEOUT, 0, NULL);
}



static char client_cert_buffer[AWS_PUB_CERT_SIZE];
static char private_key_buffer[AWS_PRIV_KEY_SIZE];
#define THING_LEN 126
#define REGION_LEN 16
static char thing_name[THING_LEN];
static char client_id[MAX_SIZE_OF_UNIQUE_CLIENT_ID_BYTES];
/* populate aws shadow configuration details */
static int aws_starter_load_configuration(ShadowInitParameters_t *sp,
					  ShadowConnectParameters_t *scp)
{
	int ret = WM_SUCCESS;
	char region[REGION_LEN];
	uint8_t device_mac[MAX_MAC_BYTES];
	memset(region, 0, sizeof(region));

	/* read configured thing name from the persistent memory */
	ret = read_aws_thing(thing_name, THING_LEN);
	if (ret != WM_SUCCESS) {
		wmprintf("Failed to configure thing. Returning!\r\n");
		return -WM_FAIL;
	}
	scp->pMyThingName = thing_name;

	/* read device MAC address */
	ret = read_aws_device_mac(device_mac);
	if (ret != WM_SUCCESS) {
		wmprintf("Failed to read device mac address. Returning!\r\n");
		return -WM_FAIL;
	}
	/* Unique client ID in the format prefix-6 byte MAC address */
	snprintf(client_id, MAX_SIZE_OF_UNIQUE_CLIENT_ID_BYTES,
		 "%s-%02x%02x%02x%02x%02x%02x", AWS_IOT_MQTT_CLIENT_ID,
		 device_mac[0], device_mac[1], device_mac[2],
		 device_mac[3], device_mac[4], device_mac[5]);
	scp->pMqttClientId = client_id;
	scp->mqttClientIdLen = (uint16_t) strlen(client_id);

	/* read configured region name from the persistent memory */
	ret = read_aws_region(region, REGION_LEN);
	if (ret == WM_SUCCESS) {
		snprintf(url, sizeof(url), "data.iot.%s.amazonaws.com",
			 region);
	} else {
		snprintf(url, sizeof(url), "data.iot.%s.amazonaws.com",
			 AWS_IOT_MY_REGION_NAME);
	}
	sp->pHost = url;
	sp->port = AWS_IOT_MQTT_PORT;
	sp->pRootCA = rootCA;
	sp->enableAutoReconnect = true;
	sp->disconnectHandler = NULL;

	/* read configured certificate from the persistent memory */
	ret = read_aws_certificate(client_cert_buffer, AWS_PUB_CERT_SIZE);
	if (ret != WM_SUCCESS) {
		wmprintf("Failed to configure certificate. Returning!\r\n");
		return -WM_FAIL;
	}
	sp->pClientCRT = client_cert_buffer;

	/* read configured private key from the persistent memory */
	ret = read_aws_key(private_key_buffer, AWS_PRIV_KEY_SIZE);
	if (ret != WM_SUCCESS) {
		wmprintf("Failed to configure key. Returning!\r\n");
		return -WM_FAIL;
	}
	sp->pClientKey = private_key_buffer;

	return ret;
}

void shadow_update_status_cb(const char *pThingName, ShadowActions_t action,
			     Shadow_Ack_Status_t status,
			     const char *pReceivedJsonDocument,
			     void *pContextData) {

	if (status == SHADOW_ACK_TIMEOUT) {
		wmprintf("Shadow publish state change timeout occurred\r\n");
	} else if (status == SHADOW_ACK_REJECTED) {
		wmprintf("Shadow publish state change rejected\r\n");
	} else if (status == SHADOW_ACK_ACCEPTED) {
		wmprintf("Shadow publish state change accepted\r\n");
	}
}


/* Publish thing state to shadow */
int aws_publish_property_state(ShadowConnectParameters_t *sp, int8_t x, int8_t y,
			       int8_t z)
{
	char buf_out[BUFSIZE];
	int ret = WM_SUCCESS;

	IoT_Publish_Message_Params cmaraca;
	snprintf(buf_out, BUFSIZE, "{\"device_id\": \"%s\",\"time\":\"\",\"device\":\"marvelliot\",\"sensors\":[{\"telemetryData\": {\"xval\": %d,\"yval\":%d,\"zval\":%d}}]}",DEVICE_ID, x, y, z);
	
	memset(&cmaraca, 0, sizeof(cmaraca));
	cmaraca.payload = buf_out;
	cmaraca.payloadLen = strlen(buf_out);
	aws_iot_mqtt_publish(&mqtt_client, "connected-maraca", 11, &cmaraca);
	
	wmprintf("\r\n%s", buf_out);
	
	return ret;
}

/* application thread */
static void connected_maraca(os_thread_arg_t data)
{
        int ret;

	ShadowInitParameters_t sp;
	ShadowConnectParameters_t scp;

	ret = aws_starter_load_configuration(&sp, &scp);
	if (ret != WM_SUCCESS) {
		wmprintf("aws shadow configuration failed : %d\r\n", ret);
		goto out;
	}

	ret = aws_iot_shadow_init(&mqtt_client, &sp);
	if (ret != WM_SUCCESS) {
		wmprintf("aws shadow init failed : %d\r\n", ret);
		goto out;
	}

	ret = aws_iot_shadow_connect(&mqtt_client, &scp);
	if (ret != WM_SUCCESS) {
		wmprintf("aws shadow connect failed : %d\r\n", ret);
		goto out;
	}

	wmprintf("Cloud Started\r\n");

	while (1) {
		/* Implement application logic here */

		if (device_state == AWS_RECONNECTED) {
			ret = aws_iot_shadow_init(&mqtt_client, &sp);
			if (ret != WM_SUCCESS) {
				wmprintf("aws shadow init failed: "
					 "%d\r\n", ret);
				goto out;
			}
			ret = aws_iot_shadow_connect(&mqtt_client, &scp);
			if (ret != WM_SUCCESS) {
				wmprintf("aws shadow reconnect failed: "
					 "%d\r\n", ret);
				goto out;
			} else {
				device_state = AWS_CONNECTED;
				wmprintf("Reconnected to cloud\r\n");
			}
		}

		aws_iot_shadow_yield(&mqtt_client, 10);

		int8_t x, y, z;
		static int8_t prev_x, prev_y, prev_z;
		MMA7660_getXYZ(&x, &y, &z);
		if( abs(prev_x-x) > THRESHOLD_ACC || abs(prev_y-y)> THRESHOLD_ACC || abs(prev_z-z)> THRESHOLD_ACC) {
			ret = aws_publish_property_state(&scp, x, y, z);
			wmprintf("\r\n%d, %d, %d\r\n",x,y,z);
			if (ret != WM_SUCCESS)
				wmprintf("Sending property failed\r\n");
			prev_x=x;
			prev_y=y;
			prev_z=z;
		}


		os_thread_sleep(1000);
	}
	
	ret = aws_iot_shadow_disconnect(&mqtt_client);
	if (AWS_SUCCESS != ret) {
		wmprintf("aws iot shadow error %d\r\n", ret);
	}

out:
	os_thread_self_complete(NULL);
	return;
}

void wlan_event_normal_link_lost(void *data)
{
	/* led indication to indicate link loss */
	aws_iot_shadow_disconnect(&mqtt_client);
	device_state = AWS_DISCONNECTED;
}

void wlan_event_normal_connect_failed(void *data)
{
	/* led indication to indicate connect failed */
	aws_iot_shadow_disconnect(&mqtt_client);
	device_state = AWS_DISCONNECTED;
}

/* This function gets invoked when station interface connects to home AP.
 * Network dependent services can be started here.
 */
void wlan_event_normal_connected(void *data)
{
	int ret;
	/* Default time set to 1 April 2016 */
	time_t time = 1459468800;

	wmprintf("Connected successfully to the configured network\r\n");

	if (!device_state) {
		/* set system time */
		wmtime_time_set_posix(time);

		/* create cloud thread */
		ret = os_thread_create(
			/* thread handle */
			&aws_starter_thread,
			/* thread name */
			"connectedMaraca",
			/* entry function */
			connected_maraca,
			/* argument */
			0,
			/* stack */
			&aws_starter_stack,
			/* priority */
			OS_PRIO_3);
		if (ret != WM_SUCCESS) {
			wmprintf("Failed to start cloud_thread: %d\r\n", ret);
			return;
		}
	}

	if (!device_state)
		device_state = AWS_CONNECTED;
	else if (device_state == AWS_DISCONNECTED)
		device_state = AWS_RECONNECTED;
}

int main()
{
	/* initialize the standard input output facility over uart */
	if (wmstdio_init(UART0_ID, 0) != WM_SUCCESS) {
		return -WM_FAIL;
	}

	wmprintf("Build Time: " __DATE__ " " __TIME__ "\r\n");
	wmprintf("\r\n#### CONNECTED MARACA DEMO ####\r\n\r\n");

	/* initialize gpio driver */
	if (gpio_drv_init() != WM_SUCCESS) {
		wmprintf("gpio_drv_init failed\r\n");
		return -WM_FAIL;
	}

	i2c_drv_init(I2C0_PORT);
	mdev_t *i2c0 = i2c_drv_open(I2C0_PORT, I2C_SLAVEADR(MMA7660_ADDR));
	MMA7660_init(i2c0);

	/* configure pushbutton on device to perform reset to factory */
	configure_reset_to_factory();

	/* This api adds aws iot configuration support in web application.
	 * Configuration details are then stored in persistent memory.
	 */
	enable_aws_config_support();

	/* This api starts micro-AP if device is not configured, else connects
	 * to configured network stored in persistent memory. Function
	 * wlan_event_normal_connected() is invoked on successful connection.
	 */
	wm_wlan_start(MICRO_AP_SSID, MICRO_AP_PASSPHRASE);
	return 0;
}
