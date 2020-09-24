/*
 * systime.cpp
 *
 * Created: 20.09.2020 13:30:01
 *  Author: Johannes
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "systime.h"
#include "Debugprint.h"

systime_handler systime;

void systime_init()
{
	OCR0A = 250; //Should give 1ms time
	TIMSK0 |= (1 << OCIE0A);
	TCCR0B |= (1 << CS02); //Prescaler 32
	TCCR0A |= (1 << WGM01); //Set CTC Mode
	
	return;
}

 systime_handler::systime_handler()
{
	system_time = 0;
	for (int i = 0; i < EVENT_STACK_SIZE; i++)
	{
		event_stack[i].empty = true;
		event_stack[i].event_time = 0;
		event_stack[i].next = 0;
	}
	head = 0;
}

void systime_handler::update(void)
{
	system_time++;
	if (head->event_time <= system_time)
	{
		*head->flag = true;
		pop_front();
	}
}

void systime_handler::attach(volatile bool *flag, uint16_t ms)
{
	uint32_t real_time = system_time + (uint32_t)ms;
	uint8_t array_pos = 0;
	
	while((array_pos < EVENT_STACK_SIZE) && (event_stack[array_pos].empty == false) )
	{
		array_pos++;
	}
	
	event_stack[array_pos].empty = false;
	event_stack[array_pos].flag = flag;
	event_stack[array_pos].event_time = real_time;
	
	if (head == 0)
	{
		event_stack[array_pos].next = 0;
		head = &event_stack[array_pos];
	}
	else
	{
		systime_event *event_pos = head;
		while((real_time >= event_pos->event_time) && (event_pos->next != 0))
		{
			event_pos = event_pos->next;
		}
		event_stack[array_pos].next = event_pos->next;
		event_pos->next = &event_stack[array_pos];
	}
	

	return;
}

uint32_t systime_handler::get_time(void)
{
	return system_time;
}

void systime_handler::pop_front(void)
{
	head->empty = true;
	head = head->next;
}

void systime_handler::d_Printlist(void)
{
	uint8_t count = 1;
	systime_event *event_pos = head;
	debugprint("SysT:", systime.get_time());
	while (event_pos->next != 0) 
	{
		event_pos = event_pos->next;
		debugprint("Ev:", count);
		debugprint("time:", event_pos->event_time);
		count++;
	}
}

ISR(TIMER0_COMPA_vect)
{
	cli();
	systime.update();
	sei();
	if (systime.get_time() % 1000 == 0)
	{
		debugprint("Time:",systime.get_time()/1000);	
	}
	return;
}