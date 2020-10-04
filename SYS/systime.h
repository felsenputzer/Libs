/*
 * systime.h
 *
 * Created: 20.09.2020 13:30:14
 *  Author: Johannes
 */ 


#ifndef SYSTIME_H_
#define SYSTIME_H_

#define EVENT_STACK_SIZE 8
#define SYSCLK sysclk_1ms

typedef struct systime_event{
	bool empty;
	volatile bool *flag;
	uint32_t event_time;
	systime_event *next;	
	};

void systime_init();

typedef enum{
	sysclk_1ms = 1,
	sysclk_2ms = 2,
	sysclk_5ms = 5,
	sysclk_10ms = 10
	}sysclk;

class systime_handler
{
public:
	systime_handler();
	void update(void);
	void attach(volatile bool *flag, uint16_t ms);
	uint32_t get_time(void);
	void d_Printlist(void);
protected:
private:
	void pop_front(void);
	systime_event *head;
	systime_event event_stack[EVENT_STACK_SIZE];
	uint32_t system_time;
};

extern systime_handler systime;

#endif /* SYSTIME_H_ */