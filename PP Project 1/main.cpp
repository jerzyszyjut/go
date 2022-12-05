#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include"conio2.h"
#include<math.h>

#define BOARD_SIZE 19
#define BOARD_BEGGINING_X 50
#define BOARD_BEGGINING_Y 1
#define BOARD_GAP_SIZE_X 2
#define BOARD_GAP_SIZE_Y 1
#define BOARD_SIZE_LIMIT 20

#define TEXT_COLOR BLUE
#define BACKGROUND_COLOR DARKGRAY
#define BORDER_COLOR YELLOW

#define ARROW_DOWN 0x48
#define ARROW_UP 0x50
#define ARROW_LEFT 0x4b
#define ARROW_RIGHT 0x4d
#define CURSOR 'X'

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

char* int_to_char(int number) {
	int number_length = (int)lround(floor(log10(number))) + 1;
	char *converted_number = new char[number_length + 1];
	for (int i = 1; i <= number_length; i++) {
		int modulo = number % (10 * i);
		converted_number[number_length - i] = (char)('0' + (char)modulo);
		number -= modulo;
		number /= 10;
	}
	converted_number[number_length] = '\0';
	return converted_number;
}

void draw_info(coordinates_t start_coordinates, coordinates_t *cursor_board_position) {
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
	gotoxy(start_coordinates.x, start_coordinates.y+=2);
	cputs("Current coordinates: (");
	char* converted_position_x = int_to_char(cursor_board_position->x);
	char* converted_position_y = int_to_char(cursor_board_position->y);
	cputs(converted_position_x);
	putch(',');
	cputs(converted_position_y);
	putch(')');
	delete converted_position_x;
	delete converted_position_y;
}

void clear_screen() {
	textbackground(DARKGRAY);
	clrscr();
}

coordinates_t *get_cursor_position_from_board_position(coordinates_t cursor_board_position) {
	coordinates_t* cursor_screen_position = new coordinates_t;
	cursor_screen_position->x = BOARD_GAP_SIZE_X * (cursor_board_position.x + 1) + BOARD_BEGGINING_X;
	cursor_screen_position->y = BOARD_GAP_SIZE_Y * (cursor_board_position.y + 1) + BOARD_BEGGINING_Y;
	return cursor_screen_position;
}

void draw_cursor(coordinates_t board_cursor_position, int color) {
	coordinates_t *cursor_position = get_cursor_position_from_board_position(board_cursor_position);
	gotoxy(cursor_position->x, cursor_position->y);
	textcolor(color);
	textbackground(BACKGROUND_COLOR);
	putch(CURSOR);
	delete cursor_position;
}

void handle_movement(coordinates_t *cursor_board_position, int board_size) {
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
		if (cursor_board_position->y < board_size - 1) {
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
		if (cursor_board_position->x < board_size - 1) {
			cursor_board_position->x++;
		}
	};
}

void draw_board(board_t *board, coordinates_t start) {
	int displayed_size = board->size;
	coordinates_t board_limits;
	board_limits.x = (displayed_size+ 1)*BOARD_GAP_SIZE_X + start.x;
	board_limits.y = (displayed_size+ 1)*BOARD_GAP_SIZE_Y + start.y;
	coordinates_t board_coordinates;
	for (int y = start.y; y <= board_limits.y; y++) {
		for (int x = start.x; x <= board_limits.x; x++) {
			gotoxy(x, y);
			board_coordinates = { ((x - start.x - 1) / BOARD_GAP_SIZE_X), ((y - start.y - 1) / BOARD_GAP_SIZE_Y) };
			if (x == start.x || x == board_limits.x || y == start.y || y == board_limits.y ) {
				textcolor(BORDER_COLOR);
				putch('*');
			}
			else {
				if (((x - start.x) % BOARD_GAP_SIZE_X == 0) && ((y - start.y) % BOARD_GAP_SIZE_Y == 0)) {
					switch (board->fields[board_coordinates.x][board_coordinates.y])
					{
					case -1:
						textcolor(BLACK);
						putch('O');
						break;
					case 1:
						textcolor(WHITE);
						putch('O');
						break;
					default:
						textcolor(TEXT_COLOR);
						putch('+');
						break;
					}
					textcolor(TEXT_COLOR);
				}
				else if ((BOARD_GAP_SIZE_X > 1 && BOARD_GAP_SIZE_Y > 1) && (x - start.x) % BOARD_GAP_SIZE_X == 0) {
					putch('|');
				}
				else if ((BOARD_GAP_SIZE_X > 1 && BOARD_GAP_SIZE_Y > 1) && (y - start.y) % BOARD_GAP_SIZE_Y == 0) {
					putch('-');
				}
			}
		}
	}
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

game_t *initialize_game() {
	game_t *game = new game_t;
	game->cursor_position = new coordinates_t;
	game->cursor_position->x = 0;
	game->cursor_position->y = 0;
	game->board = initialize_board(BOARD_SIZE);
	return game;
}

void free_game_memory(game_t* game) {
	for (int i = 0; i < game->board->size; i++) {
		delete game->board->fields[i];
	}
	delete game->board->fields;
	delete game->board;
	delete game->cursor_position;
}

int main() {
	int zn = 0;
	coordinates_t cursor_board_position = { 0, 0 };
	coordinates_t info_start_coordinates = { 1, 1 };
	game_t *game = initialize_game();

#ifndef __cplusplus
	Conio2_Init();
#endif

	settitle("Jerzy, Szyjut, 193064"); 
	_setcursortype(_NOCURSOR);
	do {
		clear_screen();
		
		draw_info(info_start_coordinates, game->cursor_position);

		draw_board(game->board, { BOARD_BEGGINING_X, BOARD_BEGGINING_Y });

		draw_cursor(*(game->cursor_position), WHITE);
		
		switch (zn = getch())
		{
			case 0:
				handle_movement(game->cursor_position, game->board->size);
			default:
				break;
		}
	} while (zn != 'q');

	_setcursortype(_NORMALCURSOR);
	free_game_memory(game);
	return 0;
}
