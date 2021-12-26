#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

const float delays[] = {0.05f, 0.1f};
// ((F_CPU / 256) * 0.1)
// 0.1 = 100ms wait for stepper to make the move
float COUNT[] = {(F_CPU / float(256)) * delays[0], (F_CPU / float(256)) * delays[1]};

int counter_right = 0;
int counter_left = 0;
int step_right = 0;
int step_left = 0;
bool right = false;
bool left = false;
int right_speed = 0;
int left_speed = 0;

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

void next_right_move()
{
    if (++step_right >= 4)
    {
        step_right = 0;
    }
    PORTA = steps[step_right];
}

void next_left_move()
{
    if (++step_left >= 4)
    {
        step_left = 0;
    }
    PORTB = steps[step_left];
}

ISR(TIMER0_OVF_vect)
{
    if (++counter_right >= COUNT[right_speed])
    {
        counter_right = 0;
        if (right)
            next_right_move();
    }
    if (++counter_left >= COUNT[left_speed])
    {
        counter_left = 0;
        if (left)
            next_left_move();
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
            left_speed = 1;
        }
        else if ((comp == 0b00000110) || (comp == 0b00000010)) {
            right=true;
            left=true;
            right_speed = 1;
            left_speed = 0;
        }
        else if (comp == 0b00000100) {
            right=true;
            left=true;
            right_speed = 0;
            left_speed = 0;
        }
        else if ((comp == 0b00001100) || (comp == 0b00001000)) {
            right=true;
            left=true;
            right_speed = 0;
            left_speed = 1;
        }
        else if ((comp == 0b00011000) || (comp == 0b00010000)) {
            right=true;
            left=false;
            right_speed = 1;
        }
        else {
            right=false;
            left=false;
        }
    }
}
