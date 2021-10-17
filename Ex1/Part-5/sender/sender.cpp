#include <avr/eeprom.h>
#include <avr/io.h>
#include <util/delay.h>

#include <string.h>

int main()
{
    char text[200] = "0720722777520536173213654769096733766844777841002558911403386565991865840856747054037476910903152738358335042791084577978553624263142980235285530008615236690398366486116795202727516656903193975365411";
    unsigned int i = -8;

    DDRC = 0xFF;
    PORTC = 0;

    eeprom_busy_wait();
    eeprom_write_block(text, 0, strlen(text));

    while (true)
    {
        if (i >= strlen(text))
            i = 0;
        for (int j = 0; j < 8; j++)
        {
            PORTC = (text[i] >> j);
            _delay_ms(100);
        }
        i = i + 1;
    }
}