/**************************************************************************************************
  Filename:       snp_rpc.h
  Revised:        $Date: 2015-07-20 11:31:07 -0700 (Mon, 20 Jul 2015) $
  Revision:       $Revision: 44370 $

  Description:    This file contains the Simple Network Processor APIs as
                  Remote Procedure Calls to an external Network Processor

  Copyright 2015 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

#ifndef SNP_RPC_H
#define SNP_RPC_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "SNP.h"
  
/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */

  /*********************************************************************
 * TYPEDEFS
 */
  
typedef void (* SNP_eventCBRouter_t)(snpEvt_t *pEvt);
typedef void (* SNP_asyncCB_t)(uint8_t cmd1, snp_msg_t *pMsg, uint16_t msgLen);

typedef struct
{
  uint16_t   len;     // Length of the received message
  snp_msg_t  *pMsg;   // SNP message format.  This is a pointer to the application's buffer.
} snpSyncRspData_t;

/*********************************************************************
 * HELPER FUNCTIONS
 */

/*********************************************************************
 * @fn      SNP_registerSAPCBs
 *
 * @brief   Register SAP with SNP
 *
 * @param   asyncCB - pointer to NP asynchronous call back function
 * @param   eventCB - pointer to NP event call back function
 *
 * @return  none
 */
extern void SNP_registerSAPCBs(SNP_asyncCB_t asyncCB, SNP_eventCBRouter_t eventCB);

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      SNP_maskEvent
 *
 * @brief   mask of events to to receive from the Network Processor
 *
 * @param   pReq - pointer to SNP request message
 * @param   pRsp - pointer to SNP response message
 *
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_maskEvent(snpMaskEventReq_t *pReq, 
                             snpMaskEventRsp_t *pRsp);

/*********************************************************************
 * @fn      SNP_getRevision
 *
 * @brief   Get SNP Image revision and Stack revision numbers
 *
 * @param   pRsp - pointer to SNP response message
 *
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_getRevision(snpGetRevisionRsp_t *pRsp);

/*********************************************************************
 * @fn      SNP_sendHCICommand
 *
 * @brief   Send an HCI command over SNP message
 *
 * @param   pReq - pointer to SNP request message
 * @param   paramLen - length of data field of SNP request
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_sendHCICommand(snpHciCmdReq_t *pHciCmd, uint8_t paramLen);

/*********************************************************************
 * @fn      SNP_testCommand
 *
 * @brief   Get SNP memory statistics
 *
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_testCommand(void);

/*********************************************************************
 * @fn      SNP_getStatus
 *
 * @brief   Get SNP status
 *
 * @param   pRsp - pointer to SNP response message
 *
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_getStatus(snpGetStatusCmdRsp_t *pRsp);

/*********************************************************************
 * @fn      SNP_startAdvertising
 *
 * @brief   Start advertising.  Advertising data must be set with 
 *          SNP_updateAdvData first. Command sets the following:
 *
 * - Advertising type:  connectable, non-connectable discoverable 
 * or non-connectable non-discoverable (broadcasting)
 * - Timeout: advertising will cease after the timeout (in ms) expires.
 * - Interval: the delta (in ms) between two consecutive advertisements.
 * - Behavior after a connection occurs: if FALSE stop advertising after 
 * connection.  Else continue advertising.
 *
 * @param   pReq - pointer to SNP request message
 * 
 * @return  uint8_t - SNP_SUCCESS, SNP_ADV_DATA_NOT_READY, 
 *                    or SNP_ADV_NOT_AVAILABLE
 */
extern uint8_t SNP_startAdvertising(snpStartAdvReq_t *pReq);

/*********************************************************************
 * @fn      SNP_stopAdvertising
 *
 * @brief   Stop advertising
 *
 * @param   none
 *
 * @return  uint8_t - SNP_SUCCESS, SNP_NOT_ADVERTISING
 */
extern uint8_t SNP_stopAdvertising(void);

/*********************************************************************
 * @fn      SNP_setAdvertisementData
 *
 * @brief   Configure Advertising or Scan Response Data.
 *
 * @param   pReq - pointer to SNP request message
 * @param   dataLen - length of data field of SNP request
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_setAdvertisementData(snpSetAdvDataReq_t *pReq, 
                                        uint8_t datalen);

/*********************************************************************
 * @fn      SNP_terminateConnection
 *
 * @brief   Terminate the existing connection.
 *
 * @param   pReq - pointer to SNP request message
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_terminateConnection(snpTermConnReq_t *pReq);

/*********************************************************************
 * @fn      SNP_updateConnectionParams
 *
 * @brief   Update the parameters of an existing connection.
 *
 * @param   pReq - pointer to SNP request message
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_updateConnectionParams(snpUpdateConnParamReq_t *pReq);

/*********************************************************************
 * @fn      SNP_setGAPparam
 *
 * @brief   Set GAP parameter on SNP
 *
 * @param   pReq - pointer to SNP request message
 * @param   pRsp - pointer to SNP response message
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_setGAPparam(snpSetGapParamReq_t *pReq,
                               snpSetGapParamRsp_t *pRsp);

/*********************************************************************
 * @fn      SNP_getGAPparam
 *
 * @brief   Get GAP parameter from SNP
 *
 * @param   pReq - pointer to SNP request message
 * @param   pRsp - pointer to SNP response message
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_getGAPparam(snpGetGapParamReq_t *pReq,
                               snpGetGapParamRsp_t *pRsp);

/*********************************************************************
 * @fn      SNP_addService
 *
 * @brief   Add a service to the GATT server on the SNP.
 *
 * @param   pReq - pointer to SNP request message
 * @param   uuidLen - Length of UUID field in request message
 * @param   pRsp - pointer to SNP response message
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_addService(snpAddServiceReq_t *pReq, uint8_t uuidLen, 
                              snpAddServiceRsp_t *pRsp);

/*********************************************************************
 * @fn      SNP_addCharValueDec
 *
 * @brief   Add a characteristic value to a service table. Used after
 *          calling SNP_addService to include characteristics within the
 *          service.
 *
 * @param   pReq - pointer to SNP request message
 * @param   pReq - Length of UUID field in request message
 * @param   pRsp - pointer to SNP response message
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_addCharValueDec(snpAddCharValueDeclReq_t *pReq, uint8_t uuidLen,
                                   snpAddCharValueDeclRsp_t *pRsp);

/*********************************************************************
 * @fn      SNP_addCharDescDec
 *
 * @brief   Add a characteristic user description, CCCD, format attribute 
 *          to a characteristic on SNP. 
 *
 * @param   pReq - pointer to SNP request message
 * @param   pReq - Length of UUID field in request message
 * @param   pRsp - pointer to SNP response message
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_addCharDescDec(snpAddCharDescDeclReq_t *pReq,
                                  snpAddCharDescDeclRsp_t *pRsp);

/*********************************************************************
 * @fn      SNP_registerService
 *
 * @brief   Registers the service within the GATT Server.  This is called
 *          after a service is added with SNP_addService and all service 
 *          characteristics for that service are added with 
 *          SNP_addCharacteristic.
 *
 * @param   pReq - pointer to SNP request message
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_registerService(snpRegisterServiceRsp_t *pRsp);

/*********************************************************************
 * @fn      SNP_readCharCnf
 *
 * @brief   Respond to a characteristic read request from a GATT Client.
 *
 * @param   pCnf - pointer to SNP confirmation message
 * @param   size - length of the confirmation data payload
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_readCharCnf(snpCharReadCnf_t *pCnf, uint16_t size);

/*********************************************************************
 * @fn      SNP_writeCharCnf
 *
 * @brief   Respond to a characteristic write request from GATT Client
 *
 * @param   pCnf - pointer to SNP confirmation message
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_writeCharCnf(snpCharWriteCnf_t *pCnf);

/*********************************************************************
 * @fn      SNP_sendNotifInd
 *
 * @brief   Send a notification for a characteristic value to GATT Client
 *
 * @param   pReq - pointer to SNP request message
 * @param   dataLen - length of the notification data payload
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_sendNotifInd(snpNotifIndReq_t *pReq, uint8_t dataLen);

/*********************************************************************
 * @fn      SNP_charConfigUpdatedRsp
 *
 * @brief   A GATT Client has requested to updated the CCCD value. the AP 
 *          has a choice of validating the request or rejecting.
 *
 * @param   pReq - pointer to SNP response message
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_charConfigUpdatedRsp(snpCharCfgUpdatedRsp_t *pReq);

/*********************************************************************
 * @fn      SNP_setGATTParam
 *
 * @brief   Overwrite characteristic values of the Device Info Service.
 *
 * @param   pReq - pointer to SNP request message
 * @param   dataLen - Length of data field in request message
 * @param   pRsp - pointer to SNP response message
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_setGATTParam(snpSetGattParamReq_t *pReq, uint16_t dataLen,
                                snpSetGattParamRsp_t *pRsp);

/*********************************************************************
 * @fn      SNP_getGATTParam
 *
 * @brief   Get characteristic values of the Device Info Service.
 *
 * @param   pReq - pointer to SNP request message
 * @param   pRsp - pointer to SNP response message
 * @param   dataLen - Length of data field in response message
 * 
 * @return  uint8_t - SNP_SUCCESS
 */
extern uint8_t SNP_getGATTParam(snpGetGattParamReq_t *pReq, 
                                snpGetGattParamRsp_t *pRsp, uint16_t *dataLen);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SNP_RPC_H */
