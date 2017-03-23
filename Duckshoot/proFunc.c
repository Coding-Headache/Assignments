/*****************************************************************************
 * Program Author:  Daniel Hyde
 * Student ID:      ********
 * File name:       project.c
 * Course code:     UFCFF6-30-1 - C Programming
 * Date created:    20/11/2015
 *
 * Description: This program is a Duck shoot game made for the Altera DE0-Nano
 *              board. To win the game you must turn off all the LED's(Ducks)
 *              by shooting them using "Button 2" ,the player will lose if all
 *              LED's are lit. In this game the player will always aim at the
 *              least significant bit "LED 0". Game speed can be changed using
 *              switches 6 and 7. The direction of the LED's can be changed
 *              using switch 1.
 * **************************************************************************/

#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "alt_types.h"
#include <stdlib.h>

int main (void) __attribute__ ((weak, alias ("alt_main")));

int initial_setup(alt_u16 *,alt_u16 *, int pace);
int take_shot(alt_u16 *, int *);
void loop_delay (void);
void display_count (int num);
void led_rotate(alt_u16 *, alt_u16 *);
alt_u16 end_game(alt_u16 *);

#define switch0 0x01        /*define the switches for speed and direction*/
#define switch1 0x40
#define switch2 0x80

#define speed1 1500       /*define the speeds*/
#define speed2 300000
#define speed3 450000
#define speed4 600000

int alt_main(void)
{
    alt_u16 led = 0x2AA, switches;
    int count = 0;
    int speed = speed1;
    volatile int i;
    
    while(1)
    {
        speed = initial_setup(&switches, &led, speed);
        take_shot(&led, &count);
        led_rotate(&switches, &led);
        end_game(&led);
        
        i=0;
        while (i<speed)
            i++;
    }
    return 0;
}

/*****************************************************************************
 * Function name: initial_setup()
 *
 * Description: This function sets the board for the game. Including: the
 *              starting LED pattern and the speed.
 * **************************************************************************/
int initial_setup(alt_u16 *sw_flip, alt_u16 *lgt, int pace)
{
    alt_16 speedSelect;
    
    *sw_flip = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_BASE);
    *sw_flip = (*sw_flip & 0xC0);          /* Mask unused switches */
    speedSelect = (*sw_flip >> 6) & 0x03;
    *lgt = (*lgt & 0x3FF);                  /* Set LED pattern */
    switch(speedSelect)                     /* Checks to see what speed switches are set */
    {
        case 3: (pace = speed4); break;
        case 2: (pace = speed3); break;
        case 1: (pace = speed2); break;
        case 0: (pace = speed1); break;
    }
    return(pace);
}

/*****************************************************************************
 * Function name: take_shot()
 *
 * Description: This function will turn the least significant LED on or off
 *              with each button press, dependant upon the LEDs previous 
 *              state.
 * **************************************************************************/
int take_shot(alt_u16 *lgt, int *num)
{
    static alt_u8 shot;
    alt_u16 button = 0x00;
    
    button = IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE);
    button = (~button & 0x04);
    
    if ((button & 0x04) && !shot)   /* Check that button pressed and shot=0 (new shot) */
    {
        shot = 1;                   /* Set shot to 1, this will disable shot until released */
        
        if(*lgt & 0x01)             /* Check if LSB is on */
        {
            *lgt -= 0x01;           /* If LSB on, turn off */
            (*num)++;
        }
        else if(!(*lgt & 0x01))     /* Check if LSB is off */
        {
            *lgt += 0x01;           /* If LSB off, turn on */
            (*num)++;
        }
    }
    if(!(button & 0x04) && shot)    /* Check that button is not pressed and shot=1 */
    {
        shot = 0;                   /* Resets shot to 0 for next itteration */
    }
    IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, *lgt);
    loop_delay();
    display_count(*num);             /*Display seven segment with count*/
    loop_delay();
    
    return (*num);
}


/*****************************************************************************
 * Function name: loop_delay()
 *
 * Description: This function creates a delay through an empty loop.
 * **************************************************************************/
void loop_delay (void)
{
    int j = 0;
    for (j = 0;j<10000;j++)
    {
    }
}

/*****************************************************************************
 * Function name: display_count()
 *
 * Description: This function creates a count on the seven segment from 0-9999
 *              the counter increments everytime the button is pressed.
 * **************************************************************************/
void display_count (int num)
{
    alt_u8 *seg;
    alt_u32 display = 0xFFFFFF;
    alt_u8 values[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90}; /* Hex values 0 - 9 */
    
    seg = (unsigned char *) &display;
    
    *seg = values[num % 10];               /*4th display segment*/
    *(seg + 1) = values[(num/10) % 10];    /*3rd display segment*/
    *(seg + 2) = values[(num/100) % 10];   /*2nd display segment*/
    *(seg + 3) = values[(num/1000) % 10];  /*1st display segment*/
    
    IOWR_ALTERA_AVALON_PIO_DATA(SEG7_BASE, display);
}

/*****************************************************************************
 * Function name: led_rotate()
 *
 * Description: This function will confirm the direction of the LED pattern
 *              and shift the pattern in the neccessary direction.
 * **************************************************************************/
void led_rotate(alt_u16 *sw_flip, alt_u16 *lgt)
{

    IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, *lgt);
    if (*sw_flip & switch0)             /* Checks if switch0 is on*/
    {
        if (*lgt & 0x001)               /* Check if least significant bit is on*/
        {
            *lgt = *lgt >> 1;           /* Shift LED pattern right 1 */
            *lgt += 0x200;              /* Turn most significant bit on*/
            loop_delay();
        }
        else
        {
            *lgt = *lgt >> 1;
            loop_delay();
        }
    }
    else
    {
        if (*lgt & 0x200)               /*check most significant bit is on*/
        {
            *lgt = *lgt << 1;
            *lgt += 0x001;               /*Turn on least significant bit*/
        }
        else
        {
            *lgt = *lgt << 1;
        }
    }
}

/*****************************************************************************
 * Function name: end_game()
 *
 * Description: This function will test the LED pattern in order to determine
 *              the end of the game and the final result.
 * **************************************************************************/
alt_u16 end_game(alt_u16 *lgt)
{
    alt_u32 lose = 0xC7C09286;
    alt_u32 end = 0x86C8A17F;
    
    IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, *lgt);
    
    if (*lgt == !0x3FF)                     /* Player wins if all LEDs are off */
    {
        IOWR_ALTERA_AVALON_PIO_DATA(SEG7_BASE, end );
        exit(1);
    }
    else if(*lgt == 0x3FF)                  /* Player loses if all LEDs are on */
    {
        IOWR_ALTERA_AVALON_PIO_DATA(SEG7_BASE, lose );
        exit(1);
    }
    
    return (*lgt);
}











