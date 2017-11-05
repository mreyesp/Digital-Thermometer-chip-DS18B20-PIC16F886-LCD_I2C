//************************************************************************************ 
// Digital thermometer using sensor DS18B20 connected to PIC16F886 + LCD display 
// By Moises Reyes - Jan 2017
//
// Use of 1wire.c library (C) copyright 2003 j.d.sandoz / jds-pic !at! losdos.dyndns.org 
// released under the GNU GENERAL PUBLIC LICENSE (GPL) 
// refer to http://www.gnu.org/licenses/gpl.txt 
// THIS LIBRARY WAS MODIFIED TO SET A /*-------delay definition-------*/ USING A 8MHz CLOCK.
// 
// Based in the 20x4 LCD driver test code (by PCM programmer) with a few changes made
// Modified by Moises Reyes to allow print special characters. 
//
// Funciones.h file has inside routines, allowing a clean view of main project.
//
// NOTE: This software show how to implement a digital thermometer using sensor DS18B20 and schowing the reoults on LCD I2C display.
//
// Hardware:
// - PIC16F886.
// - LCD Display 16x2 chip HD44780 or compatible.
// - Adapter, parallel to I2C. Chip PCF8574T or compatible.
// - Sensor DS18B20 (one wire protocol)
//************************************************************************************ 

#include<16f886.h>  
#include<1wire.h>
#include<C:\Program Files (x86)\PICC\Drivers\stdio.h>
#include<C:\Program Files (x86)\PICC\Drivers\math.h>

#fuses NOWDT,NOPROTECT          // To clarified the meaning of this "Configuration´s BITS"
#fuses NOLVP,INTRC              // go to the datasheet and also on MPLAB following the path
#fuses MCLR,NOBROWNOUT          // MPLAB-> Window-> Pic memory views->
#fuses NOIESO,NOFCMEN,DEBUG     // configuration bits

#use delay(clock=8000000)       // Inform to our compiler the clock frequency to be used  (8Mhz)
#define ocho_MHz 0b01110001     // Define the value of OSCCON register. Setting the internal clock 
                                // as source of system clock, running at 8MHz.
/* Setup UART */
#use rs232(STREAM = pc, BAUD = 57600, XMIT = PIN_C6, RCV = PIN_C7) 

/* Setup I2C */
#use I2C(MASTER, sda=PIN_C4, scl=PIN_C3, FAST=2000000) 

/* Variables to be used */
 int8 busy, temp1, temp2, i; 
 signed int16 temp3; 
 float result;
 int ID[8]; 
 int contador;
 int resulucion_sensor_temp;
 
// Creation of arrangment to save in the bits, which will represent the special char "°"
// To generate this codes, a free online tool was used, in this case was "Custom Character Generator".
// for HD44780 LCD Modules. https://omerk.github.io/lcdchargen/
char simbolo_gr_celcius[8] = {0b01110,0b10001,0b10001,0b01110,0b00000,0b00000,0b00000,0b00000};

#include <i2c_Flex_LCD_MRP_mod.h> 
#include <Funciones.h>

/* Routine to handle Timer0 interruptions */
#INT_timer0
void timer0_isr(void)   //One from 100 interrupts generate an action. In this case each 3200ms.
{
	set_timer0(6);    
	if(contador < 99){
		contador = contador +1;
	}
	else{
		delay_ms(25);
		DS_conversion();	// Function present in Funciones.h file, start the temperature conversion.
		Read_t();			// Function present in Funciones.h file, read the temperature conversion.
		result = temp3 / 16.0;  // Divided by 16.0 (float value) to obtain as result a float number, the number 16 is the equivalent to do 
								// a left shift 4 in that way the decimal value stays in the right position to be directly used.
		print_tem_leida(result);	// Function present in Funciones.h file, print on LCD display the value of temperature.
		contador = 0;
	}	
}

void main()
{  
    #byte OSCCON = ocho_MHz;   // The register OSCCON take the value previously defined.
/* Initialization and setup of Timer0 interruptions */	
    setup_timer_0(RTCC_INTERNAL|RTCC_DIV_256);
    set_timer0(6);	// The value 6 generates an interruption every 32ms.
    enable_interrupts(INT_TIMER0);
    enable_interrupts(GLOBAL);
    contador = 100;  // Initialization of variable "contador = 100" allow to read the temperature at start the system, then will be each 100 cycles.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    resulucion_sensor_temp = 9; // Variable used to set the conversion resolution for chip DS18B20 (options 9, 10, 11 y 12 bits)
    busy=0;
    
/* Function present in i2c_Flex_LCD_MRP_mod.h file. Initialization of LCD display */
    lcd_init();

/* Function present in Funciones.h file. ID reading, located in the ROM of DS18B20 */    
    ID_Read();   
/* Print on LCD display the previously read ID. (To see the value, the UART terminal must be setted in HEX, because the data came raw.) */
//    for(i=0; i<8; i++){
//        fprintf(pc,"%c", ID[i]);
//    }
//    fprintf(pc,"%c", ID[0]);

/* Function present in Funciones.h file. Convert the data format from HEX to Char. In that way the user can easily read the data. */
//   Hex_To_Char();

/* Function present in Funciones.h file. Set the conversion resolution for chip DS18B20 (options 9, 10, 11 y 12 bits) */ 
   set_resulucion(resulucion_sensor_temp);
  
   while(True){} // Endless loop. The Timer0 interruptions are in charge to execute and trigger every needed procedure.
}