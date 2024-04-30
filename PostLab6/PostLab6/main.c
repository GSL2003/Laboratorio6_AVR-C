//******************************************************************************************************************
//UNIVERSIDAD DEL VALLE DE GUATEMALA
//IE2023: PROGRAMACI�N DE MICROCONTROLADORES
//Archivo: PreLab#5
//AUTOR: Guillermo Jos� Schwartz L�pez
//Laboratorio#6 - UART
//HARDWARE: ATMEGA328P
//CREADO: 23/04/2024
//�LTIMA MODIFICACI�N: 23/04/2024 - 14:52
//******************************************************************************************************************

//******************************************************************************************************************
//LIBRERIAS
//******************************************************************************************************************
#define F_CPU 16000000UL			//Frecuencia de 16MHZ
//-->UL significa que es un long y sin signo

#define  entrada0 48
#define  entrada1 49
#define  entrada2 50

//Definir librerias
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Incluir libreria del ADC
#include "ADC/ADC.h"

//Definir prototipo de funciones
void initUART9600(void);			//El microcontrolador solo tiene un modulo UART
void writeUART(char caractrer);		//Funci�n para escribir caracteres
void writeTextUART(char* texto);	//Automatizar mensajes -> Cadena de caracteres
void ASCII_DIG(uint8_t);			//Prototipo de funci�n de los digitos de la ASCII

//Declarar Variables
uint8_t ASCII = 0;

//Declarar variables volatiles
volatile char bufferRX;				//Se pine volatile a las variables que pueden cambiar en cualquier momento
volatile uint8_t input = 0;			//Se declara la variable para selecionar los datos
volatile uint8_t flag = 1;			//Se declara la bandera para el menu

int main(void)
{
	cli();				//Se  apagan las interupcciones Globales
	
	//LLamar a las funciones
    initUART9600();		//Se llama a la funci�n de UART
	ADC_init();
	
	//Configuraci�n de los pines para las salidas (LEDS)
	DDRB = 0xFF;
	PORTB = 0;
	
	DDRD = 0xFF;
	PORTD = 0;
	
	sei();				//Se activan las interupcciones Globales
	
	//Mandar caracteres al UART
	writeUART('H');
	writeUART('o');
	writeUART('l');
	writeUART('a');
	writeUART(' ');
	
	//Mandar cadena de caracteres al UART
	writeTextUART("\n\nMuchas gracias por usar el programa");
	
    while (1) 
    {
		//Configuraci�n menu de Inicio
		if(flag == 1){
			input = 0;
			//Mostrar Menu
			writeTextUART("\n\n-->�C�al de las siguientes opciones quiere realizar?\n");
			writeTextUART("[1]Leer el potenciometro\n");
			writeTextUART("[2]Enviar ASCII\n");
			writeTextUART("-->Selecione una opci�n: ");
			
			//El programa espera hasta que haya ingresado un valor
			while(!(input == entrada1 || input == entrada2));		//Se espera hasta que se cumpla la condici�n
			
			//Verifica que se haya cumplido cualquiera de las dos condiciones
			switch(input){
				
				case entrada1: //Se elige la primera Opci�n
				ADCSRA |= (1<<ADSC); //Inicia la Conversi�n ADC
				flag = 0;			//Set la bandera en 0
				_delay_ms(20); //Espera a que la operaci�n termine
				break;
				
				case entrada2: //Se elige la opci�n de la (ASCII) 
				writeTextUART("\n\n-->Ingrese ASCII: ");		//Ingresar el valor deL ASCII
				flag = 0;      //Set la bandera en 0
				while(flag == 0); //Espera por el input
				break;
			}
		}
		flag = 1;
    }
}

//Inicializar la funci�n del UART
void initUART9600(void){
	//Paso1: Configurar pines TX y RX
	DDRD &= ~(1<<DDD0);		//Se configura el RX (PD0) como entrada
	DDRD |= (1<<DDD1);		//Se configura el TX (PD1) como salida
	
	//Paso2: Configurar registro A ---> Modo FAST U2X0 = 1
	UCSR0A = 0;
	UCSR0A |= (1<<U2X0);
	
	//Paso3: Configurar registro B --> Habilitar ISR RX, se habilita RX y TX
	//->>Se configura para tener interrupciones
	UCSR0B = 0;		//Se pone en 0 para mayor facilidad de configurar posteriomente
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	
	//Paso4: Configurar C > Frame (Se define el frame): 8 bits datos, no paridad, 1 bit de stop
	UCSR0C = 0;    //--> Se configura si se desea modo: asincrono o sincrono
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);	//Se configura el tama�o del caracter --> 8 bits
	
	//Paso5: Baudrate = 9600
	UBRR0 = 207;
}

void writeUART(char caractrer){
	while(!(UCSR0A &(1<<UDRE0)));		//La funci�n se queda en espera hasta que UDR este en 1
	UDR0 = caractrer;
}

void writeTextUART(char* texto){			// "*" es un puntero del char
	uint8_t i;
	for(i = 0; texto[i]!= '\0'; i++){
		while(!(UCSR0A &(1<<UDRE0)));		//La funci�n se queda en espera hasta que UDR este en 1
		UDR0 = texto[i];
	}
}

//Configuraci�n de la ASCII - unidades, decenas y centenas
void ASCII_DIG(uint8_t digito){
	uint8_t unidades, decenas, centenas;
	
	//Usando los modulos como operadores, se puede configurar para tener las posiciones
	// -> La entrada se encuntras restingida entre el intervalo [0,255], entonces se puede realizar 3 veces
	unidades = digito % 10;
	digito = (digito - unidades)/10;
	decenas = digito % 10;
	digito = (digito - decenas)/10;
	centenas = digito % 10;

	writeUART(entrada0 + centenas);
	writeUART(entrada0 + decenas);
	writeUART(entrada0 + unidades);
	writeUART('\n');
	writeUART('\r');
}

//*************************************************************************************
//Configuraci�n de los ISR
//*************************************************************************************

/*
//Escribir desde la computador (mandar un eco)
ISR (USART_RX_vect){
	bufferRX = UDR0;
	
	while(!(UCSR0A &(1<<UDRE0)));		//La funci�n se queda en espera hasta que UDR este en 1
	UDR0 = bufferRX;
}
*/

//Configuraci�n despues de haber leido el menu
ISR (USART_RX_vect){
	bufferRX = UDR0;
	
	input = bufferRX;
	
	if(flag == 1){
		if(input == entrada1){
			writeTextUART("\n\nLeyendo el valor del potenciometro... ");
			}
			
		else if(input == entrada2){
			writeTextUART("\n\nLeyendo el valor de ASCII... ");
			}
			
		else{
			writeTextUART("\n\nEntrada inv�lida. Por favor ingrese 1 o 2.");
			writeTextUART("\n\n-->Ingrece nuevamente la opci�n: ");
			}
	}
	
	if(flag == 0){
		//Configuraci�n salidas del ASCII (LEDS)
		PORTD=(bufferRX<<6); 
		PORTB=(bufferRX>>2); 
		flag = 1;
	}
}

ISR(ADC_vect){
	ASCII = ADCH;
	ASCII_DIG(ASCII);
	PORTD=(ASCII<<6);
	PORTB=(ASCII>>2);
	ADCSRA |= (1<< ADIF);	//Limpiar la bandera al encender el ADC
}