/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */
/*
 * AWS Starter Demo Application
 *
 * Summary:
 *
 * Application demonstrates the bi-directional communication with the Thing
 * Shadow over MQTT. Using the web application, Configure the device to thing
 * shadow or any other shadow using its Thing name.
 *
 * Device publishes the changed state of the pushbuttons pb and pb_lambda on
 * Thing Shadow. Also it subscribes to the Thing Shadow delta and when state
 * change of LED is requested from AWS IOT web console, it toggles the LED
 * state.
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
#include <aws_iot_mqtt_client_interface.h>
#include <aws_iot_shadow_interface.h>
#include <aws_utils.h>
/* configuration parameters */
#include <aws_iot_config.h>

#include "aws_starter_root_ca_cert.h"

enum state {
	AWS_CONNECTED = 1,
	AWS_RECONNECTED,
	AWS_DISCONNECTED
};

/*-----------------------Global declarations----------------------*/

/* These hold each pushbutton's count, updated in the callback ISR */
static volatile uint32_t pushbutton_a_count;
static volatile uint32_t pushbutton_a_count_prev = -1;
static volatile uint32_t pushbutton_b_count;
static volatile uint32_t pushbutton_b_count_prev = -1;
static volatile uint32_t led_1_state;
static volatile uint32_t led_1_state_prev = -1;

static output_gpio_cfg_t led_1;
static AWS_IoT_Client mqtt_client;

/* Thread handle */
static os_thread_t aws_starter_thread;
/* Buffer to be used as stack */
static os_thread_stack_define(aws_starter_stack, 12 * 1024);
/* aws iot url */
static char url[128];

#define MICRO_AP_SSID                "aws_starter"
#define MICRO_AP_PASSPHRASE          "marvellwm"
#define AMAZON_ACTION_BUF_SIZE  100
#define VAR_LED_1_PROPERTY      "led"
#define VAR_BUTTON_A_PROPERTY   "pb"
#define VAR_BUTTON_B_PROPERTY   "pb_lambda"
#define RESET_TO_FACTORY_TIMEOUT 5000
#define BUFSIZE                  128
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

/* callback function invoked when pushbutton_a is pressed */
static void pushbutton_a_cb()
{
	if (pushbutton_a_count_prev == pushbutton_a_count)
		pushbutton_a_count++;
}

/* callback function invoked when pushbutton_b is pressed */
static void pushbutton_b_cb()
{
	if (pushbutton_b_count_prev == pushbutton_b_count)
		pushbutton_b_count++;
}

/* Configure led and pushbuttons with callback functions */
static void configure_led_and_button()
{
	/* respective GPIO pins for pushbuttons and leds are defined in
	 * board file.
	 */
	input_gpio_cfg_t pushbutton_a = {
		.gpio = board_button_1(),
		.type = GPIO_ACTIVE_LOW
	};
	input_gpio_cfg_t pushbutton_b = {
		.gpio = board_button_2(),
		.type = GPIO_ACTIVE_LOW
	};

	led_1 = board_led_1();

	push_button_set_cb(pushbutton_a,
			   pushbutton_a_cb,
			   100, 0, NULL);
	push_button_set_cb(pushbutton_b,
			   pushbutton_b_cb,
			   100, 0, NULL);
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

/* This function will get invoked when led state change request is received */
void led_indicator_cb(const char *p_json_string,
		      uint32_t json_string_datalen,
		      jsonStruct_t *p_context) {
	int state;
	if (p_context != NULL) {
		state = *(int *)(p_context->pData);
		if (state) {
			led_on(led_1);
			led_1_state = 1;
		} else {
			led_off(led_1);
			led_1_state = 0;
		}
	}
}

/* Publish thing state to shadow */
int aws_publish_property_state(ShadowConnectParameters_t *sp)
{
	char buf_out[BUFSIZE];
	char state[BUFSIZE];
	char *ptr = state;
	int ret = WM_SUCCESS;

	memset(state, 0, BUFSIZE);
	if (pushbutton_a_count_prev != pushbutton_a_count) {
		snprintf(buf_out, BUFSIZE, ",\"%s\":%lu", VAR_BUTTON_A_PROPERTY,
			 pushbutton_a_count);
		strcat(state, buf_out);
		pushbutton_a_count_prev = pushbutton_a_count;
	}
	if (pushbutton_b_count_prev != pushbutton_b_count) {
		snprintf(buf_out, BUFSIZE, ",\"%s\":%lu", VAR_BUTTON_B_PROPERTY,
			 pushbutton_b_count);
		strcat(state, buf_out);
		pushbutton_b_count_prev = pushbutton_b_count;
	}
	/* On receiving led state change notification from cloud, change
	 * the state of the led on the board in callback function and
	 * publish updated state on configured topic.
	 */
	if (led_1_state_prev != led_1_state) {
		snprintf(buf_out, BUFSIZE, ",\"%s\":%lu", VAR_LED_1_PROPERTY,
			 led_1_state);
		strcat(state, buf_out);
		led_1_state_prev = led_1_state;
	}

	if (*ptr == ',')
		ptr++;
	if (strlen(state)) {
		snprintf(buf_out, BUFSIZE, "{\"state\": {\"reported\":{%s}}}",
			 ptr);
		wmprintf("Publishing '%s' to AWS\r\n", buf_out);

		/* publish incremented value on pushbutton press on
		 * configured thing */
		ret = aws_iot_shadow_update(&mqtt_client,
					    sp->pMyThingName,
					    buf_out,
					    shadow_update_status_cb,
					    NULL,
					    10, true);
	}
	return ret;
}

/* application thread */
static void aws_starter_demo(os_thread_arg_t data)
{
	int led_state = 0, ret;
	jsonStruct_t led_indicator;
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

	/* indication that device is connected and cloud is started */
	led_on(board_led_2());
	wmprintf("Cloud Started\r\n");

	/* configures property of a thing */
	led_indicator.cb = led_indicator_cb;
	led_indicator.pData = &led_state;
	led_indicator.pKey = "led";
	led_indicator.type = SHADOW_JSON_INT8;

	/* subscribes to delta topic of the configured thing */
	ret = aws_iot_shadow_register_delta(&mqtt_client, &led_indicator);
	if (ret != WM_SUCCESS) {
		wmprintf("Failed to subscribe to shadow delta %d\r\n", ret);
		goto out;
	}

	while (1) {
		/* Implement application logic here */
		aws_iot_shadow_yield(&mqtt_client, 10);
		ret = aws_publish_property_state(&scp);
		if (ret != WM_SUCCESS)
			wmprintf("Sending property failed\r\n");
		os_thread_sleep(100);
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
}

void wlan_event_normal_connect_failed(void *data)
{
	/* led indication to indicate connect failed */
	aws_iot_shadow_disconnect(&mqtt_client);
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

	if (!aws_starter_thread) {
		/* set system time */
		wmtime_time_set_posix(time);

		/* create cloud thread */
		ret = os_thread_create(
			/* thread handle */
			&aws_starter_thread,
			/* thread name */
			"awsStarterDemo",
			/* entry function */
			aws_starter_demo,
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
	wmprintf("\r\n#### AWS STARTER DEMO ####\r\n\r\n");

	/* configure pushbutton on device to perform reset to factory */
	configure_reset_to_factory();
	/* configure led and pushbutton to communicate with cloud */
	configure_led_and_button();

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
