#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// ((F_CPU / 256) * 0.005)
// 0.005 = 5ms wait for stepper to make the move
#define COUNT 19.5f

float counter = 0;
int step = 0;

unsigned char steps[] = {0b0101, 0b0110, 0b1010, 0b1001};

void _interuption_initialization()
{
    sei();
}

void _timer0_initialization()
{
    TCCR0 = (0 << COM01) | (1 << COM00) | (1 << CS00); // Toggle Compare On OC0/PORTB.3, No Prescaling
    TIMSK = (1 << TOIE0) | (1 << OCIE0);               // Overflow Interupt Enabled, Compare Interupt Enabled
    TCNT0 = 0x00;
    OCR0 = 128;
}

void next_move()
{
    if (++step >= 4)
    {
        step = 0;
    }
    PORTA = steps[step];
}

ISR(TIMER0_OVF_vect)
{
    counter += 0.5;

    if (counter >= COUNT)
    {
        counter = 0;
    }
}

ISR(TIMER0_COMP_vect)
{
    counter += 0.5;

    if (counter >= COUNT)
    {
        counter = 0;
        next_move();
    }
}

int main()
{
    DDRC = 0;
    while ((PINC & 0x04) == 0x04);
    DDRA = 0xFF;
    PORTA = 0x00;
    _timer0_initialization();
    _interuption_initialization();

    while (true)
    {
    }
}
