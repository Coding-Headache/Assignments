/********************************************************************************
*   Program Name:   escapist.c
*   Project Name:   MARCO BOT
*
*   Author Name:    Daniel Hyde
*   Copyright (C) 2016 A Coding Headache
*
*   This program is free software; you can redistribute it and/or modify it under
*   the terms of the GNU General Public License as published by the Free Software
*   Foundation; either version 3 of the License, or (at your option) any later
*   version.
*   
*   This program is distributed in the hope that it will be useful, but WITHOUT
*   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
*   FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
*   Date Created:   15/01/2016
*   Last Updated:   14/04/2016
*   File Description:
*                   This program contains the functions needed to allow a UWE 
*                   MARCO bot to escape from a room, primarily as a wall follower.
*                   The MARCO is built with a DE0 nano board.
********************************************************************************/
/*  Include section.
*   Add all #includes here:                                                    */
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "alt_types.h"
#include "nano.h"
/********************************************************************************
*   Function prototype section.
*   Add prototypes for all functions called by this program:                   */
int main (void) __attribute__ ((weak, alias ("alt_main")));

void forth_slow(alt_u32);
/*******************************************************************************/

int alt_main()
{
    alt_u32 bumpers = 0;
    alt_u32 motor;
    
     IOWR_ALTERA_AVALON_PIO_DIRECTION(EXPANSION_JP1_BASE,0xF000000F);

    while(1)
    {    
        bumpers = IORD_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE);
        bumpers &= (LEFT_FRONTBUMPER | RIGHT_FRONTBUMPER);
        bumpers >>= 11;
            
        switch(bumpers)
        {
            case 0x0    :while (bumpers != 17)  // Will test if bumpers are clear.
                        {
                            motor = 0xB;        // If not bot will turn left
                            forth_slow(motor);  // then continue forward.
                            bumpers = IORD_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE);
                            bumpers &= (LEFT_FRONTBUMPER | RIGHT_FRONTBUMPER);
                            bumpers >>= 11;     // Reset bumpers to clear
                        }
                        break;

            /* If the left bumper is on (detecting an object) */         
            case 0x1    :motor = 0x7;           // Bot will turn to the right
                        forth_slow(motor);      // then continue.
                        break;
                             
            /* If the right bumper is on (detecting an object) */             
            case 0x2    :motor = 0xB;           // Bot will turn to the left
                        forth_slow(motor);      // then continue.
                        break;
                             
            /* If both bumpers are off (unobstructed) */             
            case 0x3    :motor = 0xF;           // Bot will continue forward
                        forth_slow(motor);
                        break;
            }
    }
}

/********************************************************************************
*   Function Name:  void forth_slow(alt_u32 motor)
*       returns  :  Nil
*       arg1     :  alt_u32 motor;  passing the value of the motor.
*   Created by   :  Daniel Hyde
*   Date Created :  15/01/2016
*   Description  :  Will move the MARCO foward whilst rapidly changing the motor
*                   state for line acquisition.
*   Notes        :  Nil
********************************************************************************/

void forth_slow(alt_u32 motor)
{
    int x = 0;
    int y = 0;
    
    for(x = 0; x < 100; x++)
    {
        IOWR_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE, motor);
                                                // Writes to the motors
    }
    
    for(y = 0; y < 05; y++)
    {
        motor = 0x0;                            // Halts the motors
        IOWR_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE, motor);
    }    
}
