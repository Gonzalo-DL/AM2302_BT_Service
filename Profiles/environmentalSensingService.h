/*
 * environmentalSensingService.h
 *
 *  Created on: 03-09-2021
 *      Author: Gonzalo
 */

#ifndef SERVICES_ENVIRONMENTALSENSINGSERVICE_H_
#define SERVICES_ENVIRONMENTALSENSINGSERVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <bcomdef.h>
/*********************************************************************
* CONSTANTS
*/

// Service UUID
#define ENVIRONMENTALSENSING_SERV_UUID 0x181A

//  Characteristic defines
#define ENVIRONMENTALSENSING_TEMPERATURECELSIUS_ID   0
#define ENVIRONMENTALSENSING_TEMPERATURECELSIUS_UUID 0x272F
#define ENVIRONMENTALSENSING_TEMPERATURECELSIUS_LEN  2

//  Characteristic defines
#define ENVIRONMENTALSENSING_RELATIVEHUMIDITY_ID   1
#define ENVIRONMENTALSENSING_RELATIVEHUMIDITY_UUID 0x2A6F
#define ENVIRONMENTALSENSING_RELATIVEHUMIDITY_LEN  2

//  Characteristic defines
#define ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_ID   2
#define ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_UUID 0x27AF
#define ENVIRONMENTALSENSING_ADQUISITIONFREQUENCY_LEN  2

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*environmentalSensingChange_t)(uint16_t connHandle, uint8_t paramID, uint16_t len, uint8_t *pValue);

typedef struct
{
  environmentalSensingChange_t        pfnChangeCb;  // Called when characteristic value changes
  environmentalSensingChange_t        pfnCfgChangeCb;
} environmentalSensingCBs_t;



/*********************************************************************
 * API FUNCTIONS
 */


/*
 * EnvironmentalSensing_AddService- Initializes the EnvironmentalSensing service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t EnvironmentalSensing_AddService( uint8_t rspTaskId);

/*
 * EnvironmentalSensing_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t EnvironmentalSensing_RegisterAppCBs( environmentalSensingCBs_t *appCallbacks );

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
extern bStatus_t EnvironmentalSensing_SetParameter(uint8_t param, uint16_t len, void *value);

/*
 * EnvironmentalSensing_GetParameter - Get a EnvironmentalSensing parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t EnvironmentalSensing_GetParameter(uint8_t param, uint16_t *len, void *value);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SERVICES_ENVIRONMENTALSENSINGSERVICE_H_ */
