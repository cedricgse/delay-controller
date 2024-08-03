//PIC32 Audio Controller
// PIC32MX270F256D Configuration Bit Settings

// 'C' source line config statements

// DEVCFG3
//#pragma config USERID = 0xFFFF          // Enter Hexadecimal value (Enter Hexadecimal value)
#pragma config PMDL1WAY = OFF           // Peripheral Module Disable Configuration (Allow multiple reconfigurations)
#pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow multiple reconfigurations)
#pragma config FUSBIDIO = OFF           // USB USID Selection (Controlled by Port Function)
#pragma config FVBUSONIO = OFF          // USB VBUS ON Selection (Controlled by Port Function)

// DEVCFG2
#pragma config FPLLIDIV = DIV_10        // PLL Input Divider (10x Divider)
#pragma config FPLLMUL = MUL_18         // PLL Multiplier (18x Multiplier)
#pragma config UPLLIDIV = DIV_10        // USB PLL Input Divider (10x Divider)
#pragma config UPLLEN = OFF             // USB PLL Enable (Disabled and Bypassed)
#pragma config FPLLODIV = DIV_8         // System PLL Output Clock Divider (PLL Divide by 8)

// DEVCFG1
#pragma config FNOSC = PRI              // Oscillator Selection Bits (Primary Osc (XT,HS,EC))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = OFF               // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = OFF            // Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1024           // Watchdog Timer Postscaler (1:1024) - 1.024s
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = ON              // Watchdog Timer Enable (WDT Enabled)
#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window Size is 25%)

// DEVCFG0
#pragma config JTAGEN = ON              // JTAG Enable (JTAG Port Enabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define SYS_FREQ 40000000

#include <xc.h>
#include <proc/p32mx270f256d.h>
#include "i2s.h"
#include "menu.h"

#define LCD_ACTIVE 0
#define BUTTONS_ACTIVE 0

#define LCD_Address 0x3F

#define BUTTON_CONF TRISA
#define BUTTON1C 7 
#define BUTTON2C 8
#define BUTTON3C 9
#define BUTTON4C 10
#define BUTTON1 PORTAbits.RA7
#define BUTTON2 PORTAbits.RA8
#define BUTTON3 PORTAbits.RA9
#define BUTTON4 PORTAbits.RA10

unsigned int buttonsDebounce = 0;

void configPins()
{
    //No analog Inputs
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    
    //Initialize all pins outputs
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    
    //Initialize all outputs low
    LATA = 0;
    LATB = 0;
    LATC = 0;
    
    //Buttons Input Pin Config
    if(BUTTONS_ACTIVE)
    {
        BUTTON_CONF |= (1 << BUTTON1C) | (1 << BUTTON2C) | (1 << BUTTON3C) | (1 << BUTTON4C);
    }
}

int main()
{
    configPins();
    i2s_config();
	
	// Configure timer 1 for buttons debouncing
	T1CON |= (0b10 << 4);	// Timer 1 1:64 prescaler
	IEC0bits.T1IE = 1;		// Enable Timer 1 Interrupt
	PR1 = 625;				// 1 ms debounce: Pb_clk*tdebounce/prescaler
	
    WDTCONSET = 0x8000;		// Enable WDT
    while(1)
    {
		if(buttonsDebounce)
		{
			if(IFS0bits.T1IF)
			{
				T1CONbits.ON = 0;		// Stop Timer
				TMR1 = 0;				// Reset Timer Counter
				IFS0CLR |= 1 << 4;	// Reset Interrupt flag
				buttonsDebounce = 0;
			}
		}
		if(BUTTONS_ACTIVE && !buttonsDebounce)
		{
			if(!BUTTON1) buttonPressed(B_Enter);
			if(!BUTTON2) buttonPressed(B_Up);
			if(!BUTTON3) buttonPressed(B_Down);
			if(!BUTTON4) buttonPressed(B_Enter);
		}
        WDTCONSET = 0x01; //Reset WDT
    }
    return 0;
}