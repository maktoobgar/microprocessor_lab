#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>

const char nums[] PROGMEM = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};


void ADC_Init()
{
    DDRA = 0x00;
    ADCSRA = 0x87; // freq/128
    ADMUX = 0x40; // ADC channel 0
}

int ADC_Read(char channel)
{
    ADMUX = 0x40 | (channel & 0x07);
    ADCSRA |= (1 << ADSC);
    while (!(ADCSRA & (1 << ADIF)));
    ADCSRA |= (1 << ADIF);
    _delay_ms(1);
    return ADCW;
}

void command(unsigned char cmd)
{
    PORTC = 0x02;
    PORTD = cmd;
    PORTC = 0x00;
    _delay_ms(5);
}

void lcd_init()
{
    command(0x38);
    command(0x06);
    command(0x0E);
    command(0x01);
    command(0x80);
}

void lcd_data(unsigned char data)
{
    PORTC = 0x03;
    PORTD = data;
    PORTC = 0x01;
    _delay_ms(5);
}

void display(float celsius)
{
    int num = int(celsius);
    lcd_data(pgm_read_byte(&(nums[int(num / 10)])));
    lcd_data(pgm_read_byte(&(nums[int(num % 10)])));
}

int main()
{
    DDRC = 0xFF;
    DDRD = 0xFF;
    // Activate two lines in 8 bit mode
    command(0x38);
    // Display on, cursor blinking
    command(0x0f);
    // Clearing the display
    command(0x01);

    // ADC initialization
    ADC_Init();
    // LCD Initialization
    lcd_init();

    float celsius;

    while (true)
    {
        celsius = (ADC_Read(0)*4.88); // Step size with Vref=5V:  5/1023 = 4.88 mV. 
        celsius = (celsius / 10.00);

        // Cursor to first of first line
        command(0x80);
        // Clearing the display
        command(0x01);

        display(celsius);

        _delay_ms(500);
    }
}