/*
 * distance.c
 *
 * Created: 2020/12/13 2:26:08
 * Author : Kazu-SK
 */ 




#define F_CPU 8000000UL


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define C 340290
#define F 8000000.0


void Initialize();
void SerialWrite(char data);


int main(void){
	
    /* Replace with your application code */
	uint16_t measure_distance;
	uint32_t measure_reset;
	uint16_t pulse_time;
	uint16_t i;
	char txd_data[5] = {'\0'};

	
	Initialize();
	
	measure_reset = 2.0 * 4000 * F / C; // max 4000mm
	
    while (1) 
    {
		measure_distance = 0;
		
		PORTD |= 1 << PORTD3;
		_delay_us(10);
		PORTD &= ~(1 << PORTD3);
		
		while(!(PIND & (1 << PIND4)));
		TCNT1 = 0;
		while(PIND & (1 << PIND4));
		
		pulse_time = TCNT1;
		
		if(pulse_time > measure_reset){
			SerialWrite('e');
			SerialWrite('\r');
			SerialWrite('\n');
			_delay_ms(500);
			continue;		
		}
		
		measure_distance = (pulse_time / F) * C / 2.0;
		
		sprintf(txd_data, "%d", measure_distance);
		
		for(i = 0 ; txd_data[i] != '\0' ; i++)
			SerialWrite(txd_data[i]);
			
		SerialWrite('m');
		SerialWrite('m');
		SerialWrite('\r');
		SerialWrite('\n');
		
		_delay_ms(500);
    }
}


void Initialize(){
	
	//PORTD
	DDRD = 1 << DDD3;
	
	//USART
	UBRR0 = 51;
	UCSR0B = 0b01101000;
	UCSR0C = 0b00000110;
	
	//TIMER
	TCCR1A = 0b00000000;
	TCCR1B = 0b00000001;
}

void SerialWrite(char data){

	while(!(UCSR0A & 0b00100000));
	UDR0 = data;
	while(!(UCSR0A & 0b01000000));	
}
