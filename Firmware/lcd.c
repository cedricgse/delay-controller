#include "lcd.h"

void sendI2C(char c, char isChar){
	
}

void writeByte(char c, char isChar)
{
	
}

void writeString(char* text)
{
	
}

void clear()
{
	writeByte(0x01, 0);	// 0x01 = clear
						// delay, command needs 1.52ms
}