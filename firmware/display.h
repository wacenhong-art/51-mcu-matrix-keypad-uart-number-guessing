#ifndef DISPLAY_H
#define DISPLAY_H
#include "game.h"
void display_init(void);
void display_begin(void);
void display_input(u8 *digits,u8 count);
void display_result(u8 *digits,u8 x,u8 y);
void display_short(void);
#endif
