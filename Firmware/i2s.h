#define BUFFER_SIZE 7100 //approx 30kByte
#define F_AUDIO 48000

#ifndef _I2S_H
#define _I2S_H

void i2s_config();
void i2sIn_config();
void i2sOut_config();
int setSamples(unsigned int samplesNew);
unsigned int calcReadWriteDiff();

void SPIInInterrupt();
void SPIOutInterrupt();

#endif