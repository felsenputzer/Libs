/*
 * telegram.h
 *
 * Created: 25.10.2018 13:25:33
 *  Author: Johannes
 */ 


#ifndef TELEGRAM_H_
#define TELEGRAM_H_

#include "kennungen.h"
#include "uart_attiny87.h"

#define TELE_BUFFER_SIZE 4 

typedef enum{
	tele_handler_idle = 0,
	tele_handler_transmit,
	tele_handler_recieve,
	tele_handler_error,
	tele_hanlder_last
	}eTele_handler_state;

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
void tele_handler(eUart_event uart_event);

tele_fixed tele_send;
tele_fixed tele_recieve;

#endif /* TELEGRAM_H_ */