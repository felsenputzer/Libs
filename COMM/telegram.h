/*
 * telegram.h
 *
 * Created: 25.10.2018 13:25:33
 *  Author: Johannes
 */ 


#ifndef TELEGRAM_H_
#define TELEGRAM_H_

#include <COMM/kennungen.h>

#define TELE_BUFFER_SIZE 4 

typedef union{
	struct{
		uint8_t group_ID;
		uint8_t device_ID;
		uint8_t telegram_type;
		uint8_t payload_type;
		uint8_t payload[8];
		uint8_t checksum;
	}sTele;
	uint8_t bTele[LEN_FIXED_TELE];
} tele_fixed;

typedef struct  
{
	union{
		tele_fixed tTele[4];
		uint8_t u8Tele[TELE_BUFFER_SIZE * LEN_FIXED_TELE];
	}u;
	uint8_t *next_tele;
	uint8_t *tele_pos;
	uint8_t active;
	uint8_t tele_num;
}tele_buf;

typedef struct{
	uint8_t group_ID;
	uint8_t device_ID;
	}device;

tele_fixed build_telegram(uint8_t telegram_type, uint8_t destination_group, uint8_t destination_device, uint8_t payload_type);
void check_telegram(tele_fixed * tele, device * my_device);
uint8_t tele_checksum(tele_fixed *tele);
void send_tele(tele_fixed *tele);
void tele_init_buffer(void);

tele_fixed tele_send;
tele_buf tele_recieve_buffer;

#endif /* TELEGRAM_H_ */