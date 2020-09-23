/*
 * systime.h
 *
 * Created: 20.09.2020 13:30:14
 *  Author: Johannes
 */ 


#ifndef SYSTIME_H_
#define SYSTIME_H_

typedef struct systime_event{
	bool *flag;
	uint32_t event_time;
	systime_event *next;	
	};

void systime_init();
void systime_attach(bool *flag, uint16_t ms);

class systime_handler
{
public:
	systime_handler();
	void update(void);
	void attach(bool *flag, uint16_t ms);
	uint32_t get_time(void);
	void pop_front(void);
	void d_Printlist(void);
protected:
private:
	systime_event *head;
	uint32_t system_time;
};

extern systime_handler systime;

#endif /* SYSTIME_H_ */