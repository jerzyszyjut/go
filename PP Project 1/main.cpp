#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include"conio2.h"

/* Comment: in the final program declare appropriate constants, e.g.,
   to eliminate from your program numerical values by replacing them
   with well defined identifiers */
#define BOARD_SIZE 13
#define BOARD_BEGGINING_X 50
#define BOARD_BEGGINING_Y 1
#define BOARD_GAP_SIZE_X 4
#define BOARD_GAP_SIZE_Y 2
#define BOARD_SIZE_LIMIT 10

#define TEXT_COLOR BLUE
#define BACKGROUND_COLOR DARKGRAY

#define ARROW_DOWN 0x48
#define ARROW_UP 0x50
#define ARROW_LEFT 0x4b
#define ARROW_RIGHT 0x4d
#define CURSOR 'O'

typedef struct coordinates {
	int x;
	int y;
} coordinates_t;

typedef struct board {
	char **fields;
	int size;
} board_t;

typedef struct game {
	coordinates_t *cursor_position;
	board_t *board;
} game_t;

void draw_info(coordinates_t start_coordinates) {
	textcolor(TEXT_COLOR);
	gotoxy(start_coordinates.x, start_coordinates.y++);
	cputs("Jerzy, Szyjut, 193064");
	gotoxy(start_coordinates.x, start_coordinates.y++);
	cputs("arrows  = cursor movement");
	gotoxy(start_coordinates.x, start_coordinates.y++);
	cputs("q       = exit");
	gotoxy(start_coordinates.x, start_coordinates.y++);	
	cputs("n       = start new game");
	gotoxy(start_coordinates.x, start_coordinates.y++);
	cputs("enter   = confirm choice and end player's turn");
	gotoxy(start_coordinates.x, start_coordinates.y++);
	cputs("esc     = cancel current action");
	gotoxy(start_coordinates.x, start_coordinates.y++);
	cputs("i       = place a stone on the board");
	gotoxy(start_coordinates.x, start_coordinates.y++);
	cputs("s       = save the game state");
	gotoxy(start_coordinates.x, start_coordinates.y++);
	cputs("l       = load the game state");
	gotoxy(start_coordinates.x, start_coordinates.y++);
	cputs("f       = finish the game");
}

void clear_screen() {
	textbackground(DARKGRAY);
	clrscr();
}

coordinates_t* get_cursor_position_from_board_position(coordinates_t cursor_board_position) {
	coordinates_t* cursor_screen_position = new coordinates_t;
	cursor_screen_position->x = BOARD_GAP_SIZE_X * cursor_board_position.x + BOARD_BEGGINING_X;
	cursor_screen_position->y = BOARD_GAP_SIZE_Y * cursor_board_position.y + BOARD_BEGGINING_Y;
	return cursor_screen_position;
}

void draw_cursor(coordinates_t board_cursor_position, int color) {
	coordinates_t *cursor_posiiton = get_cursor_position_from_board_position(board_cursor_position);
	gotoxy(cursor_posiiton->x, cursor_posiiton->y);
	textcolor(color);
	textbackground(BACKGROUND_COLOR);
	putch(CURSOR);
	delete cursor_posiiton;
}

void handle_movement(coordinates_t* cursor_board_position) {
	int zn;
	zn = getch();
	if (zn == ARROW_DOWN)
	{
		if (cursor_board_position->y > 0) {
			cursor_board_position->y--;
		}
	}
	else if (zn == ARROW_UP)
	{
		if (cursor_board_position->y < BOARD_SIZE) {
			cursor_board_position->y++;
		}
	}
	else if (zn == ARROW_LEFT)
	{
		if (cursor_board_position->x > 0) {
			cursor_board_position->x--;
		}
	}
	else if (zn == ARROW_RIGHT) {
		if (cursor_board_position->x < BOARD_SIZE) {
			cursor_board_position->x++;
		}
	};
}

board_t *initialize_board(int size) {
	board_t *board = new board_t;

	char** fields = new char*[size];
	for (int i = 0; i < size; i++) {
		fields[i] = new char[size];
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			fields[i][j] = 0;
		}
	}

	board->fields = fields;
	board->size = size;

	return board;
}

game_t* initialize_game() {
	game_t *game = new game_t;
	game->cursor_position = new coordinates_t;
	game->cursor_position->x = 0;
	game->cursor_position->y = 0;
	game->board = initialize_board(BOARD_SIZE);
	return game;
}

int main() {
	int zn = 0;
	coordinates_t cursor_board_position = { 0, 0 };
	coordinates_t info_start_coordinates = { 1, 1 };

#ifndef __cplusplus
	Conio2_Init();
#endif

	settitle("Jerzy, Szyjut, 193064");
	_setcursortype(_NOCURSOR);
	do {
		clear_screen();
		
		draw_info(info_start_coordinates);

		draw_cursor(cursor_board_position, WHITE);
		
		zn = getch();
		if (zn == 0) {
			handle_movement(&cursor_board_position);
		}
	} while (zn != 'q');

	_setcursortype(_NORMALCURSOR);
	return 0;
}
