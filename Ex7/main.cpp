#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "keypad.h"

const char col0[] PROGMEM = {'3', '6', '9', '#'};
const char col1[] PROGMEM = {'2', '5', '8', '0'};
const char col2[] PROGMEM = {'1', '4', '7', '*'};

// 0 = admin input ID
// 1 = admin input password
// 2 = admin add user id
// 3 = admin add user password
// 4 = user login
int state = 0;
bool initialization_state_done = false;

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

void lcd_print(char str[])
{
    for (int i = 0; i < strlen(str); i++)
    {
        lcd_data(str[i]);
    }
}

char lcd_num_print(int col, int row)
{
    if (col == 0)
    {
        if (row == 3)
        {
            return '#';
        }
        lcd_data(pgm_read_byte(&(col0[row])));
        return col0[row];
    }
    else if (col == 1)
    {
        lcd_data(pgm_read_byte(&(col1[row])));
        return col1[row];
    }
    else if (col == 2)
    {
        if (row == 3)
        {
            return '*';
        }
        lcd_data(pgm_read_byte(&(col2[row])));
        return col2[row];
    }
    else
        return ';';
}

void reset_lcd()
{
    // Cursor to first of first line
    command(0x80);
    // Clearing the display
    command(0x01);
}

void confirm_message()
{
    reset_lcd();
    lcd_print("    Received    ");
    _delay_ms(1000);
}

void confirm_admin()
{
    reset_lcd();
    lcd_print(" Welcome  Admin ");
    _delay_ms(1000);
}

void under_process()
{
    while (true)
    {
        reset_lcd();
        lcd_print("Under Process");
        _delay_ms(500);
        lcd_print(".");
        _delay_ms(500);
        lcd_print(".");
        _delay_ms(500);
        lcd_print(".");
        _delay_ms(500);
    }
}

int main()
{
    // Keypad
    DDRB = 0x70;
    PORTB = 0x70;

    // LCD
    DDRC = 0xFF;
    DDRD = 0xFF;

    // Activate two lines in 8 bit mode
    command(0x38);
    // Display on, cursor blinking
    command(0x0f);
    // Clearing the display
    command(0x01);

    // LCD Initialization
    lcd_init();

    while (true)
    {
        if (state == 0)
        {
            if (!initialization_state_done)
            {
                reset_lcd();

                lcd_print("*=clear #=finish");
                _delay_ms(3000);

                reset_lcd();

                lcd_print("Admin ID:");
                // Cursor to first of second line
                command(0xC0);
                initialization_state_done = true;
            }
        }

        else if (state == 1)
        {
            if (!initialization_state_done)
            {
                reset_lcd();

                lcd_print("*=clear #=finish");
                _delay_ms(1000);

                reset_lcd();

                lcd_print("Admin Password:");
                // Cursor to first of second line
                command(0xC0);
                initialization_state_done = true;
            }
        }

        else
        {
            under_process();
        }

        if (key_pressed())
        {
            int row = which_row();
            int col = which_col();

            // receiving ID from Admin
            if (state == 0)
            {
                char special = lcd_num_print(col, row);

                // confirm command
                if (special == '#')
                {
                    confirm_message();
                    initialization_state_done = false;
                    state += 1;
                }
                // clear command
                else if (special == '*')
                {
                    initialization_state_done = false;
                    reset_lcd();
                }
            }

            // receiving Password from Admin
            else if (state == 1)
            {
                char special = lcd_num_print(col, row);

                // confirm command
                if (special == '#')
                {
                    confirm_message();
                    initialization_state_done = false;
                    state += 1;
                }
                // clear command
                else if (special == '*')
                {
                    initialization_state_done = false;
                    reset_lcd();
                }
            }

            while (!key_released())
                ;
        }
    }
}