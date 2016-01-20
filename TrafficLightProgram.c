/*
 * TrafficLightProgram.c
 *
 * Created: 1/11/2016 2:29:08 PM
 * Author : lamarts
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>


#define LCDCONTROL			PORTA
#define LCDDATA				PORTA
#define LCDDIR				DDRA

#define REGISTERSEL			PINA0
#define READWRITE			PINA1
#define ENABLE				PINA2


#define NS_GREEN_LITE		PINB0	// Definitions for outputs used to control lights - NS and EW
#define NS_YELLOW_LITE		PINB1
#define NS_RED_LITE			PINB2

#define EW_GREEN_LITE		PINB3
#define EW_YELLOW_LITE		PINB4
#define EW_RED_LITE			PINB5

#define PED_CROSSING_NS		PINC2	// Definitions for pedestrian crossing buttons - NS and EW
#define PED_CROSSING_EW		PINC1
#define ALL_TRAFFIC_STOP	PINC0	// Definitions for all traffic stop button

char time_left;						// time spent in each state
int current_state;					// current state of the lights
char toggle_flasher;				// used to toggle flasher

// Function prototypes
// LCD Control functions
void LCD_Init(void);
void LCD_SendCommand(unsigned char cmd);
void LCD_SendData(unsigned char data);
void LCD_SendString(char *string);


void state_machine(void);

enum{EW_RIGHT_OF_WAY, EW_WARNING, NS_RIGHT_OF_WAY, NS_WARNING, FLASHER};  // Way to add states to the machine by name

int main(void)
{
	DDRB = 0xff;		// Port B used for traffic lights for N-S and E-W Streets
	PORTB |= 0x00;
	DDRC = 0x00;
	LCD_Init();
	LCD_SendString("Sim Traffic Lite");
	LCD_SendCommand(0xC0);
	LCD_SendString("Demo w ATMEGA32");
	
	
	current_state = NS_RIGHT_OF_WAY;
    while (1) 
    {
		_delay_ms(1000);
		state_machine();
    }
}


void state_machine()
{
	switch(current_state)
	{
		case EW_RIGHT_OF_WAY:								// East-West has the green light
		PORTB = (1<<EW_GREEN_LITE)|(1<<NS_RED_LITE);	// North-South has the red light
		
		if((PINC & (1<<PED_CROSSING_EW) || (1<<ALL_TRAFFIC_STOP))==1)
		{
			if(time_left > 10)
			time_left = 10;
		}
		
		if(time_left != 0)
		{
			--time_left;
			return;
		}
		time_left = 5;
		current_state = EW_WARNING;
		break;
		
		case EW_WARNING:
		PORTB = (1<<EW_YELLOW_LITE)|(1<<NS_RED_LITE);
		if(time_left != 0)
		{
			--time_left;
			return;
		}
		// if((PORTC & (1<<ALL_TRAFFIC_STOP))==1)
		if((PINC & (1<<ALL_TRAFFIC_STOP))==1)
		current_state = FLASHER;
		else
		{
			time_left = 60;
			current_state = NS_RIGHT_OF_WAY;
		}
		break;
		case NS_RIGHT_OF_WAY:
		PORTB = (1<<NS_GREEN_LITE)|(1<<EW_RED_LITE);
		
		if((PINC & (1<<PED_CROSSING_NS) || (1<<ALL_TRAFFIC_STOP))==1)  // if pedestrian wishes to cross or 4-way stop is required
		{
			if(time_left > 10)
			time_left = 10;					// shorten the time
		}
		if(time_left != 0)
		{
			--time_left;						// count down the time
			return;
		}
		time_left = 5;							// five seconds to WARNING
		current_state = NS_WARNING;
		break;
		
		case NS_WARNING:
		PORTB = (1<<EW_RED_LITE)|(1<<NS_YELLOW_LITE);
		
		if(time_left != 0)
		{
			--time_left;				// count down
			return;
		}
		if((PINC & (1<<ALL_TRAFFIC_STOP))==1)
		{
			current_state = FLASHER;
		}

		else
		{
			time_left = 30;			// give 30 seconds to moving
			current_state = EW_RIGHT_OF_WAY;
		}
		break;
		case FLASHER:					// All green and yellow lights are off
		PORTB = 0x00;
		toggle_flasher ^= 1;
		if(toggle_flasher & 1)
		{
			PORTB |=  (1<<NS_RED_LITE);
			PORTB &= ~(1<<EW_RED_LITE);
		}
		else
		{
			PORTB &= ~(1<<NS_RED_LITE);
			PORTB |=  (1<<EW_RED_LITE);
		}
		if((PINC & (1<<ALL_TRAFFIC_STOP))==1)
		current_state = EW_WARNING;
		break;
		default:
		current_state = NS_WARNING;
		break;
	}
}


void LCD_Init(void)
{
	LCDDIR	= 0xff;
	LCD_SendCommand(0x02);
	_delay_us(50);
	LCD_SendCommand(0x28);
	_delay_us(50);
	LCD_SendCommand(0x0C);
	_delay_us(50);
	LCD_SendCommand(0x01);
	_delay_ms(2);
	LCD_SendCommand(0x80);
}
void LCD_SendCommand(unsigned char cmd)
{
	LCDDATA = (cmd & 0xF0);
	LCDCONTROL &= ~(1<<REGISTERSEL);
	LCDCONTROL &= ~(1<<READWRITE);
	LCDCONTROL |= 1<<ENABLE;
	_delay_us(10);
	LCDCONTROL &= ~(1<<ENABLE);
	
	LCDDATA = ((cmd << 4) & 0xF0);
	LCDCONTROL &= ~(1<<REGISTERSEL);
	LCDCONTROL &= ~(1<<READWRITE);
	LCDCONTROL |= 1<<ENABLE;
	_delay_us(10);
	LCDCONTROL &= ~(1<<ENABLE);
	_delay_ms(10);
}
void LCD_SendData(unsigned char data)
{
	LCDDATA = (data & 0xF0);
	LCDCONTROL |= (1<<REGISTERSEL);
	LCDCONTROL &= ~(1<<READWRITE);
	LCDCONTROL |= 1<<ENABLE;
	_delay_us(10);
	LCDCONTROL &= ~(1<<ENABLE);
	
	LCDDATA = ((data << 4) & 0xF0);
	LCDCONTROL |= (1<<REGISTERSEL);
	LCDCONTROL &= ~(1<<READWRITE);
	LCDCONTROL |= 1<<ENABLE;
	_delay_us(10);
	LCDCONTROL &= ~(1<<ENABLE);
	_delay_ms(10);
}
void LCD_SendString(char *string)
{
	while(*string)
	LCD_SendData(*string++);
}
