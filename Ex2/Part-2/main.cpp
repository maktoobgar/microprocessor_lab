#include <avr/eeprom.h>
#include <avr/io.h>
#include <util/delay.h>

#include <string.h>

#define F_CPU 1000000UL

int main()
{
    DDRB = 0xFF;

    while (true)
    {
        PORTB ^= 0x00;
        _delay_ms(500);
        PORTB ^= 0xFF;
        _delay_ms(500);
    }
}