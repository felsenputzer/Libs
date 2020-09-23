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

/*void systime_attach(bool *flag, uint16_t ms)
{
	systime_event *event_pos = first_event;
	uint32_t real_time = time_ms + (uint32_t)ms;
	
	systime_event new_event;
	new_event.flag = flag;
	new_event.event_time = real_time;
	if (first_event->next == 0)
	{
		new_event.next = 0;
		first_event->next = &new_event;
	} 
	else
	{
		while((real_time <= event_pos->next->event_time) && (event_pos->next != 0))
		{
			event_pos = event_pos->next;
		}
		new_event.next = event_pos->next;
		*(event_pos->next) = new_event;	
	}
	return;
}*/

 systime_handler::systime_handler()
{
	system_time = 0;
	head->next = 0;
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

void systime_handler::attach(bool *flag, uint16_t ms)
{
	uint32_t real_time = system_time + (uint32_t)ms;
	systime_event *new_event;
	new_event->flag = flag;
	new_event->event_time = real_time;
	systime_event *event_pos = head;
	
	if (event_pos->next == 0)
	{
		new_event->next = 0;
		head->next = new_event;
	}
	else
	{
		while((real_time >= event_pos->event_time) && (event_pos->next != 0))
		{
			event_pos = event_pos->next;
		}
		new_event->next = event_pos->next;
		event_pos->next = new_event;
	}
	return;
}

uint32_t systime_handler::get_time(void)
{
	return system_time;
}

void systime_handler::pop_front(void)
{
	head->next = head->next->next;
}

void systime_handler::d_Printlist(void)
{
	uint8_t count = 1;
	systime_event *event_pos = head;
	do 
	{
		count++;
		debugprint("Ev:", count);
		debugprint("time:", event_pos->event_time);
		event_pos = event_pos->next;
	} while (event_pos->next != 0);
}

ISR(TIMER0_COMPA_vect)
{
	cli();
	systime.update();
	/*if (first_event->next != 0)
	{
		if (time_ms >= first_event->next->event_time)
		{
			*first_event->next->flag = true;
			if (first_event->next->next == 0)
			{
				first_event->next = 0;
			}
			else
			{
				first_event->next = first_event->next->next;
			}
		}
	}*/
	sei();
	if (systime.get_time() % 1000 == 0)
	{
		debugprint("Time:",systime.get_time()/1000);	
	}
	return;
}