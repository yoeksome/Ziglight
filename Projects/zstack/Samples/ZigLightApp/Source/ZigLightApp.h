/**************************************************************************************************
  Filename:       ZigLightApp.h
  Revised:        $Date: 2007-10-27 17:22:23 -0700 (Sat, 27 Oct 2007) $
  Revision:       $Revision: 15795 $

  Description:    This file contains the Sample Application definitions.


  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
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

#ifndef ZigLightApp_H
#define ZigLightApp_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"

/*********************************************************************
 * CONSTANTS
 */

// These constants are only for example and should be changed to the
// device's needs
#define ZigLightApp_ENDPOINT           20

#define ZigLightApp_PROFID             0x0F08
#define ZigLightApp_DEVICEID           0x0001
#define ZigLightApp_DEVICE_VERSION     0
#define ZigLightApp_FLAGS              0

#define ZigLightApp_MAX_CLUSTERS       3 //2
#define ZigLightApp_PERIODIC_CLUSTERID 1
#define ZigLightApp_FLASH_CLUSTERID    2
#define ZigLightApp_POINT_TO_POINT_CLUSTERID  3

// Send Message Timeout
#define ZigLightApp_SEND_PERIODIC_MSG_TIMEOUT   3000     // Every 3 seconds
#define ZigLightApp_SEND_HEARTBEAT_MSG_TIMEOUT   10000     // Every 10 seconds by liu
#define ZigLightApp_SEND_INIT_REQUEST_MSG_TIMEOUT   1000     // Every 1 seconds by liu
  
// Application Events (OSAL) - These are bit weighted definitions.
#define ZigLightApp_SEND_PERIODIC_MSG_EVT       0x0001
  
// Group ID for Flash Command
#define ZigLightApp_FLASH_GROUP                  0x0001
  
// Flash Command Duration - in milliseconds
#define ZigLightApp_FLASH_DURATION               1000

/*********************************************************************
 * MACROS
 */
#define GUOHUA_LIGHT 0x04
//#define GUOHUA_PLUG 0x08
//#define GUOHUA_POWERSTRIP 0x0c
/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the Generic Application
 */
extern void ZigLightApp_Init( uint8 task_id );

/*
 * Task Event Processor for the Generic Application
 */
extern UINT16 ZigLightApp_ProcessEvent( uint8 task_id, uint16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZigLightApp_H */
