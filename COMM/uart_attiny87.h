/*
 * uart.h
 *
 * Created: 24.10.2018 22:34:49
 *  Author: Johannes
 */ 


#ifndef UART_H_
#define UART_H_

#define BIT_SAMPLES 16
#define BAUD_RATE 38400
#define LDIV_VAL (F_CPU / (BIT_SAMPLES * BAUD_RATE)) - 1

#define UART_MODE_STDBY			0b00000100
#define UART_MODE_RX_BYTE		0b00000110
#define UART_MODE_TX_BYTE		0b00000101
#define UART_MODE_FULL_DUPLEX	0b00000111

#define PIN_RS485_RX PA0
#define PIN_RS485_TX PA1
#define PIN_RS485_RE PA2

#define RS485_CMD_TEST			0xA0

typedef enum{
	uart_event_tx_ready = 0,
	uart_event_rx_ready,
	uart_event_error
	}eUart_event;

uint8_t uart_notbussy(void);
void uart_set_recieve(void);
void uart_set_transmit(void);
void uart_init(void);
void uart_set_cmd(uint8_t mode);

#endif /* UART_H_ */