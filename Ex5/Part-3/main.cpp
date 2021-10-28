#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// ((F_CPU / 256) * 0.005)
// 0.005 = 5ms wait for stepper to make the move

const float delays[] = {0.1f, 0.09f, 0.08f, 0.07f, 0.06f, 0.05f, 0.04f, 0.03f, 0.02f, 0.01f};
int current_delay = 0;
float COUNT = (F_CPU / float(256)) * delays[current_delay];
const int max_rotation_till_new_delay = 10;
int rotation_times = 0;
bool direction = true;

float counter = 0;
int step = 0;

unsigned char steps[] = {0b0101, 0b0110, 0b1010, 0b1001};

void _interuption_initialization()
{
    sei();
}

void _timer0_initialization()
{
    TCCR0 = (1 << CS00); // Toggle Compare On OC0/PORTB.3, No Prescaling
    TIMSK = (1 << TOIE0);               // Overflow Interupt Enabled, Compare Interupt Enabled
    TCNT0 = 0x00;
}

void next_move()
{
    if (direction)
    {
        if (++step >= 4)
        {
            step = 0;
        }
    }
    else
    {
        if (--step <= -1)
        {
            step = 3;
        }
    }

    if (++rotation_times >= max_rotation_till_new_delay)
    {
        rotation_times = 0;
        if (direction)
        {
            if (++current_delay >= 10)
            {
                direction = false;
                --current_delay;
            }
        }
        else {
            if (--current_delay <= -1)
            {
                direction = true;
                ++current_delay;
            }
        }
        COUNT = (F_CPU / float(256)) * delays[current_delay];
    }

    PORTA = steps[step];
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
    DDRD = 0xFF;
    DDRA = 0xFF;
    PORTA = 0x00;
    _timer0_initialization();
    _interuption_initialization();

    while (true)
        ;
}
