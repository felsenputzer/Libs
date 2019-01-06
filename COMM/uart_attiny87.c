/*
 * uart.c
 *
 * Created: 24.10.2018 22:35:05
 *  Author: Johannes
 */ 
#include <avr/io.h>
#include "uart_attiny87.h"

#ifndef F_CPU
#define F_CPU 16000000UL

#endif

void uart_init(void)
{
	LINCR |= (1 << LSWRES);
	while ((LINCR & (1 << LSWRES)) != 0)
	{
	}
	
	uint16_t ldiv_value = (F_CPU / (BIT_SAMPLES * BAUD_RATE)) - 1;
	LINCR |= (1 << LENA);		//Enable UART Controller
	
	LINBRRH |= (uint8_t) (ldiv_value >> 8);
	LINBRRL |= (uint8_t) (ldiv_value);
	
	LINBTR |= (BIT_SAMPLES & 0x3F);
	LINBTR |= (1 << LDISR);

	LINENIR |= (1 <<LENTXOK);
	LINENIR |= (1 <<LENRXOK);
	
	LINCR |= (1 << LCONF0);		//Set to 8bit, odd parity, 1 stopbit
	
	uart_set_recieve();
	
}

uint8_t uart_notbussy()
{
	if ((LINSIR & (1 << LBUSY)) != 0)
	{
		return 1;
	} 
	else
	{
		return 0;
	}
}

void uart_set_recieve(void)
{
	uart_set_cmd(UART_MODE_STDBY);
	PORTA &= ~(1 << PIN_RS485_RE);
	uart_set_cmd(UART_MODE_RX_BYTE);
}

void uart_set_transmit(void)
{
	uart_set_cmd(UART_MODE_STDBY);
	PORTA |= (1 << PIN_RS485_RE);
	uart_set_cmd(UART_MODE_TX_BYTE);
}

void uart_set_cmd(uint8_t mode)
{
	LINCR = (LINCR & 0xF8) | (mode & 0x07);
}