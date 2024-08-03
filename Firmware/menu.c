#include <proc/p32mx270f256d.h>

#include "menu.h"
#include "i2s.h"
#include "lcd.h"

#define C_LUFT 343.2

typedef enum menuState{Idle, Delay, Delay1, Delay2, Delay3, Meters, Meters1, Meters2, Meters3, Meters4} MenuState;

unsigned int delayMetersNew = 0, delayMeters = 0;
unsigned int delayTimeNew = 0, delayTime = 0;
MenuState currMenuState = Idle;

extern unsigned int buttonsDebounce;

/**
 * Menu Structure:
 *	Delay: 0.xyz seconds (saved as xyz int)
 *	Meters: ab,cd meters (saved as abcd int)
 *	Idle -> Menu pressed: select Delay or Meters
 *	Not Idle -> Menu pressed: cancel
 *	Not Idle:	Change between Delay and Meters with up and down
 *				Enter to confirm
 *				Up and down to change digit, enter confirm
 *				Enter at last digit: confirm change, calculate and change delay samples
 * 
 * @param pressed Button pressed as enum Buttons - Menu, Up, Down or Enter
 */
void buttonPressed(Buttons pressed)
{
	buttonsDebounce = 1;
	T1CONbits.ON = 1;	//activate Timer1 for debouncing
	switch(pressed)
	{
	case B_Menu:
		if(currMenuState == Idle) currMenuState = Delay;
		else
		{
			currMenuState = Idle;
			delayMetersNew = delayMeters;
			delayTimeNew = delayTime;
		}
		break;
	case B_Up:
		switch(currMenuState)
		{
		case Delay:
			currMenuState = Meters;
			break;
		case Delay1:
			delayTimeNew += 100;
			if(delayTimeNew > 999) delayTimeNew -= 1000;
			break;
		case Delay2:
			if(delayTimeNew % 100 >= 90) delayTimeNew -= 90;
			else delayTimeNew += 10;
			break;
		case Delay3:
			if(delayTimeNew % 10 == 9) delayTimeNew -= 9;
			else delayTimeNew += 1;
			break;
		case Meters:
			currMenuState = Delay;
			break;
		case Meters1:
			delayMetersNew += 1000;
			if(delayMetersNew > 9999) delayTimeNew -= 10000;
			break;
		case Meters2:
			if(delayTimeNew % 1000 >= 900) delayTimeNew -= 900;
			else delayTimeNew += 100;
			break;
		case Meters3:
			if(delayTimeNew % 100 >= 90) delayTimeNew -= 90;
			else delayTimeNew += 10;
			break;
		case Meters4:
			if(delayTimeNew % 10 >= 9) delayTimeNew -= 9;
			else delayTimeNew += 1;
			break;
		}
		break;
	case B_Down:
		switch(currMenuState)
		{
		case Delay:
			currMenuState = Meters;
			break;
		case Delay1:
			if(delayTimeNew % 1000 < 100) delayTimeNew += 900;
			else delayTimeNew -= 100;
			break;
		case Delay2:
			if(delayTimeNew % 100 < 90) delayTimeNew += 90;
			else delayTimeNew -= 10;
			break;
		case Delay3:
			if(delayTimeNew % 10 == 0) delayTimeNew += 9;
			else delayTimeNew -= 1;
			break;
		case Meters:
			currMenuState = Delay;
			break;
		case Meters1:
			if(delayTimeNew % 10000 < 9000) delayTimeNew += 9000;
			else delayTimeNew -= 1000;
			break;
		case Meters2:
			if(delayTimeNew % 1000 < 900) delayTimeNew += 900;
			else delayTimeNew -= 100;
			break;
		case Meters3:
			if(delayTimeNew % 100 < 90) delayTimeNew += 90;
			else delayTimeNew -= 10;
			break;
		case Meters4:
			if(delayTimeNew % 10 < 9) delayTimeNew += 9;
			else delayTimeNew -= 1;
			break;
		}
		break;
	case B_Enter:
		switch(currMenuState)
		{
		case Delay3:
			currMenuState = Idle;
			delayTime = delayTimeNew;
			delayMeters = delayTime / 10 * C_LUFT;
			delayMetersNew = delayMeters;
			setSamples(F_AUDIO / 1000 * delayTime);
			break;
		case Meters4:
			currMenuState = Idle;
			delayMeters = delayMetersNew;
			delayTime = 10 * delayMeters / C_LUFT;
			delayTimeNew = delayTime;
			setSamples(F_AUDIO / 1000 * delayTime);
			break;
		case Idle:
			break;
		default:
			currMenuState++;
		}
		break;
	}
	refreshLCD();
}

/**
 * Menu:
 * >Delay:  0.xyz s
 *  Meters: ab.cd m
 * 
 * current digit underlined and arrow in front of delay/meters if not in idle
 */
void refreshLCD()
{
	
}