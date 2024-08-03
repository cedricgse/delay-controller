#include "i2s.h"
#include <xc.h>
#include <proc/p32mx270f256d.h>
#include <sys/attribs.h>

typedef enum initState{InitComplete, ReadBeforeWrite, WriteBeforeRead} InitState;

int audioBuffer[BUFFER_SIZE];
unsigned int readPos = 0, writePos = 0;
InitState currInitState = 0;
unsigned int delaySamples = 0;

void __ISR(_SPI_1_VECTOR, IPL7AUTO) SPIInInterrupt(void);
void __ISR(_SPI_2_VECTOR, IPL7AUTO) SPIOutInterrupt(void);

void i2s_config()
{
    // ADC and DAC pin config I2S and clock
    RPC3R = 0b0111;         // RPC3 = REFCLKO
    TRISBbits.TRISB5 = 1;   // RPB5 Input
    SDI1R = 0b0001;         // RPB5 = SDI1
    RPC7R = 0b0011;         // RPC7 = !SS1
    RPA3R = 0b0100;         // RPA3 = !SS2
    RPB13R = 0b0100;        // RPB13 = SDO2
    
    // Reference oscillator config
    REFOTRIM = 339 << 23;   // Trim M = 339
    REFOCON |= 3 << 16;     // Divider N = 3
    REFOCONbits.OE = 1;     // Reference clock on REFCLK0 pin
    
    i2sIn_config();
    i2sOut_config();
}

void i2sIn_config()
{
    unsigned int rData;
    IEC1CLR = 0x00000070;   // disable all interrupts
    SPI1CON = 0;            // Stops and resets the SPI1
    SPI1CON2 = 0;           // Reset audio settings
    SPI1BRG = 0;            // Reset Baud rate register
    rData = SPI1BUF;        // clears the receive buffer
    IFS1CLR = 0x00000070;   // clear any existing event
    IPC7CLR = 0x1f000000;   // clear the priority
    IPC7SET = 0x0d000000;   // Set IPL = 3, Subpriority 1
    IEC1SET = 0x00000040;   // Enable RX interrupts
    
    SPI1STATCLR = 0x40;     // clear the Overflow
    SPI1CON2 = 0x00000080;  // I2S Mode, AUDEN = 1, AUDMON = 0
    SPI1BRG = 0x01;         //(to generate 256 kbps sample rate, PBCLK @ 40 MHz)
    SPI1CON = 0x00808C61;   // Use REFCLK, Master mode, SPI ON, CKP = 1, 24-bit audio channel
                            // data, 32 bits per frame, Interrupt receive buffer not empty
    // from here, the device is ready to receive and transmit data
}

void i2sOut_config()
{
    unsigned int rData;
    IEC1CLR = 0x001C0000;   // disable all interrupts
    SPI2CON = 0;            // Stops and resets the SPI1
    SPI2CON2 = 0;           // Reset audio settings
    SPI2BRG = 0;            // Reset Baud rate register
    rData = SPI1BUF;        // clears the receive buffer
    IFS1CLR = 0x001C0000;   // clear any existing event
    IPC9CLR = 0x0000001f;   // clear the priority
    IPC9SET = 0x0000000d;   // Set IPL = 3, Subpriority 1
    IEC1SET = 0x00100000;   // Enable TX interrupts
    
    SPI2STATCLR = 0x40;     // clear the Overflow
    SPI2CON2 = 0x00000080;  // I2S Mode, AUDEN = 1, AUDMON = 0
    SPI2BRG = 0x01;         //(to generate 256 kbps sample rate, PBCLK @ 40 MHz)
    SPI2CON = 0x00808C6C;   // Use REFCLK, Master mode, SPI ON, CKP = 1, 24-bit audio channel
                            // data, 32 bits per frame, Interrupt transmit buffer not full
    // from here, the device is ready to receive and transmit data
}

int setSamples(unsigned int samplesNew)
{
	if(samplesNew >= BUFFER_SIZE-10) return 0;	// if number of samples needed bigger than BUFFER_SIZE - 10 samples for safety
												// return 0 -> number of samples too big
	if(samplesNew < delaySamples)
	{
		currInitState = ReadBeforeWrite;
	}else if(samplesNew > delaySamples)
	{
		currInitState = WriteBeforeRead;
	}
	delaySamples = samplesNew;
	return 1;
}

unsigned int calcReadWriteDiff()
{
	if(writePos >= readPos)
	{
		return writePos - readPos;
	}else
	{
		return BUFFER_SIZE - readPos + writePos;
	}
}

void SPIInInterrupt()
{
	if(currInitState == WriteBeforeRead) return;
	if(currInitState == ReadBeforeWrite)
	{
		unsigned int difference = calcReadWriteDiff();
		if(difference == delaySamples) currInitState = InitComplete; 
		else if(difference > delaySamples) currInitState = WriteBeforeRead;
	}
    int data;
    data = SPI1BUF;
    audioBuffer[writePos] = data;
    writePos++;
    if(writePos >= BUFFER_SIZE) writePos = 0;
    
    IFS1CLR = 0x00000070;   // clear any existing event
}

void SPIOutInterrupt()
{
	if(currInitState == ReadBeforeWrite) return;
	if(currInitState == WriteBeforeRead)
	{
		unsigned int difference = calcReadWriteDiff();
		if(difference == delaySamples) currInitState = InitComplete; 
		else if(difference < delaySamples) currInitState = ReadBeforeWrite;
	}
    int data;
    data = audioBuffer[readPos];
    SPI1BUF = data;
    readPos++;
    if(readPos >= BUFFER_SIZE) readPos = 0;
    
    IFS1CLR = 0x001C0000;   // clear any existing event
}
