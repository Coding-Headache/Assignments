/********************************************************************************
*   Program Name:   line.c
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
*                   MARCO bot to follow a simple line. The MARCO is built with
*                   a DE0 nano board.
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
void reacquire(alt_u32);     
void clearence(alt_u32, alt_u32);
/*******************************************************************************/

int alt_main()
{
    alt_u32 sensors;
    alt_u32 motor;
    alt_u32 bumpers = 0;
    int i = 0;

    IOWR_ALTERA_AVALON_PIO_DIRECTION(EXPANSION_JP1_BASE,0xF000000F);

    while(1)
    {   
        bumpers = IORD_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE);
        bumpers &= (LEFT_FRONTBUMPER | RIGHT_FRONTBUMPER);
        bumpers >>= 11;                             // assigns left and right 
                                                    // bumper to bumpers and 
                                                    // shifts.
    
        sensors = IORD_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE);
        sensors = sensors & (LEFT_LINEFOLLOWER | RIGHT_LINEFOLLOWER);
        sensors >>= 13;                             // assigns left and right 
                                                    // linefollower to sensors 
                                                    // and shifts.
                      
        clearence(bumpers, motor);

        switch(sensors)                             // Whilst sensors read: 
        {
            case 0x0    :for(i = 0; i < 40; i++)
                        {
                            motor = 0x7;            // Turn to the right
                            reacquire(motor);       // to find the line initially
                            motor = 0xF;            // move forward.
                           
                        }
                        break;
                            
            case 0x1    :for(i = 0; i < 40; i++)
                        {
                               motor = 0xF;         // Move forward
                               forth_slow(motor);   
                        }
                        break;
                            
            case 0x2    :for(i = 0; i < 30; i++)
                        {
                            motor = 0x7;            // Turn right
                            reacquire(motor);
                            motor = 0xF;            // continue forward
                            forth_slow(motor);
                        }
                        break;
                        
            case 0x3    :for(i=0;i< 40; i++)
                        {
                            motor = 0xB;            // Turn left
                            forth_slow(motor);
                            motor = 0xF;            // continue forward
                            reacquire(motor);
                        }
                        break;

            default:    break;
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
    }
    
    for(y = 0; y < 5; y++)
    {
        motor = 0x0;
        IOWR_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE, motor);
    }    
}

/********************************************************************************
*   Function Name:  void reacquire(alt_u32 motor)
*       returns  :  Nil
*       arg1     :  alt_u32 motor;  passing the value of the motor.
*   Created by   :  Daniel Hyde
*   Date Created :  15/01/2016
*   Description  :  Will rapidly change the motor state for line acquisition
*                   after a turn.
*   Notes        :  Nil
********************************************************************************/

void reacquire(alt_u32 motor)
{
    int x = 0;
    int y = 0;
    
    for(x = 0; x < 20; x++)
    {
        IOWR_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE, motor);
    }
    
    for(y = 0; y < 5; y++)
    {
        motor = 0x0;
        IOWR_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE, motor);
    } 
    
}

/********************************************************************************
*   Function Name:  void clearence(alt_u32 bumpers, alt_u32 motor)
*       returns  :  Nil
*       arg1     :  alt-u32 bumpers; will pass the value of bumpers variable.
*       arg2     :  alt_u32 motor; passing the value of the motor variable.
*   Created by   :  Daniel Hyde
*   Date Created :  15/01/2016
*   Description  :  Will move the MARCO foward whilst rapidly changing the motor
*                   state for line acquisition.
*   Notes        :  Nil
********************************************************************************/

void clearence(alt_u32 bumpers, alt_u32 motor)
{
    while (bumpers != 17)
        {
            motor = 0x0;
            IOWR_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE, motor);
            
            bumpers = IORD_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE);
            bumpers &= (LEFT_FRONTBUMPER | RIGHT_FRONTBUMPER);
            bumpers >>= 11;
        }
        
    return;
}


