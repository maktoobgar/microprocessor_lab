#include <avr/eeprom.h>
#include <avr/io.h>
#include <util/delay.h>

#include <string.h>

int main()
{
    DDRA = 0x00;
    DDRD = 0xFF;
    while (true)
    {
        PORTD = PINA;
    }
}