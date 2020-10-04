/*
 * ADC.h
 *
 * Created: 24.09.2020 22:35:36
 *  Author: Johannes
 */ 


#ifndef ADC_H_
#define ADC_H_

#define ADC_CHANNELS 5
#define ADC_BUFFER_SIZE 16

void ADC_hwinit(void);

typedef enum{
	eADC_speed_div1,
	eADC_speed_div2,
	eADC_speed_div4,
	eADC_speed_div8,
	eADC_speed_div16,
	eADC_speed_div32,
	eADC_speed_div64,
	eADC_speed_div128
	}eADC_speed;

class cADC
{
public:
	cADC();
	uint16_t get_last_result(uint8_t ADCPin);
	uint16_t get_avrg_result(uint8_t ADCPin);
	uint16_t get_high_res_result(uint8_t ADCPin);
	uint16_t get_min_result(uint8_t ADCPin);
	uint16_t get_max_result(uint8_t ADCPin);
	void start_single_measurement(uint8_t ADCPin);
	void start_complete_measurement(uint8_t ADCPin);
	void callback_ADC_ISR(void);
	bool bussy(void);

protected:

private:	
	void set_channel(uint8_t channel);
	void set_speed(eADC_speed speed);
	void start_measurement(void);
	uint8_t remaining_measurements;
	uint8_t active_channel;
	uint8_t buffer_pos[ADC_CHANNELS];
	uint16_t results[ADC_CHANNELS][ADC_BUFFER_SIZE];
	bool buffer_full;
};

extern cADC myADC;

#endif /* ADC_H_ */