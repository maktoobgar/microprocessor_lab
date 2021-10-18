#include <avr/eeprom.h>
#include <avr/io.h>
#include <util/delay.h>

#include <string.h>

#define F_CPU 1000000UL

int main()
{
    DDRB = 0xFF;
    DDRA = 0x00;

    while (true)
    {
        if (PINA == 0x00)
        {
            PORTB = 0x01;
            _delay_ms(10);
            PORTB = 0x02;
            _delay_ms(10);
            PORTB = 0x04;
            _delay_ms(10);
            PORTB = 0x08;
            _delay_ms(10);
            PORTB = 0x10;
            _delay_ms(10);
            PORTB = 0x20;
            _delay_ms(10);
            PORTB = 0x40;
            _delay_ms(10);
            PORTB = 0x80;
            _delay_ms(10);
            PORTB = 0x80;
            _delay_ms(10);
            PORTB = 0x40;
            _delay_ms(10);
            PORTB = 0x20;
            _delay_ms(10);
            PORTB = 0x10;
            _delay_ms(10);
            PORTB = 0x08;
            _delay_ms(10);
            PORTB = 0x04;
            _delay_ms(10);
            PORTB = 0x02;
            _delay_ms(10);
            PORTB = 0x01;
            _delay_ms(10);
        }
        PORTB = 0;
    }
}