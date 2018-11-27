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

device mydevice;

tele_fixed build_telegram(uint8_t telegram_type, uint8_t destination_group, uint8_t destination_device, uint8_t payload_type)
{
	tele_fixed tele;
	
	tele.sTele.group_ID = destination_group;
	tele.sTele.device_ID = destination_device;
	tele.sTele.telegram_type = telegram_type;
	tele.sTele.payload_type = payload_type;
	
	return tele;
	
}

void check_fixed_telegram(tele_fixed * tele, device * my_device)
{
	tele_fixed tele_out;
	 if ((tele->sTele.group_ID != my_device->group_ID) && (tele->sTele.group_ID != BROADCAST_GROUP_ID))
	 {
		 return;
	 }
	 
	 if ((tele->sTele.device_ID != my_device->device_ID) && (tele->sTele.device_ID != BROADCAST_DEVICE_ID))
	 {
		 return;
	 }

	if (tele->sTele.checksum != tele_checksum(tele))
	{
		return;
	}
	
	switch(tele->sTele.payload_type)
	{
		case TEL_DATA_REQ:
			break;
		case TEL_DATA_RESP:
			break;
		case TEL_HEARTBEAT_REQ:
			tele_out = build_telegram(TEL_TYPE_FIXED_LEN, 0, 0, TEL_HEARTBEAT_RESP);
			tele_out.sTele.payload[0] = my_device->group_ID;
			tele_out.sTele.payload[1] = my_device->device_ID;
			tele_out.sTele.checksum = tele_checksum(&tele_out);
			send_tele(&tele_out);
			break;
		case TEL_HEARTBEAT_RESP:
			break;
	}
}

uint8_t tele_checksum(tele_fixed *tele)
{
	uint8_t checksum = 0;
	int i = 0;
	for(i = 0; i < TEL_TYPE_FIXED_LEN-2; i++)
	{
		checksum ^= tele->bTele[i];
	}
	return checksum;
}

void send_tele(tele_fixed *tele)
{
	memcpy(&tele_send, tele, TEL_TYPE_FIXED_LEN);

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
				if (bytecounter >= TEL_TYPE_FIXED_LEN)
				{
					state = tele_handler_idle;
					check_fixed_telegram(&tele_recieve, &mydevice);
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
				if (bytecounter < TEL_TYPE_FIXED_LEN)
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
			memset(&tele_recieve, 0 ,TEL_TYPE_FIXED_LEN);
			state = tele_handler_idle;
			break;
		case tele_hanlder_last:
			state = tele_handler_error;
			break;
	}
}