/*
 * ADC.c
 *
 * Created: 29/04/2024 19:14:02
 *  Author: Guillermo Schwartz
 */ 

#include "ADC.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void ADC_init(void)
{	
	
	ADMUX = 0;	
	//Seleccionar el voltaje de referencia
	ADMUX |= (1<<REFS0);
	ADMUX |= (0<<REFS1);
	ADMUX |= (1<<ADLAR);	// Se define que se trabajara con 10 bits
	ADCSRA = 0;
	
	// Configuración Prescaler de 128 --> 16M/128 = 125KHz
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
	ADCSRA |= (1<<ADEN); // Se enceinde el ADC y se activa su interruptor de igual forma
	ADCSRA |= (1<<ADIE);

}
