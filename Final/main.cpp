#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

const float delays[] = {0.1f, 0.09f, 0.08f, 0.07f, 0.06f, 0.05f, 0.04f, 0.03f, 0.02f, 0.01f};
// ((F_CPU / 256) * 0.1)
// 0.1 = 100ms wait for stepper to make the move
float COUNT = (F_CPU / float(256)) * delays[0];

int counter = 0;
int step_right = 0;
int step_left = 0;
bool right = false;
bool left = false;

unsigned char steps[] = {0b0101, 0b0110, 0b1010, 0b1001};

void _interuption_initialization()
{
    sei();
}

void _timer0_initialization()
{
    TCCR0 = (1 << CS00);  // Toggle Compare On OC0/PORTB.3, No Prescaling
    TIMSK = (1 << TOIE0); // Overflow Interupt Enabled, Compare Interupt Enabled
    TCNT0 = 0x00;
}

void next_move()
{
    if (right)
    {
        if (++step_right >= 4)
        {
            step_right = 0;
        }
        PORTA = steps[step_right];
    }
    if (left)
    {
        if (++step_left >= 4)
        {
            step_left = 0;
        }
        PORTB = steps[step_left];
    }
}

ISR(TIMER0_OVF_vect)
{
    if (++counter >= COUNT)
    {
        counter = 0;
        next_move();
    }
}

int main()
{
    DDRC = 0x00;
    DDRA = 0xFF;
    DDRB = 0xFF;
    PORTA = 0x00;
    PORTB = 0x00;
    _interuption_initialization();
    _timer0_initialization();

    while (true) {
        char comp = (PINC & 0b00011111);
        if      ((comp == 0b00000011) || (comp == 0b00000001)) {
            right=false;
            left=true;
        }
        else if ((comp == 0b00000110) || (comp == 0b00000010)) {
            right=false;
            left=true;
        }
        else if (comp == 0b00000100) {
            right=true;
            left=true;
        }
        else if ((comp == 0b00001100) || (comp == 0b00001000)) {
            right=true;
            left=false;
        }
        else if ((comp == 0b00011000) || (comp == 0b00010000)) {
            right=true;
            left=false;
        }
        else {
            right=false;
            left=false;
        }
    }
}
