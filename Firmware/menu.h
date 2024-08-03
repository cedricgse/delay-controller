#ifndef MENU_H
#define MENU_H

typedef enum buttons{B_Menu, B_Up, B_Down, B_Enter} Buttons;

void buttonPressed(Buttons pressed);
void refreshLCD();

#endif