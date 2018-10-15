/*
 *  Copyright 2008-2016 Marvell International Ltd.
 *  All Rights Reserved.
 */
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <string.h>
#include "aws_iot_error.h"
#include "network_interface.h"

#define NET_BLOCKING_OFF 1
#define NET_BLOCKING_ON	0

static inline int net_socket_blocking(int sock, int state)
{
	return lwip_ioctl(sock, FIONBIO, &state);
}

typedef int tls_handle_t;

typedef enum {
	/* TLS server mode */
	/* If this flag bit is zero client mode is assumed.*/
	TLS_SERVER_MODE = 0x01,
	TLS_CHECK_CLIENT_CERT = 0x02,

	/* TLS Client mode */
	TLS_CHECK_SERVER_CERT = 0x04,
	/* This will be needed if server mandates client certificate. If
	   this flag is enabled then client_cert and client_key from the
	   client structure in the union tls (from tls_init_config_t)
	   needs to be passed to tls_session_init() */
	TLS_USE_CLIENT_CERT = 0x08,
#ifdef CONFIG_WPA2_ENTP
	TLS_WPA2_ENTP = 0x10,
#endif
	/* Set this bit if given client_cert (client mode) or server_cert
	   (server mode) is a chained buffer */
	TLS_CERT_BUFFER_CHAINED = 0x20,
} tls_flags_t;

typedef struct {
	/** OR of flags defined in \ref tls_flags_t */
	int flags;
	/** Either a client or a server can be configured at a time through
	   tls_session_init(). Fill up appropriate structure from the
	   below union depending on your requirement. */
	union {
		/** Structure for client TLS configuration */
		struct {
			/**
			 * Needed if the RADIUS server mandates verification of
			 * CA certificate. Otherwise set to NULL.*/
			const unsigned char *ca_cert;
			/** Size of CA_cert */
			int ca_cert_size;
			/**
			 * Needed if the server mandates verification of
			 * client certificate. Otherwise set to NULL. In
			 * the former case please OR the flag
			 * TLS_USE_CLIENT_CERT to flags variable in
			 * tls_init_config_t passed to tls_session_init()
			 */
			const unsigned char *client_cert;
			/** Size of client_cert */
			int client_cert_size;
			/** Client private key */
			const unsigned char *client_key;
			/** Size of client key */
			int client_key_size;
		} client;
		/** Structure for server TLS configuration */
		struct {
			/** Mandatory. Will be sent to the client */
			const unsigned char *server_cert;
			/** Size of server_cert */
			int server_cert_size;
			/**
			 * Server private key. Mandatory.
			 * For the perusal of the server
			 */
			const unsigned char *server_key;
			/** Size of server_key */
			int server_key_size;
			/**
			 * Needed if the server wants to verify client
			 * certificate. Otherwise set to NULL.
			 */
			const unsigned char *client_cert;
			/** Size of client_cert */
			int client_cert_size;
		}server;
	} tls;
} tls_init_config_t;

int tls_lib_init(void);
int tls_session_init(tls_handle_t *h, int sockfd,
		     const tls_init_config_t *cfg);
int tls_send(tls_handle_t h, const void *buf, int len);
int tls_recv(tls_handle_t h, void *buf, int max_len);
void tls_close(tls_handle_t *h);

static void Populate_ConnectParams(Network *pNetwork, char *pRootCALocation, char *pDeviceCertLocation,
		 char *pDevicePrivateKeyLocation, char *pDestinationURL,
		 uint16_t destinationPort, uint32_t timeout_ms, bool ServerVerificationFlag)
{
	pNetwork->tlsConnectParams.DestinationPort = destinationPort;
	pNetwork->tlsConnectParams.pDestinationURL = pDestinationURL;
	pNetwork->tlsConnectParams.pDeviceCertLocation = pDeviceCertLocation;
	pNetwork->tlsConnectParams.pDevicePrivateKeyLocation = pDevicePrivateKeyLocation;
	pNetwork->tlsConnectParams.pRootCALocation = pRootCALocation;
	pNetwork->tlsConnectParams.timeout_ms = timeout_ms;	
}

IoT_Error_t iot_tls_init(Network *pNetwork, char *pRootCALocation, char *pDeviceCertLocation,
			 char *pDevicePrivateKeyLocation, char *pDestinationURL,
			 uint16_t destinationPort, uint32_t timeout_ms, bool ServerVerificationFlag)
{
	Populate_ConnectParams(pNetwork, pRootCALocation, pDeviceCertLocation,
			       pDevicePrivateKeyLocation, pDestinationURL,
			       destinationPort, timeout_ms, ServerVerificationFlag);

	pNetwork->my_socket = 0;
	pNetwork->connect = iot_tls_connect;
	pNetwork->read = iot_tls_read;
	pNetwork->write = iot_tls_write;
	pNetwork->disconnect = iot_tls_disconnect;
	pNetwork->isConnected = iot_tls_is_connected;
	pNetwork->destroy = iot_tls_destroy;

	tls_lib_init();

	return AWS_SUCCESS;
}

int Create_TCPSocket(void) {
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	return sockfd;
}

static void Close_TCPSocket(int *sockfd) {
	if (-1 == *sockfd)
		return;
	close(*sockfd);
	*sockfd = -1;
}

IoT_Error_t Connect_TCPSocket(int socket_fd, char *pURLString, int port) {

	IoT_Error_t ret_val = TCP_CONNECTION_ERROR;
	int connect_status = -1;
	struct hostent *host;
	struct sockaddr_in dest_addr;

	host = gethostbyname(pURLString);

	if (NULL != host) {
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(port);
		dest_addr.sin_addr.s_addr = *(long*) (host->h_addr);
		memset(&(dest_addr.sin_zero), '\0', 8);

		connect_status = connect(socket_fd, (struct sockaddr *) &dest_addr,
				sizeof(struct sockaddr));
		if (-1 != connect_status) {
			ret_val = AWS_SUCCESS;
		}
	}
	return ret_val;
}

IoT_Error_t setSocketToNonBlocking(int server_fd) {
	net_socket_blocking(server_fd, NET_BLOCKING_OFF);
	return 0;
}

tls_handle_t tls_handle;
tls_init_config_t tls_cfg;

IoT_Error_t iot_tls_connect(Network *pNetwork, TLSConnectParams *params)
{
	if (pNetwork == NULL)
		return NULL_VALUE_ERROR;

	IoT_Error_t ret_val = AWS_FAILURE;

	if (params != NULL) {
		Populate_ConnectParams(pNetwork, params->pRootCALocation,
				       params->pDeviceCertLocation,
				       params->pDevicePrivateKeyLocation,
				       params->pDestinationURL,
				       params->DestinationPort,
				       params->timeout_ms,
				       params->ServerVerificationFlag);
	}

	pNetwork->my_socket = Create_TCPSocket();
	if (-1 == pNetwork->my_socket) {
		ret_val = TCP_CONNECTION_ERROR;
		return ret_val;
	}

	ret_val = Connect_TCPSocket(pNetwork->my_socket,
				    pNetwork->tlsConnectParams.pDestinationURL,
				    pNetwork->tlsConnectParams.DestinationPort);
	if (AWS_SUCCESS != ret_val) {
		Close_TCPSocket(&pNetwork->my_socket);
		return ret_val;
	}
	tls_cfg.flags = TLS_USE_CLIENT_CERT;
	tls_cfg.tls.client.client_cert = (unsigned char *)
		pNetwork->tlsConnectParams.pDeviceCertLocation;
	tls_cfg.tls.client.client_cert_size =
		strlen(pNetwork->tlsConnectParams.pDeviceCertLocation);
	tls_cfg.tls.client.client_key = (unsigned char *)
		pNetwork->tlsConnectParams.pDevicePrivateKeyLocation;
	tls_cfg.tls.client.client_key_size =
		strlen(pNetwork->tlsConnectParams.pDevicePrivateKeyLocation);
	tls_cfg.tls.client.ca_cert =
		(unsigned char *)pNetwork->tlsConnectParams.pRootCALocation;
	tls_cfg.tls.client.ca_cert_size =
		strlen(pNetwork->tlsConnectParams.pRootCALocation);

	ret_val = tls_session_init(&tls_handle, pNetwork->my_socket, &tls_cfg);
	if (AWS_SUCCESS != ret_val)
		Close_TCPSocket(&pNetwork->my_socket);
	return ret_val;
}

IoT_Error_t iot_tls_read(Network *pNetwork, unsigned char *pMsg, size_t len,
			 Timer *timer, size_t *read_len)
{
	int val = -1;
	int recv_len = 0;
	fd_set fds;
	int error_flag = false, complete_flag = false;
	unsigned int timeout = timer->timeout;

	FD_ZERO(&fds);
	FD_SET(pNetwork->my_socket, &fds);
	setsockopt(pNetwork->my_socket, SOL_SOCKET, SO_RCVTIMEO,
		   &timeout, sizeof(timeout));

	do {
		if (tls_handle) {
			val = tls_recv(tls_handle, pMsg + recv_len,
				       len - recv_len);
		}
		if (val >= 0) {
			recv_len += val;
		} else {
			error_flag = true;
		}

		if (recv_len >= len) {
			complete_flag = true;
		}
	} while (!error_flag && !complete_flag);

	*read_len = recv_len;
	if (0 == *read_len && error_flag) {
		return NETWORK_SSL_NOTHING_TO_READ;
	} else if (has_timer_expired(timer) && !complete_flag) {
		return NETWORK_SSL_READ_TIMEOUT_ERROR;
	}

	return AWS_SUCCESS;
}

IoT_Error_t iot_tls_write(Network *pNetwork, unsigned char *pMsg, size_t len,
			  Timer *timer, size_t *written_len)
{
	if (tls_handle) {
		*written_len = tls_send(tls_handle, pMsg, len);
		return AWS_SUCCESS;
	}
	return AWS_FAILURE;
}

IoT_Error_t iot_tls_disconnect(Network *pNetwork)
{
	if (tls_handle)
		tls_close(&tls_handle);
	Close_TCPSocket(&pNetwork->my_socket);

	return AWS_SUCCESS;
}

IoT_Error_t iot_tls_destroy(Network *pNetwork)
{
	return AWS_SUCCESS;
}

IoT_Error_t iot_tls_is_connected(Network *pNetwork)
{
	return true;
}
