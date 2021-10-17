#include <avr/eeprom.h>
#include <avr/io.h>
#include <util/delay.h>

#include <string.h>

#define F_CPU 1000000UL

int main()
{
    DDRA = 1;

    while (true)
    {
        PORTA ^= 1;
        _delay_ms(500);
    }
}