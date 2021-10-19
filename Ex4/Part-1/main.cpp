#include <avr/io.h>
#include <util/delay.h>

#include <string.h>

#define F_CPU 16000000UL

void command(unsigned char cmd)
{
    PORTC = 0x02;
    PORTD = cmd;
    PORTC = 0x00;
    _delay_ms(15);
}

void lcd_data(unsigned char data)
{
    PORTC = 0x03;
    PORTD = data;
    PORTC = 0x01;
    _delay_ms(15);
}

void lcd_print(char str[])
{
    for (int i = 0; i < strlen(str); i++)
    {
        lcd_data(str[i]);
    }
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
    while (true)
    {
        // Cursor to first of first line
        command(0x80);
        lcd_print("Hello World");
        _delay_ms(50);
        // Cursor to first of second line
        command(0xC0);
        lcd_print("This is nice :)");
        _delay_ms(100);
        // Clearing display
        command(0x01);
    }
}