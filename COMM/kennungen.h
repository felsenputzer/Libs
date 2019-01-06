/*
 * Kennungen.h
 *
 * Created: 25.10.2018 13:25:33
 *  Author: Johannes
 */ 


#ifndef KENNUNGEN_H
#define KENNUNGEN_H

#define LEN_FIXED_TELE			0x0D

#define TEL_TYPE_FIXED_LEN		0x10
#define TEL_TYPE_VAR_LEN		0x20

#define TEL_DATA_REQ			0x10
#define TEL_DATA_RESP			0x20
#define TEL_HEARTBEAT_REQ		0x30
#define TEL_HEARTBEAT_RESP		0x40

#define GRP_ID_TEST				0xFE

#define DEVICE_ID_TASTER_TEST	0x01
#define DEVICE_ID_LED_TEST		0x02
#define DEVICE_ID_7SEG_TEST		0x03
#define DEVICE_ID_LAST_TEST		0x04
#define DEVICE_ID_SENS_TEST		0x05

#define BROADCAST_DEVICE_ID		0xFF
#define BROADCAST_GROUP_ID		0xFF

#endif /* KENNUNGEN_H */