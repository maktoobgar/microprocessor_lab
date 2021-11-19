#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>

int step = 0;
unsigned char steps[] = {0b0101, 0b0110, 0b1010, 0b1001};

const char nums[] PROGMEM = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

char *rotation_count = "XX";

void _interuption_initialization()
{
    sei();
}

void ADC_Init()
{
    DDRA = 0x00;
    ADCSRA = 0x87; // freq/128
    ADMUX = 0x40;  // ADC channel 0
}

int ADC_Read(char channel)
{
    ADMUX = 0x40 | (channel & 0x07);
    ADCSRA |= (1 << ADSC);
    while (!(ADCSRA & (1 << ADIF)))
        ;
    ADCSRA |= (1 << ADIF);
    _delay_ms(1);
    return ADCW;
}

void next_move()
{
    if (++step >= 4)
    {
        step = 0;
    }
    PORTB = steps[step];
}

void USART_receiving_initialization()
{
    // USART Receiving Initialization
    UBRRH = 0;
    UBRRL = 51;
    UCSRA = 0x00;
    UCSRB = 0x10;
    UCSRC = 0x86;
}

void USART_sending_initialization()
{
    // USART Sending Initialization
    UBRRH = 0x00;
    UBRRL = 51;
    UCSRA = 0x00;
    UCSRB = 0x08;
    UCSRC = 0x86;
}

int ignore = 0;

ISR(INT0_vect)
{
    USART_receiving_initialization();

    if (ignore < 1)
    {
        ignore++;
        USART_sending_initialization();
        return;
    }

    bool yekan_updated = false;
    bool dahgan_updated = false;
    while (!yekan_updated || !dahgan_updated)
    {
        PORTB = (PIND & 0x10) | (PIND & 0x20);
        while ((UCSRA & 0x80) == 0x00)
            ;
        // dahgan
        if ((PIND & 0x10) == 0x10)
        {
            PORTB = UDR;
            rotation_count[0] = UDR;
            dahgan_updated = true;
        }
        // yekan
        if ((PIND & 0x20) == 0x20)
        {
            PORTB = UDR;
            rotation_count[1] = UDR;
            yekan_updated = true;
        }
    }

    PORTB = 0x00;
    int count = atoi(rotation_count);

    for (int i = 0; i < count; i++)
    {
        next_move();
        _delay_ms(50);
    }

    USART_sending_initialization();
}

int main()
{
    _interuption_initialization();
    GICR = 1 << INT0;   /* Enable INT0*/
    MCUCR = 1 << ISC00; /* Trigger INT0 on rising and falling edge */

    DDRC = 0x03;
    DDRD = 0x00;
    DDRB = 0xFF;

    USART_sending_initialization();

    // ADC initialization
    ADC_Init();

    float celsius;

    while (true)
    {
        celsius = (ADC_Read(0) * 4.88); // Step size with Vref=5V:  5/1023 = 4.88 mV.
        celsius = (celsius / 10.00);

        int num = int(celsius);

        UDR = pgm_read_byte(&(nums[int(num / 10)]));

        PORTC = 0x01;

        while ((UCSRA & 0x20) == 0x00)
            ;

        UDR = pgm_read_byte(&(nums[int(num % 10)]));

        PORTC = 0x02;

        while ((UCSRA & 0x20) == 0x00)
            ;
    }
}