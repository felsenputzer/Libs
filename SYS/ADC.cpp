/*
 * ADC.cpp
 *
 * Created: 24.09.2020 22:35:23
 *  Author: Johannes
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC.h"
#include "systime.h"

#ifdef DEBUG
#include "Debugprint.h"
#endif

cADC myADC;

uint8_t channel[9];


void ADC_hwinit(void)
{
	ADCSRA |= (1 << ADEN); // Enable ADC
	DIDR0 |= (1 << ADC3D) | (1 << ADC4D) | (1 << ADC5D) | (1 << ADC6D) | (1 << ADC7D);
	ADCSRA |= (1 << ADIE); // Interrupt Enable
	
	channel[PA3] = 0;
	channel[PA4] = 1;
	channel[PA5] = 2;
	channel[PA6] = 3;
	channel[PA7] = 4;
}

 cADC::cADC()
{
	for (int i = 0; i < ADC_CHANNELS; i++)
	{
		results[i] = 0;
	}
	remaining_measurements = 0;
	set_speed(eADC_speed_div128);
}

uint16_t cADC::get_result(uint8_t ADCPin)
{
	return results[channel[ADCPin]];
}

void cADC::start_complete_measurement(uint8_t ADCPin)
{
	helper = 0;
	active_channel = channel[ADCPin];
	remaining_measurements = ADC_BUFFER_SIZE;
	set_channel(ADCPin);
	start_measurement();
	return;
}

void cADC::callback_ADC_ISR(void)
{
	uint16_t result;
	result = 0x00FF & (uint16_t)ADCL;
	result |= (uint16_t)(ADCH << 8);
	helper += result;
	remaining_measurements--;
	if (remaining_measurements != 0)
	{
		start_measurement();
	}
	else
	{
		results[active_channel] = (helper >> 2);
	}
	return;
}

bool cADC::bussy(void)
{
	if (remaining_measurements == 0)
	{
		return false;
	} 
	else
	{
		return true;
	}
}

void cADC::set_channel(uint8_t channel)
{
	switch(channel)
	{
		case PA0:
			ADMUX = (0x1F & 0b00000);
			break;
		case PA1:
			ADMUX = (0x1F & 0b00001);
			break;		
		case PA2:
			ADMUX = (0x1F & 0b00010);
			break;
		case PA3:
			ADMUX = (0x1F & 0b00011);
			break;	
		case PA4:
			ADMUX = (0x1F & 0b00100);
			break;
		case PA5:
			ADMUX = (0x1F & 0b00101);
			break;
		case PA6:
			ADMUX = (0x1F & 0b00110);
			break;
		case PA7:
			ADMUX = (0x1F & 0b00111);
			break;
		/*case PB5:		-- FIX duplicated cases by defines
			ADMUX = (0x1F & 0b01000);
			break;
		case PB6:
			ADMUX = (0x1F & 0b01001);
			break;		
		case PB7:
			ADMUX = (0x1F & 0b01010);
			break;
		/*case Temp:	-- FIX missing defines
			ADMUX = (0x1F & 0b01011);
			break;		
		case Bandgap:
			ADMUX = (0x1F & 0b01100);
			break;
		case AVCC_4:
			ADMUX = (0x1F & 0b01101);
			break;*/
	}
	return;
}

void cADC::set_speed(eADC_speed speed)
{
	ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2); //Prescale clock 128
	return;
	switch(speed)
	{
		case eADC_speed_div1:
			ADCSRA = 0;
			break;
		case eADC_speed_div2:
			ADCSRA |= (1 << ADPS0);
			break;
		case eADC_speed_div4:
			ADCSRA |=(1 << ADPS1);
			break;
		case eADC_speed_div8:
			ADCSRA |= (1 << ADPS0) | (1 << ADPS1);
			break;
		case eADC_speed_div16:
			ADCSRA |= (1 << ADPS2);
			break;
		case eADC_speed_div32:
			ADCSRA |= (1 << ADPS0) | (1 << ADPS2);
			break;
		case eADC_speed_div64:
			ADCSRA |= (1 << ADPS1) | (1 << ADPS2);
			break;
		case eADC_speed_div128:
			ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
			break;
	}
	return;
}

void cADC::start_measurement(void)
{
	ADCSRA |= (1 << ADSC);
	return;
}
