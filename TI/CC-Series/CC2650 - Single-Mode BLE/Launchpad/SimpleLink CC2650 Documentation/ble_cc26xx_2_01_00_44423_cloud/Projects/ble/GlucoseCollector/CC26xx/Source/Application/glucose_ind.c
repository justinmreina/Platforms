/*******************************************************************************
  Filename:       glucose_ind.c
  Revised:        $Date: 2014-11-04 14:27:46 -0800 (Tue, 04 Nov 2014) $
  Revision:       $Revision: 40983 $

  Description:    Glucose Collector App indication and notification handling 
                  routines for use with the CC2540 Bluetooth Low Energy
                  Protocol Stack.

  Copyright 2011 - 2014 Texas Instruments Incorporated. All rights reserved.

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
*******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include "string.h"
#include "bcomdef.h"

#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "glucservice.h"
#include "glucoseCollector.h"
#include "board_lcd.h"
#include <ti/drivers/LCD/LCDDogm1286.h>
#include "util.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define STR_MG_PER_DL "mg/dL:"
#define STR_MMOL_PER_L "mmol/L:"

/*********************************************************************
 * TYPEDEFS
 */

// Data in a glucose measurement as defined in the profile
typedef struct {
  uint8_t flags;
  uint16_t seqNum;
  uint8_t baseTime[7];
  int16 timeOffset;
  uint16_t concentration;
  uint8_t typeSampleLocation;
  uint16_t sensorStatus;
} glucoseMeas_t;

// Context data as defined in profile
typedef struct {
  uint8_t flags;
  uint16_t seqNum;
  uint8_t extendedFlags;
  uint8_t carboId;
  uint16_t carboVal;
  uint8_t mealVal;
  uint8_t TesterHealthVal;
  uint16_t exerciseDuration;
  uint8_t exerciseIntensity;
  uint8_t medId;
  uint16_t medVal;
  uint16_t HbA1cVal;
} glucoseContext_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
// Clock instances for internal periodic events.
extern Clock_Struct procTimeoutClock;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
// For test purposes
static glucoseMeas_t glucoseMeas;
static glucoseContext_t glucoseContext;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      glucoseIndGattMsg
 *
 * @brief   Handle indications and notifications. 
 *
 * @param   pMsg - GATT message.
 *
 * @return  none
 */
void glucoseIndGattMsg(gattMsgEvent_t *pMsg)
{
  uint8_t i;
  
  // Look up the handle in the handle cache
  for (i = 0; i < HDL_CACHE_LEN; i++)
  {
    if (pMsg->msg.handleValueInd.handle == glucoseHdlCache[i])
    {
      break;
    }
  }

  // Perform processing for this handle 
  switch (i)
  {
    case HDL_GLUCOSE_START:     
      {
        uint8_t *p = pMsg->msg.handleValueNoti.pValue;

        // restart procedure timer
        if (glucCollWritePending == true)
        {
          // start procedure timer
          Util_stopClock(&procTimeoutClock);
          Util_startClock(&procTimeoutClock);             
        }

        memset(&glucoseMeas, 0, sizeof(glucoseMeas));

        // Flags
        glucoseMeas.flags = *p++;

        // Sequence number
        glucoseMeas.seqNum = BUILD_UINT16(p[0], p[1]);
        LCD_WRITE_STRING_VALUE("SeqNum:", glucoseMeas.seqNum, 10, LCD_PAGE0);
        p += 2;

        // Base time
        memcpy(glucoseMeas.baseTime, p, 7);
        p += 7;

        // Time offset;
        if (glucoseMeas.flags & GLUCOSE_MEAS_FLAG_TIME_OFFSET)
        {
          glucoseMeas.timeOffset = BUILD_UINT16(p[0], p[1]);
          p += 2;
        }

        // Glucose concentration
        if(glucoseMeas.flags & GLUCOSE_MEAS_FLAG_CONCENTRATION)
        {
          glucoseMeas.concentration = BUILD_UINT16(p[0], p[1]);
          
          if(glucoseMeas.flags & GLUCOSE_MEAS_FLAG_UNITS)
          {
            LCD_WRITE_STRING_VALUE(STR_MMOL_PER_L, glucoseMeas.concentration, 
                                   10, LCD_PAGE1);
          }
          else
          {
            LCD_WRITE_STRING_VALUE(STR_MG_PER_DL, glucoseMeas.concentration, 
                                   10, LCD_PAGE1);
          }

          p += 2;

          // Type sample location
          glucoseMeas.typeSampleLocation = *p++;
        }

        // Sensor status annunciation
        if (glucoseMeas.flags & GLUCOSE_MEAS_FLAG_STATUS_ANNUNCIATION)
        {
          glucoseMeas.sensorStatus = BUILD_UINT16(p[0], p[1]);
          p += 2;
        }
      }
      break;

    case HDL_GLUCOSE_CONTEXT_START:  
      {
        uint8_t *p = pMsg->msg.handleValueNoti.pValue;

        // restart procedure timer
        if (glucCollWritePending == true)
        {     
          // start procedure timer
          Util_stopClock(&procTimeoutClock);
          Util_startClock(&procTimeoutClock);
        }

        memset(&glucoseContext, 0, sizeof(glucoseContext));

        // Flags
        glucoseContext.flags = *p++;

        // Sequence number
        glucoseContext.seqNum = BUILD_UINT16(p[0], p[1]);
        p += 2;

        // Extended flags
        if(glucoseContext.flags & GLUCOSE_CONTEXT_FLAG_EXTENDED)
        {
          glucoseContext.extendedFlags = *p++;
        }
        
        // Carbohydrate
        if(glucoseContext.flags & GLUCOSE_CONTEXT_FLAG_CARBO)
        {
          // carbohydrate ID
          glucoseContext.carboId = *p++;
          
          // Carbohydrate
          glucoseContext.carboVal = BUILD_UINT16(p[0], p[1]);          
          p += 2;
        }
        
        // Meal
        if(glucoseContext.flags & GLUCOSE_CONTEXT_FLAG_MEAL)
        {
          glucoseContext.mealVal = *p++;
        }
        
        // Tester health
        if(glucoseContext.flags & GLUCOSE_CONTEXT_FLAG_TESTER_HEALTH)
        {
          glucoseContext.TesterHealthVal = *p++;
        }
        
        // Exercise
        if(glucoseContext.flags & GLUCOSE_CONTEXT_FLAG_EXERCISE)
        {
          // Duration
          glucoseContext.exerciseDuration = BUILD_UINT16(p[0], p[1]);              
          p += 2;
          
          // Intensity
          glucoseContext.exerciseIntensity = *p++;
        }
        
        // Medication
        if(glucoseContext.flags & GLUCOSE_CONTEXT_FLAG_MEDICATION)
        {
          // Medication ID
          glucoseContext.medId = *p++;

          // Medication
          glucoseContext.medVal = BUILD_UINT16(p[0], p[1]);  
          p += 2;
        }
        
        // HbA1c
        if(glucoseContext.flags & GLUCOSE_CONTEXT_FLAG_HbA1c)
        {
          glucoseContext.HbA1cVal = BUILD_UINT16(p[0], p[1]);

          LCD_WRITE_STRING_VALUE("HbA1c:",  glucoseContext.HbA1cVal, 10, 
                                 LCD_PAGE2);
          p += 2;
        }
      }
      break;
      
    case HDL_GLUCOSE_CTL_PNT_START:
      {
        uint8_t *pValue = pMsg->msg.handleValueInd.pValue;

        // stop procedure timer
        Util_stopClock(&procTimeoutClock);

        if(pValue[0] == CTL_PNT_OP_NUM_RSP)
        {
          if(pMsg->msg.handleValueInd.len >= 3)
          {
            LCD_WRITE_STRING("Matching ",  LCD_PAGE0);
            LCD_WRITE_STRING("Records:", LCD_PAGE1);
            LCD_WRITE_STRING_VALUE("", BUILD_UINT16(pValue[2], pValue[3]), 10, 
                                   LCD_PAGE2);
          }
        }
        else if(pValue[0] == CTL_PNT_OP_REQ_RSP && glucCollClearPending)
        {
          glucCollClearPending = false;
          
          if(pMsg->msg.handleValueInd.len >= 3)
          {
            switch(pValue[3])
            {
              case CTL_PNT_RSP_SUCCESS:
                LCD_WRITE_STRING("Records",  LCD_PAGE0);
                LCD_WRITE_STRING("Cleared", LCD_PAGE1);
                LCD_WRITE_STRING("", LCD_PAGE2);
                break;
                
              case CTL_PNT_RSP_NO_RECORDS:
                LCD_WRITE_STRING("No Matching",  LCD_PAGE0);
                LCD_WRITE_STRING("Records", LCD_PAGE1);
                LCD_WRITE_STRING("to Delete", LCD_PAGE2);
                break;
                
              default:
                LCD_WRITE_STRING("Error:",  LCD_PAGE0);
                LCD_WRITE_STRING_VALUE("", pValue[3], 10, LCD_PAGE1);  
                LCD_WRITE_STRING("", LCD_PAGE2);
                break;
            }
          }
        }
        else if(pValue[0] == CTL_PNT_OP_REQ_RSP)
        {
          if(pMsg->msg.handleValueInd.len >= 3)
          {
            switch(pValue[3])
            {
              case CTL_PNT_RSP_SUCCESS:
                break;
                
              case CTL_PNT_RSP_NO_RECORDS:
                LCD_WRITE_STRING("No Matching",  LCD_PAGE0);
                LCD_WRITE_STRING("Records", LCD_PAGE1);
                LCD_WRITE_STRING("Found", LCD_PAGE2);
                break;

              default:
                LCD_WRITE_STRING("Error:",  LCD_PAGE0);
                LCD_WRITE_STRING_VALUE("", pValue[3], 10, LCD_PAGE1);  
                LCD_WRITE_STRING("", LCD_PAGE2);
                break;
            }
          }
        }
      }
      break;      
        
    default:
      break;
  }
  
  // Send confirm for indication
  if (pMsg->method == ATT_HANDLE_VALUE_IND)
  {
    ATT_HandleValueCfm(pMsg->connHandle);
  }
}


/*********************************************************************
 *********************************************************************/
