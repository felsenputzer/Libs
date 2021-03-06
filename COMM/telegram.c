/*
 * telegram.c
 *
 * Created: 25.10.2018 13:27:10
 *  Author: Johannes
 */
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "telegram.h"
#include "../../tele_event_wrapper.h"

void tele_handler_init(void)
{
	memset(&tele_recieve, 0 ,LEN_FIXED_TELE);
	memset(&tele_send, 0 ,LEN_FIXED_TELE);
}

void build_telegram(tele_fixed *tele, uint8_t telegram_type, uint8_t destination_group, uint8_t destination_device, uint8_t payload_type)
{

	tele->sTele.group_ID = destination_group;
	tele->sTele.device_ID = destination_device;
	tele->sTele.telegram_type = telegram_type;
	tele->sTele.payload_type = payload_type;
	tele->sTele.origin_group_ID = my_device.group_ID;
	tele->sTele.origin_device_ID = my_device.device_ID;

	return;
}

void setmydevice(uint8_t group_ID, uint8_t device_ID)
{
	my_device.group_ID = group_ID;
	my_device.device_ID = device_ID;
}

void check_fixed_telegram(tele_fixed * tele)
{
	if ((tele->sTele.group_ID != my_device.group_ID) && (tele->sTele.group_ID != BROADCAST_GROUP_ID))
	{
		wrapper_check_event(eTele_check_event_group);
		return;
	}
	if ((tele->sTele.device_ID != my_device.device_ID) && (tele->sTele.device_ID != BROADCAST_DEVICE_ID))
	{
		wrapper_check_event(eTele_check_event_id);
		return;
	}
	if (tele->sTele.checksum != tele_checksum(tele))
	{
		wrapper_check_event(eTele_check_event_checksum);
		return;
	}

	if (wrapper_comm_event(tele->sTele.payload_type, tele, &tele_send))
	{
		send_tele(&tele_send);
	}
}

uint8_t tele_checksum(tele_fixed *tele)
{
	uint8_t checksum = 0;
	int i = 0;
	for(i = 0; i < LEN_FIXED_TELE-2; i++)
	{
		checksum ^= tele->bTele[i];
	}
	return checksum;
}

void send_tele(tele_fixed *tele)
{
	memcpy(&tele_send, tele, LEN_FIXED_TELE);
	tele_handler(uart_event_tx_ready);
}

void tele_handler(eUart_event uart_event)
{
	static eTele_handler_state state = tele_handler_idle;
	static uint8_t bytecounter = 0;

	switch(state)
	{
		case tele_handler_idle:
			bytecounter = 0;
			if (uart_event == uart_event_rx_ready)
			{
				memset(&tele_recieve, 0 ,LEN_FIXED_TELE);
				state = tele_handler_recieve;
				tele_recieve.bTele[bytecounter] = LINDAT;
			}
			if (uart_event == uart_event_tx_ready)
			{
				state = tele_handler_transmit;
				uart_set_transmit();
				LINDAT = tele_send.bTele[bytecounter];

			}
			bytecounter++;
			break;
		case tele_handler_recieve:
			if (uart_event == uart_event_rx_ready)
			{
				tele_recieve.bTele[bytecounter] = LINDAT;
				bytecounter++;
				if (bytecounter >= LEN_FIXED_TELE)
				{
					state = tele_handler_idle;
					check_fixed_telegram(&tele_recieve);
				}
			}
			else
			{
				state = tele_handler_error;
			}
			break;
		case tele_handler_transmit:
			if (uart_event == uart_event_tx_ready)
			{
				if (bytecounter <= LEN_FIXED_TELE - 1)
				{
					LINDAT = tele_send.bTele[bytecounter];
					bytecounter++;
				}
				else
				{
					state = tele_handler_idle;
					uart_set_recieve();
				}
				LINSIR |= (1 << LTXOK);
			}
			else
			{
				state = tele_handler_error;
			}
			break;
		case tele_handler_error:
			bytecounter = 0;
			memset(&tele_recieve, 0 ,LEN_FIXED_TELE);
			memset(&tele_send, 0 ,LEN_FIXED_TELE);
			state = tele_handler_idle;
			break;
		case tele_hanlder_last:
			state = tele_handler_error;
			break;
	}
}
