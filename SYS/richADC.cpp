/*
 * ADC.cpp
 *
 * Created: 24.09.2020 22:35:23
 *  Author: Johannes
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "richADC.h"
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
	for (int i; i < ADC_CHANNELS; i++)
	{
		for (int j; j < ADC_BUFFER_SIZE; j++)
		{
			results[i][j] = 0;
		}
		buffer_pos[i] = 0;
	}
	remaining_measurements = 0;
	buffer_full = false;
	set_speed(eADC_speed_div128);
}

uint16_t cADC::get_last_result(uint8_t ADCPin)
{
	return results[channel[ADCPin]][buffer_pos[channel[ADCPin]]];
}

uint16_t cADC::get_avrg_result(uint8_t ADCPin)
{
	uint16_t result = 0;
	for (int i = 0; i < ADC_BUFFER_SIZE; i++)
	{
		result += results[channel[ADCPin]][i];
	}
	return (result/ADC_BUFFER_SIZE);
}

uint16_t cADC::get_high_res_result(uint8_t ADCPin)
{
	uint16_t result = 0;
	for (int i = 0; i < ADC_BUFFER_SIZE; i++)
	{
		result += results[channel[ADCPin]][i];
	}
	return (result >> 2);
}

uint16_t cADC::get_min_result(uint8_t ADCPin)
{
	uint16_t result = 0xFFFF;
	for (int i = 0; i < ADC_BUFFER_SIZE; i++)
	{
		if (result > results[channel[ADCPin]][i]) result = results[channel[ADCPin]][i];
	}
	return result;
}

uint16_t cADC::get_max_result(uint8_t ADCPin)
{
	uint16_t result = 0x0000;
	for (int i = 0; i < ADC_BUFFER_SIZE; i++)
	{
		if (result < results[channel[ADCPin]][i]) result = results[channel[ADCPin]][i];
	}
	return result;
}

void cADC::start_single_measurement(uint8_t ADCPin)
{
	active_channel = channel[ADCPin];
	remaining_measurements = 1;
	set_channel(ADCPin);
	start_measurement();
	return;
}

void cADC::start_complete_measurement(uint8_t ADCPin)
{
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
	results[active_channel][buffer_pos[active_channel]] = result;
	remaining_measurements--;
	if(buffer_pos[active_channel] == ADC_BUFFER_SIZE - 1)
	 {
		 buffer_pos[active_channel] = 0;
	 }
	 else
	 {
		 buffer_pos[active_channel]++;
	 }
	if (remaining_measurements != 0)
	{
		start_measurement();
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
