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
        for (int j = 0; j < 8; j++)
        {
            PORTD = (PORTD << 1) | (PINA & 1);
            _delay_ms(100);
        }
        PORTD = 0;
    }
}