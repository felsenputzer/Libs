/*
 * telegram.c
 *
 * Created: 25.10.2018 13:27:10
 *  Author: Johannes
 */
#include <stdio.h> 
#include <string.h>
#include <COMM/telegram.h>


tele_fixed build_telegram(uint8_t telegram_type, uint8_t destination_group, uint8_t destination_device, uint8_t payload_type)
{
	tele_fixed tele;
	
	tele.sTele.group_ID = destination_group;
	tele.sTele.device_ID = destination_device;
	tele.sTele.telegram_type = telegram_type;
	tele.sTele.payload_type = payload_type;
	
	return tele;
	
}

void tele_init_buffer(void)
{
	tele_recieve_buffer.active = 0;
	tele_recieve_buffer.tele_num = 0;
	tele_recieve_buffer.next_tele = &tele_recieve_buffer.u.tTele[tele_recieve_buffer.tele_num];
	tele_recieve_buffer.tele_pos = &tele_recieve_buffer.u.tTele[tele_recieve_buffer.tele_num];
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