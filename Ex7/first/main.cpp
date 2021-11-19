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

char dahgan = 'X';
char yekan = 'X';

char day[2] = {'0', '1'};
char month[2] = {'0', '2'};
char year[4] = {'1', '3', '7', '8'};

char id[] = {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'};
char pass[] = {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'};

char input[] = {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'};
int where_in_input = 0;

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
    PORTA = cmd;
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
    PORTA = data;
    PORTC = 0x01;
    _delay_ms(5);
}

void lcd_print(char str[])
{
    for (unsigned int i = 0; i < strlen(str); i++)
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
        if (where_in_input < 8)
            lcd_data(pgm_read_byte(&(col0[row])));
        return col0[row];
    }
    else if (col == 1)
    {
        if (where_in_input < 8)
            lcd_data(pgm_read_byte(&(col1[row])));
        return col1[row];
    }
    else if (col == 2)
    {
        if (row == 3)
        {
            return '*';
        }
        if (where_in_input < 8)
            lcd_data(pgm_read_byte(&(col2[row])));
        return col2[row];
    }
    else
        return 'X';
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

void overwrite()
{
    for (int i = 0; i < 8; i++)
    {
        input[i] = 'X';
    }
    where_in_input = 0;
}

void read_temperature()
{
    bool yekan_updated = false;
    bool dahgan_updated = false;
    while (!yekan_updated || !dahgan_updated)
    {
        while ((UCSRA & 0x80) == 0x00)
            ;
        if ((PIND & 0x08) == 0x08)
        {
            yekan_updated = true;
            yekan = UDR;
        }
        if ((PIND & 0x10) == 0x10)
        {
            dahgan_updated = true;
            dahgan = UDR;
        }
    }
}

void USART_receiving_initialization()
{
    // USART Receiving Initialization
    UBRRH = 0;
    UBRRL = 51;
    UCSRA = 0x00;
    UCSRB = 0x10;
    UCSRC = 0x86;
}

void USART_sending_initialization()
{
    // USART Sending Initialization
    UBRRH = 0x00;
    UBRRL = 51;
    UCSRA = 0x00;
    UCSRB = 0x08;
    UCSRC = 0x86;
}

int main()
{
    DDRD = 0x64;
    // To Activate INT0 To other microprocessor
    PORTD = 0x00;

    // Keypad
    DDRB = 0x70;
    PORTB = 0x70;

    // LCD
    DDRC = 0xFF;
    DDRA = 0xFF;

    USART_receiving_initialization();

    // Activate two lines in 8 bit mode
    command(0x38);
    // Display on, cursor blinking
    command(0x0f);
    // Clearing the display
    command(0x01);

    // LCD Initialization
    lcd_init();

    // Input Is Now An Empty Char*
    overwrite();

    while (true)
    {
        if (state == 0)
        {
            if (!initialization_state_done)
            {
                reset_lcd();

                lcd_print("    * = clear   ");

                // Cursor to first of second line
                command(0xC0);

                lcd_print("   # = confirm  ");

                _delay_ms(3000);

                reset_lcd();

                lcd_print("Enter 8 Numbers ");

                // Cursor to first of second line
                command(0xC0);

                lcd_print("For Admin ID... ");

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

                lcd_print("    * = clear   ");

                // Cursor to first of second line
                command(0xC0);

                lcd_print("   # = confirm  ");

                _delay_ms(3000);

                reset_lcd();

                lcd_print("Enter 8 Numbers ");

                // Cursor to first of second line
                command(0xC0);

                lcd_print("For Admin Pass..");

                _delay_ms(3000);

                reset_lcd();

                lcd_print("Admin Pass:");
                // Cursor to first of second line
                command(0xC0);
                initialization_state_done = true;
            }
        }

        else if (state = 2)
        {
            if (!initialization_state_done)
            {
                read_temperature();
                reset_lcd();

                lcd_print("temperature : ");
                lcd_data(dahgan);
                lcd_data(yekan);

                // Cursor to first of second line
                command(0xC0);

                lcd_print("date: 1378/02/01");
                for (int i = 0; i < 4; i++)
                {
                    lcd_data(year[i]);
                }
                lcd_data('/');
                for (int i = 0; i < 2; i++)
                {
                    lcd_data(month[i]);
                }
                for (int i = 0; i < 2; i++)
                {
                    lcd_data(day[i]);
                }

                _delay_ms(3000);

                reset_lcd();

                lcd_print("    * = clear   ");

                // Cursor to first of second line
                command(0xC0);

                lcd_print("   # = confirm  ");

                _delay_ms(3000);

                reset_lcd();

                lcd_print("Enter Rotation:");

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
                char in = lcd_num_print(col, row);
                if (in == '#')
                {
                    reset_lcd();
                    if (where_in_input > 7)
                    {
                        for (int i = 0; i < 8; i++)
                        {
                            id[i] = input[i];
                        }
                        overwrite();
                        confirm_message();
                        initialization_state_done = false;
                        state++;
                    }
                }
                else if (in == '*')
                {
                    reset_lcd();
                    overwrite();
                    initialization_state_done = false;
                    reset_lcd();
                }
                else
                {
                    if (where_in_input < 8)
                    {
                        input[where_in_input++] = in;
                    }
                }
            }

            // receiving Password from Admin
            else if (state == 1)
            {
                char in = lcd_num_print(col, row);
                if (in == '#')
                {
                    if (where_in_input > 7)
                    {
                        for (int i = 0; i < 8; i++)
                        {
                            pass[i] = input[i];
                        }
                        overwrite();
                        confirm_message();
                        initialization_state_done = false;
                        state++;
                    }
                }
                else if (in == '*')
                {
                    overwrite();
                    initialization_state_done = false;
                    reset_lcd();
                }
                else
                {
                    if (where_in_input < 8)
                    {
                        input[where_in_input++] = in;
                    }
                }
            }

            // enter rotation
            else if (state == 2)
            {
                char in = lcd_num_print(col, row);
                if (in == '#')
                {
                    if (where_in_input > 1)
                    {
                        USART_sending_initialization();
                        // Sending INT0 Interrupt
                        PORTD ^= 0x04;
                        for (int i = 0; i < 2; i++)
                        {
                            // Dahgan
                            if (i == 0)
                            {
                                PORTD = (PORTD & ~(0x40)) | 0x20;
                            }
                            // Yekan
                            else if (i == 1)
                            {
                                PORTD = (PORTD & ~(0x20)) | 0x40;
                            }
                            UDR = input[i];

                            while ((UCSRA & 0x20) == 0x00)
                                ;

                            _delay_ms(5000);
                        }
                        PORTD = ((PORTD & ~(0x20)) & ~(0x40));
                        USART_receiving_initialization();
                        overwrite();
                        confirm_message();
                        initialization_state_done = false;
                    }
                }
                else if (in == '*')
                {
                    overwrite();
                    initialization_state_done = false;
                    reset_lcd();
                }
                else
                {
                    if (where_in_input < 2)
                    {
                        input[where_in_input++] = in;
                    }
                }
            }

            while (!key_released())
                ;
        }
    }
}