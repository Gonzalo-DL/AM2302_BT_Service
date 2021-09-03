/*
 * environmentalSensingService.c
 *
 *  Created on: 03-09-2021
 *      Author: Gonzalo
 */


#include <string.h>

#include <icall.h>

/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include <Profiles/environmentalSensingService.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
* GLOBAL VARIABLES
*/

// environmentalSensing Service UUID
CONST uint8_t environmentalSensingUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(ENVIRONMENTALSENSING_SERV_UUID), HI_UINT16(ENVIRONMENTALSENSING_SERV_UUID)
};

// temperatureCelsius UUID
CONST uint8_t environmentalSensing_TemperatureCelsiusUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(ENVIRONMENTALSENSING_TEMPERATURECELSIUS_UUID), HI_UINT16(ENVIRONMENTALSENSING_TEMPERATURECELSIUS_UUID)
};
// relativeHumidity UUID
CONST uint8_t environmentalSensing_RelativeHumidityUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(ENVIRONMENTALSENSING_RELATIVEHUMIDITY_UUID), HI_UINT16(ENVIRONMENTALSENSING_RELATIVEHUMIDITY_UUID)
};
// adquisitionFrequency UUID
CONST uint8_t environmentalSensing_AdquisitionFrequencyUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_UUID), HI_UINT16(ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_UUID)
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static environmentalSensingCBs_t *pAppCBs = NULL;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t environmentalSensingDecl = { ATT_BT_UUID_SIZE, environmentalSensingUUID };

// Characteristic "TemperatureCelsius" Properties (for declaration)
static uint8_t environmentalSensing_TemperatureCelsiusProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic "TemperatureCelsius" Value variable
static uint8_t environmentalSensing_TemperatureCelsiusVal[ENVIRONMENTALSENSING_TEMPERATURECELSIUS_LEN] = {0};

// Characteristic "TemperatureCelsius" CCCD User Description
static uint8 environmentalSensing_TemperatureCelsiusDesc[17] = "Temperature";

// Characteristic "TemperatureCelsius" CCCD
static gattCharCfg_t *environmentalSensing_TemperatureCelsiusConfig;

// Characteristic "RelativeHumidity" Properties (for declaration)
static uint8_t environmentalSensing_RelativeHumidityProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic "RelativeHumidity" Value variable
static uint8_t environmentalSensing_RelativeHumidityVal[ENVIRONMENTALSENSING_RELATIVEHUMIDITY_LEN] = {0};

// Characteristic "RelativeHumidity" CCCD User Description
static uint8 environmentalSensing_RelativeHumidityDesc[17] = "Humidity";

// Characteristic "RelativeHumidity" CCCD
static gattCharCfg_t *environmentalSensing_RelativeHumidityConfig;

// Characteristic "AdquisitionFrequency" Properties (for declaration)
static uint8_t environmentalSensing_AdquisitionFrequencyProps = GATT_PROP_READ | GATT_PROP_WRITE | GATT_PROP_WRITE_NO_RSP | GATT_PROP_NOTIFY;

// Characteristic "AdquisitionFrequency" Value variable
static uint8_t environmentalSensing_AdquisitionFrequencyVal[ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_LEN] = {0};

// Characteristic "AdquisitionFrequency" CCCD User Description
static uint8 environmentalSensing_AdquisitionFrequencyDesc[17] = "Adq Period";

// Characteristic "AdquisitionFrequency" CCCD
static gattCharCfg_t *environmentalSensing_AdquisitionFrequencyConfig;

/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t environmentalSensingAttrTbl[] =
{
  // environmentalSensing Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&environmentalSensingDecl
  },
    // TemperatureCelsius Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &environmentalSensing_TemperatureCelsiusProps
    },
      // TemperatureCelsius Characteristic Value
      {
        { ATT_BT_UUID_SIZE, environmentalSensing_TemperatureCelsiusUUID },
        GATT_PERMIT_AUTHEN_READ,
        0,
        environmentalSensing_TemperatureCelsiusVal
      },
      // TemperatureCelsius User Description
        {
          { ATT_BT_UUID_SIZE, charUserDescUUID },
          GATT_PERMIT_READ,
          0,
          environmentalSensing_TemperatureCelsiusDesc
        },
      // TemperatureCelsius CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE,
        0,
        (uint8 *)&environmentalSensing_TemperatureCelsiusConfig
      },
    // RelativeHumidity Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &environmentalSensing_RelativeHumidityProps
    },
      // RelativeHumidity Characteristic Value
      {
        { ATT_BT_UUID_SIZE, environmentalSensing_RelativeHumidityUUID },
        GATT_PERMIT_READ,
        0,
        environmentalSensing_RelativeHumidityVal
      },
      // RelativeHumidity User Description
        {
          { ATT_BT_UUID_SIZE, charUserDescUUID },
          GATT_PERMIT_READ,
          0,
          environmentalSensing_RelativeHumidityDesc
        },
      // RelativeHumidity CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&environmentalSensing_RelativeHumidityConfig
      },
    // AdquisitionFrequency Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &environmentalSensing_AdquisitionFrequencyProps
    },
      // AdquisitionFrequency Characteristic Value
      {
        { ATT_BT_UUID_SIZE, environmentalSensing_AdquisitionFrequencyUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        environmentalSensing_AdquisitionFrequencyVal
      },
      // AdquisitionFrequency User Description
        {
          { ATT_BT_UUID_SIZE, charUserDescUUID },
          GATT_PERMIT_READ,
          0,
          environmentalSensing_AdquisitionFrequencyDesc
        },
      // AdquisitionFrequency CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&environmentalSensing_AdquisitionFrequencyConfig
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t environmentalSensing_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                           uint16_t maxLen, uint8_t method );
static bStatus_t environmentalSensing_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len, uint16_t offset,
                                            uint8_t method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t environmentalSensingCBs =
{
  environmentalSensing_ReadAttrCB,  // Read callback function pointer
  environmentalSensing_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 * EnvironmentalSensing_AddService- Initializes the EnvironmentalSensing service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t EnvironmentalSensing_AddService( uint8_t rspTaskId )
{
  uint8_t status;

  // Allocate Client Characteristic Configuration table
  environmentalSensing_TemperatureCelsiusConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( environmentalSensing_TemperatureCelsiusConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID, environmentalSensing_TemperatureCelsiusConfig );
  // Allocate Client Characteristic Configuration table
  environmentalSensing_RelativeHumidityConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( environmentalSensing_RelativeHumidityConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID, environmentalSensing_RelativeHumidityConfig );
  // Allocate Client Characteristic Configuration table
  environmentalSensing_AdquisitionFrequencyConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( environmentalSensing_AdquisitionFrequencyConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID, environmentalSensing_AdquisitionFrequencyConfig );
  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( environmentalSensingAttrTbl,
                                        GATT_NUM_ATTRS( environmentalSensingAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &environmentalSensingCBs );

  return ( status );
}

/*
 * EnvironmentalSensing_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t EnvironmentalSensing_RegisterAppCBs( environmentalSensingCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    pAppCBs = appCallbacks;

    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*
 * EnvironmentalSensing_SetParameter - Set a EnvironmentalSensing parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t EnvironmentalSensing_SetParameter( uint8_t param, uint16_t len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case ENVIRONMENTALSENSING_TEMPERATURECELSIUS_ID:
      if ( len == ENVIRONMENTALSENSING_TEMPERATURECELSIUS_LEN )
      {
        memcpy(environmentalSensing_TemperatureCelsiusVal, value, len);

        // Try to send notification.
        GATTServApp_ProcessCharCfg( environmentalSensing_TemperatureCelsiusConfig, (uint8_t *)&environmentalSensing_TemperatureCelsiusVal, FALSE,
                                    environmentalSensingAttrTbl, GATT_NUM_ATTRS( environmentalSensingAttrTbl ),
                                    INVALID_TASK_ID,  environmentalSensing_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case ENVIRONMENTALSENSING_RELATIVEHUMIDITY_ID:
      if ( len == ENVIRONMENTALSENSING_RELATIVEHUMIDITY_LEN )
      {
        memcpy(environmentalSensing_RelativeHumidityVal, value, len);

        // Try to send notification.
        GATTServApp_ProcessCharCfg( environmentalSensing_RelativeHumidityConfig, (uint8_t *)&environmentalSensing_RelativeHumidityVal, FALSE,
                                    environmentalSensingAttrTbl, GATT_NUM_ATTRS( environmentalSensingAttrTbl ),
                                    INVALID_TASK_ID,  environmentalSensing_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_ID:
      if ( len == ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_LEN )
      {
        memcpy(environmentalSensing_AdquisitionFrequencyVal, value, len);

        // Try to send notification.
        GATTServApp_ProcessCharCfg( environmentalSensing_AdquisitionFrequencyConfig, (uint8_t *)&environmentalSensing_AdquisitionFrequencyVal, FALSE,
                                    environmentalSensingAttrTbl, GATT_NUM_ATTRS( environmentalSensingAttrTbl ),
                                    INVALID_TASK_ID,  environmentalSensing_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*
 * EnvironmentalSensing_GetParameter - Get a EnvironmentalSensing parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t EnvironmentalSensing_GetParameter( uint8_t param, uint16_t *len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_ID:
      memcpy(value, environmentalSensing_AdquisitionFrequencyVal, ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_LEN);
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*********************************************************************
 * @fn          environmentalSensing_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t environmentalSensing_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                       uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                       uint16_t maxLen, uint8_t method )
{
  bStatus_t status = SUCCESS;

  // See if request is regarding the TemperatureCelsius Characteristic Value
if ( ! memcmp(pAttr->type.uuid, environmentalSensing_TemperatureCelsiusUUID, pAttr->type.len) )
  {
    if ( offset > ENVIRONMENTALSENSING_TEMPERATURECELSIUS_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, ENVIRONMENTALSENSING_TEMPERATURECELSIUS_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the RelativeHumidity Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, environmentalSensing_RelativeHumidityUUID, pAttr->type.len) )
  {
    if ( offset > ENVIRONMENTALSENSING_RELATIVEHUMIDITY_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, ENVIRONMENTALSENSING_RELATIVEHUMIDITY_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the AdquisitionFrequency Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, environmentalSensing_AdquisitionFrequencyUUID, pAttr->type.len) )
  {
    if ( offset > ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has READ permissions.
    *pLen = 0;
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  return status;
}


/*********************************************************************
 * @fn      environmentalSensing_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t environmentalSensing_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                        uint8_t *pValue, uint16_t len, uint16_t offset,
                                        uint8_t method )
{
  bStatus_t status  = SUCCESS;
  uint8_t   paramID = 0xFF;

  // See if request is regarding a Client Characterisic Configuration
  if ( ! memcmp(pAttr->type.uuid, clientCharCfgUUID, pAttr->type.len) )
  {
    // Allow only notifications.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY);
  }
  // See if request is regarding the AdquisitionFrequency Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, environmentalSensing_AdquisitionFrequencyUUID, pAttr->type.len) )
  {
    if ( offset + len > ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      // Copy pValue into the variable we point to from the attribute table.
      memcpy(pAttr->pValue + offset, pValue, len);

      // Only notify application if entire expected value is written
      if ( offset + len == ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_LEN)
        paramID = ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_ID;
    }
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has WRITE permissions.
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  // Let the application know something changed (if it did) by using the
  // callback it registered earlier (if it did).
  if (paramID != 0xFF)
    if ( pAppCBs && pAppCBs->pfnChangeCb )
      pAppCBs->pfnChangeCb(connHandle, paramID, len, pValue); // Call app function from stack task context.

  return status;
}
