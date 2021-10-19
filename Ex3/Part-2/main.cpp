#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 10000UL

int num_to_7seg(int num)
{
    switch (num)
    {
    case 0:
        return 0x3F;
    case 1:
        return 0x06;
    case 2:
        return 0x5B;
    case 3:
        return 0x4F;
    case 4:
        return 0x66;
    case 5:
        return 0x6D;
    case 6:
        return 0x7D;
    case 7:
        return 0x07;
    case 8:
        return 0x7F;
    case 9:
        return 0xEF;
    }
    return 0;
}

int main()
{
    DDRA = 0xFF;
    DDRB = 0xFF;
    DDRC = 0xFF;
    DDRD = 0xFF;

    PORTA = num_to_7seg(0);
    PORTB = num_to_7seg(0);
    PORTC = num_to_7seg(0);
    PORTD = num_to_7seg(0);
    int i = 0;
    int j = 1;

    while (true)
    {
        if (i > 9999)
        {
            j = -1;
            i = i + j;
        }
        else if (i < 0)
        {
            j = 1;
            i = i + j;
        }
        PORTA = num_to_7seg(i % 10);
        PORTB = num_to_7seg((i / 10) % 10);
        PORTC = num_to_7seg((i / 100) % 10);
        PORTD = num_to_7seg(i / 1000);
        _delay_ms(1);
        i = i + j;
    }
}