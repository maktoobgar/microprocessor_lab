#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

int s = 0, m = 0, h = 0;
const char nums[] PROGMEM = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

void timer2_initialization()
{
    // prescaler = clk/8
    TCCR2 |= (1 << CS22);

    // initialization of timer 1 register
    TCNT2 = 0;
}

void command(unsigned char cmd)
{
    PORTC = 0x02;
    PORTD = cmd;
    PORTC = 0x00;
    _delay_ms(5);
}

void lcd_init()
{
    command(0x38);
    command(0x06);
    command(0x0E);
    command(0x01);
    command(0x80);
}

void lcd_data(unsigned char data)
{
    PORTC = 0x03;
    PORTD = data;
    PORTC = 0x01;
    _delay_ms(5);
}

void time_passing()
{
    s++;
    if (s >= 60)
    {
        s = 0;
        m++;
    }
    if (m >= 60)
    {
        m = 0;
        h++;
    }
}

void display()
{
    lcd_data(pgm_read_byte(&(nums[h / 10])));
    lcd_data(pgm_read_byte(&(nums[h % 10])));
    lcd_data(':');
    lcd_data(pgm_read_byte(&(nums[m / 10])));
    lcd_data(pgm_read_byte(&(nums[m % 10])));
    lcd_data(':');
    lcd_data(pgm_read_byte(&(nums[s / 10])));
    lcd_data(pgm_read_byte(&(nums[s % 10])));
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

    // Timer 2 Initialization
    timer2_initialization();
    // LCD Initialization
    lcd_init();

    int clock_counter = 0;

    while (true)
    {
        if (TCNT2 >= 125)
        {
            clock_counter++;
            TCNT2 = 0;
        }

        if (clock_counter >= 125)
        {
            // Clearing the display
            command(0x01);
            time_passing();
            display();
            clock_counter = 0;
        }
    }
}