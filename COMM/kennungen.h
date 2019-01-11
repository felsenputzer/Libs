/*
 * Kennungen.h
 *
 * Created: 25.10.2018 13:25:33
 *  Author: Johannes
 */


#ifndef KENNUNGEN_H
#define KENNUNGEN_H

#define LEN_FIXED_TELE			0x0F

#define TEL_TYPE_FIXED_LEN		0x10
#define TEL_TYPE_VAR_LEN		0x20

typedef enum
{
	TEL_HEARTBEAT_REQ		=0,
	TEL_HEARTBEAT_RESP,
	TEL_DATA_REQ,
	TEL_DATA_RESP,
	TEL_SET_OUTPUT,
	TEL_ACK,

	TEL_TELE_LAST
}eTeleType;

typedef enum
{
	eTestAction_None = 0,
	eTestAction_Set_LED,
	eTestAction_Set_7SEG,
	eTestAction_Set_LoadCurrent,
	eTestAction_Last
}eTestAction;

typedef enum
{
		eData_Temp_CH1 = 0,
		eData_Temp_CH2,
		eData_Temp_CH3,
		eData_Temp_CH4,
		eData_Temp_CH5,
		eData_Temp_CH6,
		eData_Humidity,
		eData_Pressure,
		eData_Voltage,
		eData_Current,
		eData_Moisture
}eData;

#define GRP_ID_TEST				0xFE

#define DEVICE_ID_TASTER_TEST	0x01
#define DEVICE_ID_LED_TEST		0x02
#define DEVICE_ID_7SEG_TEST		0x03
#define DEVICE_ID_LAST_TEST		0x04
#define DEVICE_ID_SENS_TEST		0x05

#define BROADCAST_DEVICE_ID		0xFF
#define BROADCAST_GROUP_ID		0xFF

#endif /* KENNUNGEN_H */
