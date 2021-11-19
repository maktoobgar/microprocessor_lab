#include <util/delay.h>

bool key_pressed()
{
    // PORTA = 0xFF;
    if ((PINB & 0x0F) != 0x00)
    {
        return true;
    }
    return false;
}

bool key_released()
{
    // PORTA = (PINC & 0x0F);
    if ((PINB & 0x0F) == 0x00)
    {
        return true;
    }
    return false;
}

int which_row()
{
    if ((PINB & 0x0F) == 0x01)
    {
        return 3;
    }
    else if ((PINB & 0x0F) == 0x02)
    {
        return 2;
    }
    else if ((PINB & 0x0F) == 0x04)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int which_col()
{
    // PC4 off
    PORTB = 0x60;
    if ((PINB & 0x0F) == 0x00)
    {
        PORTB = 0x70;
        return 2;
    }
    // PC5 off
    PORTB = 0x50;
    if ((PINB & 0x0F) == 0x00)
    {
        PORTB = 0x70;
        return 1;
    }
    // PC6 off
    PORTB = 0x30;
    if ((PINB & 0x0F) == 0x00)
    {
        PORTB = 0x70;
        return 0;
    }
    return 0;
}