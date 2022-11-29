#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include"conio2.h"

/* Comment: in the final program declare appropriate constants, e.g.,
   to eliminate from your program numerical values by replacing them
   with well defined identifiers */
#define BOARD_SIZE 13
#define BOARD_BEGGINING_X 1
#define BOARD_BEGGINING_Y 3
#define BOARD_GAP_SIZE_X 4
#define BOARD_GAP_SIZE_Y 2

#define TEXT_COLOR LIGHTMAGENTA
#define BACKGROUND_COLOR DARKGRAY

#define ARROW_DOWN 0x48
#define ARROW_UP 0x50
#define ARROW_LEFT 0x4b
#define ARROW_RIGHT 0x4d
#define CURSOR '0'

void draw_board(int start_x, int start_y, int size) {
	textcolor(TEXT_COLOR);
	for (int y = start_y; y <= size * BOARD_GAP_SIZE_Y + start_y; y++)
	{
		if ((y-start_y+1) % BOARD_GAP_SIZE_Y) {
			for (int x = start_x; x <= size * BOARD_GAP_SIZE_X + start_x; x++) {
				gotoxy(x, y);
				if ((x - start_x) % BOARD_GAP_SIZE_X == 0) {
					putch('+');
				}
				else {
					putch('-');
				}
			}
		}
		else {
			for (int x = start_x; x <= size * BOARD_GAP_SIZE_X + start_x; x++) {
				gotoxy(x, y);
				if ((x - start_x) % BOARD_GAP_SIZE_X == 0) {
					putch('|');
				}
			}
		}
	}
}

void draw_info(int start_x, int start_y) {
	textcolor(TEXT_COLOR);
	gotoxy(start_x, start_y);
	cputs("q       = exit");
}

void clear_screen() {
	textbackground(DARKGRAY);
	clrscr();
}

void draw_cursor(int x, int y, int color) {
	gotoxy(x, y);
	textcolor(color);
	textbackground(BACKGROUND_COLOR);
	putch(CURSOR);
}

int main() {
	int zn = 0, x = BOARD_BEGGINING_X, y = BOARD_BEGGINING_Y, current_player = 0;
	char txt[32];
#ifndef __cplusplus
	Conio2_Init();
#endif
	settitle("Jerzy, Szyjut, 193064");
	_setcursortype(_NOCURSOR);
	do {
		int player_color;
		if (current_player % 2 == 0) {
			player_color = WHITE;
		}
		else {
			player_color = BLACK;
		}
		clear_screen();
		draw_info(1, 1);
		draw_board(BOARD_BEGGINING_X, BOARD_BEGGINING_Y, BOARD_SIZE);
		draw_cursor(x, y, player_color);
		current_player++;
		current_player = current_player % 2;
		

		zn = getch();
		if (zn == 0) {
			zn = getch();
			if (zn == ARROW_DOWN)
			{
				if (y > BOARD_BEGGINING_Y) {
					y-=BOARD_GAP_SIZE_Y;
				}
			}
			else if (zn == ARROW_UP)
			{
				if (y < BOARD_SIZE * BOARD_GAP_SIZE_Y + BOARD_BEGGINING_Y) {
					y+=BOARD_GAP_SIZE_Y;
				}
			}
			else if (zn == ARROW_LEFT)
			{
				if (x > BOARD_BEGGINING_X) {
					x-=BOARD_GAP_SIZE_X;
				}
			}
			else if (zn == ARROW_RIGHT) {
				if (x < BOARD_SIZE * BOARD_GAP_SIZE_X + BOARD_BEGGINING_X) {
					x+=BOARD_GAP_SIZE_X;
				}
			};
		}
	} while (zn != 'q');

	_setcursortype(_NORMALCURSOR);
	return 0;
}
