#ifndef LCD_H
#define LCD_H

void initI2C(char lcd_address);
void sendI2C(char c, char isChar);
void writeByte(char c, char isChar);
void writeString(char* text);
void clear();

#endif