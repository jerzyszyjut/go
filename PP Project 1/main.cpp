#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include"conio2.h"
#include<string.h>

#define BOARD_GAP_SIZE_X 2
#define BOARD_GAP_SIZE_Y 1
#define BOARD_BEGGINING_X 1
#define BOARD_BEGGINING_Y 1
#define INFO_BEGGINING_X 1
#define INFO_BEGGINING_Y 1
#define BORDER_WIDTH 1

#define RIGHT 1
#define LEFT 0

#define LEGEND_SIDE LEFT

#define STARTING_WHITE_SCORE 6.5
#define STARTING_BLACK_SCORE 0.0

#define WHITE_PLAYER 1
#define BLACK_PLAYER -1

#define TEXT_COLOR BLUE
#define BACKGROUND_COLOR DARKGRAY
#define BORDER_COLOR YELLOW
#define WHITE_PLAYER_COLOR WHITE
#define BLACK_PLAYER_COLOR BLACK

#define ARROW_DOWN 0x48
#define ARROW_UP 0x50
#define ARROW_LEFT 0x4b
#define ARROW_RIGHT 0x4d
#define ENTER 0xd
#define ESCAPE 0x1b

#define CURSOR 'X'
#define STONE 'O'
#define CUT '+'

typedef struct coordinates {
	int x;
	int y;
} coordinates_t;

typedef struct score {
	float white_player;
	float black_player;
} score_t;

typedef struct board {
	char** fields;
	int size;
} board_t;

typedef struct game {
	coordinates_t* cursor_position;
	coordinates_t* draw_board_position;
	board_t* board;
	board_t* previous_board;
	score_t* score;
	char current_player;
} game_t;

/* General utilities functions */

void input_string(char* target, int limit) {
	int i = 0;
	char temp;
	do {
		temp = getche();
		target[i] = temp;
		i++;
	} while (temp != '\n' && temp != '\r' && i<limit);
	target[i-1] = '\0';
}

int string_to_int(char* string) {
	int result = 0;
	for (int i = 0; string[i] != '\0'; i++) {
		if (string[i] < '0' || string[i] > '9') {
			return result;
		}
		result *= 10;
		result += (string[i] - '0');
	}
	return result;
}

/* Game utilities functions */

coordinates_t* get_cursor_position_from_board_position(coordinates_t cursor_board_position, coordinates_t board_beggining) {
	coordinates_t* cursor_screen_position = new coordinates_t;
	cursor_screen_position->x = BOARD_GAP_SIZE_X * (cursor_board_position.x) + board_beggining.x + BORDER_WIDTH;
	cursor_screen_position->y = BOARD_GAP_SIZE_Y * (cursor_board_position.y) + board_beggining.y + BORDER_WIDTH;
	return cursor_screen_position;
}

int get_max_board_size(game_t* game, int info_width) {
	text_info* screen_info = new text_info;
	gettextinfo(screen_info);

	coordinates_t current_displayed_size = { (game->board->size * BOARD_GAP_SIZE_X) + 2 , (game->board->size * BOARD_GAP_SIZE_Y) + 2 };
	coordinates_t displayed_size_limits;
	if (current_displayed_size.x >= screen_info->screenwidth - info_width) {
		displayed_size_limits.x = (screen_info->screenwidth - info_width - 2) / BOARD_GAP_SIZE_X;
	}
	else {
		displayed_size_limits.x = game->board->size;
	}

	if (current_displayed_size.y >= screen_info->screenheight) {
		displayed_size_limits.y = (screen_info->screenheight - 2) / BOARD_GAP_SIZE_Y;
	}
	else {
		displayed_size_limits.y = game->board->size;
	}
	delete screen_info;

	if (displayed_size_limits.x > displayed_size_limits.y) {
		return displayed_size_limits.y;
	}
	else {
		return displayed_size_limits.x;
	}
}

int get_displayed_board_width(game_t* game, int info_width) {
	text_info *screen_info = new text_info;
	gettextinfo(screen_info);

	coordinates_t current_displayed_size = { (game->board->size * BOARD_GAP_SIZE_X) + 2 , (game->board->size * BOARD_GAP_SIZE_Y) + 2 };
	coordinates_t displayed_size_limits;
	if (current_displayed_size.x >= screen_info->screenwidth - info_width) {
		displayed_size_limits.x = screen_info->screenwidth - info_width;
	}
	else {
		displayed_size_limits.x = game->board->size * BOARD_GAP_SIZE_X + 2;
	}
	delete screen_info;
	return displayed_size_limits.x;
}

board_t* initialize_board(int size) {
	board_t* board = new board_t;
	board->size = size;

	char** fields = new char* [board->size];
	for (int i = 0; i < board->size; i++) {
		fields[i] = new char[board->size];
	}

	for (int i = 0; i < board->size; i++) {
		for (int j = 0; j < board->size; j++) {
			fields[i][j] = 0;
		}
	}

	board->fields = fields;
	return board;
}

void free_board_memory(board_t* board) {
	if (board != NULL) {
		for (int i = 0; i < board->size; i++) {
			delete board->fields[i];
		}
		delete board->fields;
	}
}

game_t* initialize_game(int board_size) {
	game_t* game = new game_t;
	game->cursor_position = new coordinates_t;
	game->cursor_position->x = 0;
	game->cursor_position->y = 0;
	game->draw_board_position = new coordinates_t;
	game->draw_board_position->x = 0;
	game->draw_board_position->y = 0;
	game->score = new score_t;
	game->score->white_player = STARTING_WHITE_SCORE;
	game->score->black_player = STARTING_BLACK_SCORE;
	game->board = initialize_board(board_size);
	game->previous_board = NULL;
	game->current_player = BLACK_PLAYER;
	return game;
}

void free_game_memory(game_t* game) {
	if (game != NULL) {
		free_board_memory(game->board);
		delete game->board;
		free_board_memory(game->previous_board);
		delete game->previous_board;
		delete game->score;
		delete game->cursor_position;
		delete game->draw_board_position;
	}
}

board_t* copy_board(board_t* board) {
	board_t* new_board = new board_t;
	new_board->size = board->size;

	char** fields = new char* [new_board->size];
	for (int i = 0; i < new_board->size; i++) {
		fields[i] = new char[new_board->size];
	}

	for (int i = 0; i < new_board->size; i++) {
		for (int j = 0; j < new_board->size; j++) {
			fields[i][j] = board->fields[i][j];
		}
	}

	new_board->fields = fields;
	return new_board;
}

int compare_boards(board_t* board_1, board_t* board_2) {
	for (int i = 0; i < board_1->size; i++) {
		for (int j = 0; j < board_1->size; j++) {
			if (board_1->fields[i][j] != board_2->fields[i][j]) {
				return 0;
			}
		}
	}
	return 1;
}

int get_board_size(coordinates_t* message_position) {
	char board_size[10];
	gotoxy(message_position->x, message_position->y++);
	cputs("Choose board size:\n");
	gotoxy(message_position->x, message_position->y++);
	cputs("1 - 9x9\n");
	gotoxy(message_position->x, message_position->y++);
	cputs("2 - 13x13\n");
	gotoxy(message_position->x, message_position->y++);
	cputs("3 - 19x19\n");
	gotoxy(message_position->x, message_position->y++);
	cputs("4 - Custom\n");
	gotoxy(message_position->x, message_position->y++);
	switch (getch())
	{
	case '1':
		return 9;
	case '2':
		return 13;
	case '3':
		return 19;
	case '4':
		cputs("Input board size: ");
		input_string(board_size, 10);
		return string_to_int(board_size);
	default:
		return 9;
		break;
	}
}

/* UI functions */

void clear_screen() {
	textbackground(DARKGRAY);
	clrscr();
}

int draw_info(coordinates_t *start_coordinates, game_t *game) {
	char buffer[48];
	textcolor(TEXT_COLOR);
	int max_width;
	gotoxy(start_coordinates->x, start_coordinates->y++);
	cputs("Jerzy, Szyjut, 193064");
	gotoxy(start_coordinates->x, start_coordinates->y++);
	cputs("arrows  = cursor movement");
	gotoxy(start_coordinates->x, start_coordinates->y++);
	cputs("q       = exit");
	gotoxy(start_coordinates->x, start_coordinates->y++);	
	cputs("n       = start new game");
	gotoxy(start_coordinates->x, start_coordinates->y++);
	cputs("enter   = confirm choice and end player's turn");
	max_width = 47;
	gotoxy(start_coordinates->x, start_coordinates->y++);
	cputs("esc     = cancel current action");
	gotoxy(start_coordinates->x, start_coordinates->y++);
	cputs("i       = place a stone on the board");
	gotoxy(start_coordinates->x, start_coordinates->y++);
	cputs("s       = save the game state");
	gotoxy(start_coordinates->x, start_coordinates->y++);
	cputs("l       = load the game state");
	gotoxy(start_coordinates->x, start_coordinates->y++);
	gotoxy(start_coordinates->x, start_coordinates->y++);
	sprintf(buffer, "White player score: %.1f", game->score->white_player);
	if (strlen(buffer) > max_width) {
		max_width = strlen(buffer);
	}
	puts(buffer);
	gotoxy(start_coordinates->x, start_coordinates->y++);
	sprintf(buffer, "Black player score: %.1f", game->score->black_player);
	if (strlen(buffer) > max_width) {
		max_width = strlen(buffer);
	}
	puts(buffer);
	gotoxy(start_coordinates->x, start_coordinates->y++);
	sprintf(buffer, "Current coordinates: (%d, %d)", game->cursor_position->x, game->cursor_position->y);
	if (strlen(buffer) > max_width) {
		max_width = strlen(buffer);
	}
	puts(buffer);
	return max_width;
}

void draw_board_and_cursor(game_t* game, int info_width) {
	info_width++;
	coordinates_t start_screen;
	if (LEGEND_SIDE == LEFT) {
		start_screen = { info_width, BOARD_BEGGINING_Y };
	}
	else if (LEGEND_SIDE == RIGHT) {
		start_screen = { BOARD_BEGGINING_X, BOARD_BEGGINING_Y };
	}

	int displayed_size = get_max_board_size(game, info_width);

	if (game->cursor_position->x - game->draw_board_position->x >= displayed_size) {
		game->draw_board_position->x++;
	}
	else if (game->cursor_position->x - game->draw_board_position->x < 0) {
		game->draw_board_position->x--;
	}
	else if (game->cursor_position->y - game->draw_board_position->y >= displayed_size) {
		game->draw_board_position->y++;
	}
	else if (game->cursor_position->y - game->draw_board_position->y < 0) {
		game->draw_board_position->y--;
	}

	coordinates_t limits;
	limits.x = start_screen.x + ((displayed_size - 1)*BOARD_GAP_SIZE_X) + 2 * BORDER_WIDTH;
	limits.y = start_screen.y + ((displayed_size - 1)*BOARD_GAP_SIZE_Y) + 2 * BORDER_WIDTH;

	for (int x = start_screen.x; x <= limits.x; x++) {
		for (int y = start_screen.y; y <= limits.y; y++) {
			gotoxy(x, y);
			if (x == start_screen.x || x == limits.x || y == start_screen.y || y == limits.y) {
				textcolor(BORDER_COLOR);
				putch('*');
			}
			else {
				if (((x - start_screen.x - 1) % BOARD_GAP_SIZE_X == 0) && ((y - start_screen.y - 1) % BOARD_GAP_SIZE_Y == 0)) {
					coordinates_t board_coordinates = { ((x - start_screen.x - 1) / BOARD_GAP_SIZE_X), ((y - start_screen.y - 1) / BOARD_GAP_SIZE_Y) };
					switch (game->board->fields[board_coordinates.x + game->draw_board_position->x][board_coordinates.y + game->draw_board_position->y])
					{
					case BLACK_PLAYER:
						textcolor(BLACK_PLAYER_COLOR);
						putch(STONE);
						break;
					case WHITE_PLAYER:
						textcolor(WHITE_PLAYER_COLOR);
						putch(STONE);
						break;
					default:
						textcolor(TEXT_COLOR);
						putch(CUT);
						break;
					}
					textcolor(TEXT_COLOR);
				}
				else if ((BOARD_GAP_SIZE_X > 1 && BOARD_GAP_SIZE_Y > 1) && (x - start_screen.x) % BOARD_GAP_SIZE_X == 0) {
					putch('|');
				}
				else if ((BOARD_GAP_SIZE_X > 1 && BOARD_GAP_SIZE_Y > 1) && (y - start_screen.y) % BOARD_GAP_SIZE_Y == 0) {
					putch('-');
				}
			}
		}
	}

	coordinates_t limited_cursor_position = { game->cursor_position->x - game->draw_board_position->x, game->cursor_position->y - game->draw_board_position->y };
	coordinates_t* displayed_cursor_position = get_cursor_position_from_board_position(limited_cursor_position, start_screen);
	gotoxy(displayed_cursor_position->x, displayed_cursor_position->y);
	if (game->current_player == WHITE_PLAYER) {
		textcolor(WHITE_PLAYER_COLOR);
	}
	else if (game->current_player == BLACK_PLAYER) {
		textcolor(BLACK_PLAYER_COLOR);
	}
	textbackground(BACKGROUND_COLOR);
	putch(CURSOR);
	delete displayed_cursor_position;
}

int display_confirmation_dialog(char* message, coordinates_t* dialog_position) {
	gotoxy(dialog_position->x, dialog_position->y++);
	textcolor(RED);
	cputs(message);
	gotoxy(dialog_position->x, dialog_position->y++);
	cputs("ENTER - Yes");
	gotoxy(dialog_position->x, dialog_position->y++);
	cputs("ESCAPE - No");
	char answer = 0;
	do {
		answer = getch();
		if (answer == ENTER) {
			return 1;
		}
		if (answer == ESCAPE) {
			return 0;
		}
	} while ((answer != ENTER) && (answer != ESCAPE));
}

void display_info_dialog(char* message, coordinates_t* dialog_position) {
	gotoxy(dialog_position->x, dialog_position->y++);
	textcolor(RED);
	cputs(message);
	gotoxy(dialog_position->x, dialog_position->y++);
	cputs("Press any button to continue");
	getch();
}

/* Game mechanics functions */

game *start_new_game(coordinates_t* message_position) {
	int board_size = get_board_size(message_position);
	return initialize_game(board_size);
}

int _count_group_liberties(board_t* board, coordinates_t* starting_position) {
	int liberties = 0;
	char current_color = board->fields[starting_position->x][starting_position->y];
	if (current_color == 0) {
		return liberties;
	}
	if (current_color == 2 || current_color == 1) {
		*(&(board->fields[starting_position->x][starting_position->y])) = 2;
	}
	else if (current_color == -2 || current_color == -1) {
		*(&(board->fields[starting_position->x][starting_position->y])) = -2;
	}
	if (starting_position->x > 0) {
		if (board->fields[starting_position->x - 1][starting_position->y] == current_color) {
			coordinates_t new_starting_position = { starting_position->x - 1, starting_position->y };
			liberties += _count_group_liberties(board, &new_starting_position);
		}
		else if (board->fields[starting_position->x - 1][starting_position->y] == 0) {
			liberties += 1;
		}
	}
	if (starting_position->x < (board->size - 1)) {
		if (board->fields[starting_position->x + 1][starting_position->y] == current_color) {
			coordinates_t new_starting_position = { starting_position->x + 1, starting_position->y };
			liberties += _count_group_liberties(board, &new_starting_position);
		}
		else if (board->fields[starting_position->x + 1][starting_position->y] == 0) {
			liberties += 1;
		}
	}
	if (starting_position->y > 0) {
		if (board->fields[starting_position->x][starting_position->y - 1] == current_color) {
			coordinates_t new_starting_position = { starting_position->x, starting_position->y - 1 };
			liberties += _count_group_liberties(board, &new_starting_position);
		}
		else if (board->fields[starting_position->x][starting_position->y - 1] == 0) {
			liberties += 1;
		}
	}
	if (starting_position->y < (board->size - 1)) {
		if (board->fields[starting_position->x][starting_position->y + 1] == current_color) {
			coordinates_t new_starting_position = { starting_position->x, starting_position->y + 1 };
			liberties += _count_group_liberties(board, &new_starting_position);
		}
		else if (board->fields[starting_position->x][starting_position->y + 1] == 0) {
			liberties += 1;
		}
	}
	return liberties;
}

int count_group_liberties(board_t* board, coordinates_t* starting_position) {
	board_t* new_board = copy_board(board);
	int liberties = _count_group_liberties(new_board, starting_position);
	free_board_memory(new_board);
	delete new_board;
	return liberties;
}

int remove_group(board_t *board, coordinates_t *starting_position) {
	int removed_stones = 0;
	char current_color = board->fields[starting_position->x][starting_position->y];
	if (current_color == 0) {
		return 0;
	}
	*(&(board->fields[starting_position->x][starting_position->y])) = 0;
	removed_stones++;
	if (starting_position->x > 0) {
		if (board->fields[starting_position->x - 1][starting_position->y] == current_color) {
			coordinates_t new_starting_position = { starting_position->x - 1, starting_position->y };
			removed_stones += remove_group(board, &new_starting_position);
		}
	}
	if (starting_position->x < (board->size - 1)) {
		if (board->fields[starting_position->x + 1][starting_position->y] == current_color) {
			coordinates_t new_starting_position = { starting_position->x + 1, starting_position->y };
			removed_stones += remove_group(board, &new_starting_position);
		}
	}
	if (starting_position->y > 0) {
		if (board->fields[starting_position->x][starting_position->y - 1] == current_color) {
			coordinates_t new_starting_position = { starting_position->x, starting_position->y - 1 };
			removed_stones += remove_group(board, &new_starting_position);
		}
	}
	if (starting_position->y < (board->size - 1)) {
		if (board->fields[starting_position->x][starting_position->y + 1] == current_color) {
			coordinates_t new_starting_position = { starting_position->x, starting_position->y + 1 };
			removed_stones += remove_group(board, &new_starting_position);
		}
	}
	return removed_stones;
}

int get_liberties_after_placing_stone(game_t* game, coordinates_t *cursor_position) {
	board_t* new_board = copy_board(game->board);
	new_board->fields[game->cursor_position->x][game->cursor_position->y] = game->current_player;
	int liberties = _count_group_liberties(new_board, cursor_position);
	free_board_memory(new_board);
	delete new_board;
	return liberties;
}

int can_stone_be_placed_here(game_t *game) {
	int liberties = get_liberties_after_placing_stone(game, game->cursor_position);
	char other_player = game->current_player * -1;

	if (liberties > 0) {
		return 1;
	}
	else {
		if (game->cursor_position->x > 0) {
			if (game->board->fields[game->cursor_position->x - 1][game->cursor_position->y] == other_player) {
				coordinates_t neighbour_liberties_position = { game->cursor_position->x - 1 , game->cursor_position->y };
				int neighbour_liberties = get_liberties_after_placing_stone(game, &neighbour_liberties_position);
				if (neighbour_liberties == 0) {
					return 1;
				}
			}
		}
		if (game->cursor_position->x < (game->board->size - 1)) {
			if (game->board->fields[game->cursor_position->x + 1][game->cursor_position->y] == other_player) {
				coordinates_t neighbour_liberties_position = { game->cursor_position->x + 1 , game->cursor_position->y };
				int neighbour_liberties = get_liberties_after_placing_stone(game, &neighbour_liberties_position);
				if (neighbour_liberties == 0) {
					return 1;
				}
			}
		}
		if (game->cursor_position->y > 0) {
			if (game->board->fields[game->cursor_position->x][game->cursor_position->y - 1] == other_player) {
				coordinates_t neighbour_liberties_position = { game->cursor_position->x, game->cursor_position->y - 1 };
				int neighbour_liberties = get_liberties_after_placing_stone(game, &neighbour_liberties_position);
				if (neighbour_liberties == 0) {
					return 1;
				}
			}
		}
		if (game->cursor_position->y < (game->board->size - 1)) {
			if (game->board->fields[game->cursor_position->x][game->cursor_position->y + 1] == other_player) {
				coordinates_t neighbour_liberties_position = { game->cursor_position->x , game->cursor_position->y + 1 };
				int neighbour_liberties = get_liberties_after_placing_stone(game, &neighbour_liberties_position);
				if (neighbour_liberties == 0) {
					return 1;
				}
			}
		}
	}
	return 0;
}

void remove_neighbours_if_possible(coordinates_t *cursor_position, board_t *board, char current_player, score_t *score) {
	int score_points = 0;
	if (cursor_position->x > 0) {
		if (board->fields[cursor_position->x - 1][cursor_position->y] != current_player) {
			coordinates_t neighbour_liberties_position = { cursor_position->x - 1 , cursor_position->y };
			int neighbour_liberties = count_group_liberties(board, &neighbour_liberties_position);
			if (neighbour_liberties == 0) {
				score_points += remove_group(board, &neighbour_liberties_position);
			}
		}
	}
	if (cursor_position->x < (board->size - 1)) {
		if (board->fields[cursor_position->x + 1][cursor_position->y] != current_player) {
			coordinates_t neighbour_liberties_position = { cursor_position->x + 1 , cursor_position->y };
			int neighbour_liberties = count_group_liberties(board, &neighbour_liberties_position);
			if (neighbour_liberties == 0) {
				score_points += remove_group(board, &neighbour_liberties_position);
			}
		}
	}
	if (cursor_position->y > 0) {
		if (board->fields[cursor_position->x][cursor_position->y - 1] != current_player) {
			coordinates_t neighbour_liberties_position = { cursor_position->x, cursor_position->y - 1 };
			int neighbour_liberties = count_group_liberties(board, &neighbour_liberties_position);
			if (neighbour_liberties == 0) {
				score_points += remove_group(board, &neighbour_liberties_position);
			}
		}
	}
	if (cursor_position->y < (board->size - 1)) {
		if (board->fields[cursor_position->x][cursor_position->y + 1] != current_player) {
			coordinates_t neighbour_liberties_position = { cursor_position->x , cursor_position->y + 1 };
			int neighbour_liberties = count_group_liberties(board, &neighbour_liberties_position);
			if (neighbour_liberties == 0) {
				score_points += remove_group(board, &neighbour_liberties_position);
			}
		}
	}
	if (score != NULL) {
		if (current_player == WHITE_PLAYER) {
			score->white_player += (float)score_points;
		}
		else if (current_player == BLACK_PLAYER) {
			score->black_player += (float)score_points;
		}
	}
}

int check_ko_rule (game_t *game) {
	int result = 0;
	board_t* next_board = copy_board(game->board);
	next_board->fields[game->cursor_position->x][game->cursor_position->y] = game->current_player;
	remove_neighbours_if_possible(game->cursor_position, next_board, game->current_player, NULL);
	if (game->previous_board == NULL || compare_boards(next_board, game->previous_board) != 1) {
		result = 1;
	}
	free_board_memory(next_board);
	delete next_board;
	return result;
}

void place_stone(game_t *game, coordinates_t *dialog_position) {
	if (game->board->fields[game->cursor_position->x][game->cursor_position->y] == 0 && can_stone_be_placed_here(game) == 1 && check_ko_rule(game) == 1) {
			char message[] = "Do you want to place stone on this coordinates?";
			int confirmation = display_confirmation_dialog(message, dialog_position);
			if (confirmation == 1) {
				if (game->previous_board != NULL) {
					free_board_memory(game->previous_board);
				}
				game->previous_board = copy_board(game->board);
				game->board->fields[game->cursor_position->x][game->cursor_position->y] = game->current_player;
				remove_neighbours_if_possible(game->cursor_position, game->board, game->current_player, game->score);
				game->current_player = -1 * game->current_player;
			}
	}
	else {
		char message[] = "You cannot place stone here";
		display_info_dialog(message, dialog_position);
	}
}

void save_game_state(game_t* game, char *filename) {
	FILE* file = fopen(filename, "wb");
	if (file != NULL) {
		fwrite(&(game->current_player), sizeof(game->current_player), 1, file);
		fwrite(&(game->cursor_position->x), sizeof(game->cursor_position->x), 1, file);
		fwrite(&(game->cursor_position->y), sizeof(game->cursor_position->y), 1, file);
		fwrite(&(game->draw_board_position->x), sizeof(game->draw_board_position->x), 1, file);
		fwrite(&(game->draw_board_position->y), sizeof(game->draw_board_position->y), 1, file);
		fwrite(&(game->score->white_player), sizeof(game->score->white_player), 1, file);
		fwrite(&(game->score->black_player), sizeof(game->score->black_player), 1, file);
		fwrite(&(game->board->size), sizeof(game->board->size), 1, file);
		for (int x = 0; x < game->board->size; x++) {
			for (int y = 0; y < game->board->size; y++) {
				fwrite(&(game->board->fields[x][y]), sizeof(char), 1, file);
			}
		}		
		if (game->previous_board != NULL) {
			fwrite(&(game->previous_board->size), sizeof(game->previous_board->size), 1, file);
			for (int x = 0; x < game->previous_board->size; x++) {
				for (int y = 0; y < game->previous_board->size; y++) {
					fwrite(&(game->previous_board->fields[x][y]), sizeof(char), 1, file);
				}
			}
		}
		else{
			int minus_one = -1;
			fwrite(&minus_one, sizeof(minus_one), 1, file);
		}
		fclose(file);
	}
}

game_t* load_game_state(char *filename) {
	FILE* file = fopen(filename, "rb");
	if (file != NULL) {
		game_t* game = new game_t;
		fread(&(game->current_player), sizeof(game->current_player), 1, file);
		game->cursor_position = new coordinates_t;
		fread(&(game->cursor_position->x), sizeof(game->cursor_position->x), 1, file);
		fread(&(game->cursor_position->y), sizeof(game->cursor_position->y), 1, file);
		game->draw_board_position = new coordinates_t;
		fread(&(game->draw_board_position->x), sizeof(game->draw_board_position->x), 1, file);
		fread(&(game->draw_board_position->y), sizeof(game->draw_board_position->y), 1, file);
		game->score = new score_t;
		fread(&(game->score->white_player), sizeof(game->score->white_player), 1, file);
		fread(&(game->score->black_player), sizeof(game->score->black_player), 1, file);
		game->board = new board_t;
		fread(&(game->board->size), sizeof(game->board->size), 1, file);
		game->board->fields = new char*[game->board->size];
		for (int x = 0; x < game->board->size; x++) {
			game->board->fields[x] = new char[game->board->size];
			for (int y = 0; y < game->board->size; y++) {
				fread(&(game->board->fields[x][y]), sizeof(char), 1, file);
			}
		}
		game->previous_board = new board_t;
		fread(&(game->previous_board->size), sizeof(game->previous_board->size), 1, file);
		if (game->previous_board->size != -1) {
			game->previous_board->fields = new char* [game->previous_board->size];
			for (int x = 0; x < game->previous_board->size; x++) {
				game->previous_board->fields[x] = new char[game->previous_board->size];
				for (int y = 0; y < game->previous_board->size; y++) {
					fread(&(game->previous_board->fields[x][y]), sizeof(char), 1, file);
				}
			}
		}
		else {
			delete game->previous_board;
			game->previous_board = NULL;
		}
		fclose(file);
		return game;
	}
	return NULL;
}

void handle_movement(coordinates_t* cursor_board_position, int board_size) {
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

game_t *handle_input(game_t *game, int *zn, coordinates_t *info_start_coordinates) {
	game_t* new_game;
	char filename[64];
	switch (*zn = getch())
	{
	case 0:
		handle_movement(game->cursor_position, game->board->size);
		break;
	case 'i':
		place_stone(game, info_start_coordinates);
		break;
	case 'n':
		free_game_memory(game);
		game = start_new_game(info_start_coordinates);
		break;
	case 's':
		gotoxy(info_start_coordinates->x, info_start_coordinates->y++);
		cputs("Specify save filename: \n");
		gotoxy(info_start_coordinates->x, info_start_coordinates->y++);
		input_string(filename, 64);
		save_game_state(game, filename);
		break;
	case 'l':
		gotoxy(info_start_coordinates->x, info_start_coordinates->y++);
		cputs("Specify save filename: \n");
		gotoxy(info_start_coordinates->x, info_start_coordinates->y++);
		input_string(filename, 64);
		new_game = load_game_state(filename);
		if (new_game != NULL) {
			free_game_memory(game);
			game = new_game;
		}
		break;
	default:
		break;
	}
	return game;
}

int main() {
	int zn = 0;
	int info_width = 50;
	coordinates_t cursor_board_position = { 0, 0 };
	coordinates_t display_info_start_coordinates = { INFO_BEGGINING_X, INFO_BEGGINING_Y };
	coordinates_t display_board_start_coordinates = { BOARD_BEGGINING_X, BOARD_BEGGINING_Y };
	coordinates_t scroll_board_start_coordinates = { 0, 0 };

#ifndef __cplusplus
	Conio2_Init();
#endif

	settitle("Jerzy, Szyjut, 193064"); 
	_setcursortype(_NOCURSOR);
	game_t* game = start_new_game(&cursor_board_position);
	do {
		clear_screen();
		
		if (LEGEND_SIDE == LEFT) {
			display_info_start_coordinates = { INFO_BEGGINING_X, INFO_BEGGINING_Y };
		}
		else if (LEGEND_SIDE == RIGHT) {
			display_info_start_coordinates = { get_displayed_board_width(game, info_width) + 1, INFO_BEGGINING_Y};
		}

		info_width = draw_info(&display_info_start_coordinates, game);

		draw_board_and_cursor(game, info_width);
		
		game = handle_input(game, &zn, &display_info_start_coordinates);
	} while (zn != 'q');

	_setcursortype(_NORMALCURSOR);
	free_game_memory(game);
	delete game;
	return 0;
}
