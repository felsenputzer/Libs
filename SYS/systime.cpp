/*
 * systime.cpp
 *
 * Created: 20.09.2020 13:30:01
 *  Author: Johannes
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "systime.h"

#ifdef DEBUG
#include "Debugprint.h"
#endif


systime_handler systime;

void systime_init()
{
	switch(SYSCLK)
	{
		case sysclk_1ms:
			OCR0A = 250; 
			TCCR0B |= (1 << CS02); //Prescaler 64
			break;
		case sysclk_2ms:
			OCR0A = 250; 
			TCCR0B |= (1 << CS02) | (1<< CS00); //Prescaler 128
			break;
		case sysclk_5ms:
			OCR0A = 78; 
			TCCR0B |= (1 << CS02) | (1 << CS01) | (1 << CS00); //Prescaler 1024
			break;
		case sysclk_10ms:
			OCR0A = 156; 
			TCCR0B |= (1 << CS02) | (1 << CS01) | (1 << CS00); //Prescaler 1024
			break;
	}
	
	TIMSK0 |= (1 << OCIE0A);
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
	system_time += SYSCLK;
	while(head->event_time <= system_time)
	{
		*head->flag = true;
		pop_front();
	}
}

void systime_handler::attach(volatile bool *flag, uint16_t ms)
{
	uint32_t new_time = system_time + (uint32_t)ms;
	uint8_t array_pos = 0;
	
	while((array_pos < EVENT_STACK_SIZE) && (event_stack[array_pos].empty == false) )
	{
		array_pos++;
	}
	
	event_stack[array_pos].empty = false;
	event_stack[array_pos].flag = flag;
	event_stack[array_pos].event_time = new_time;
	event_stack[array_pos].next = 0;
	
	if (head == 0)
	{
		event_stack[array_pos].next = 0;
		head = &event_stack[array_pos];
	}
	else
	{
		systime_event *event_pos = head;
		systime_event *last_event = head;
		bool isHead = true;
		while(true)
		{
			if (new_time <= event_pos->event_time)
			{	
				event_stack[array_pos].next = event_pos;
				if (isHead)
				{
					head = &event_stack[array_pos];
					break;
				} 
				else
				{
					last_event->next = &event_stack[array_pos];
					break;
				}
			} 
			else if (event_pos->next == 0)
			{
				event_pos->next = &event_stack[array_pos];
				break;
			} 
			else
			{
				last_event = event_pos;
				event_pos = event_pos->next;
				isHead = false;
			}
		}
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
	#ifdef DEBUG
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
	#endif
	return;
}

ISR(TIMER0_COMPA_vect)
{
	cli();
	systime.update();
	sei();
	
	#ifdef DEBUG
	if (systime.get_time() % 1000 == 0)
	{
		//debugprint("Time:",systime.get_time()/1000);	
	}
	#endif	
	
	return;

}