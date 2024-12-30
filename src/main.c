#include <graphx.h>
#include <keypadc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>


#define TILE_SIZE 8


typedef struct cursor_t {
		int x;
		int y;

	} cursor_t;


int render(bool**, cursor_t*, bool*);
int update(bool**, cursor_t*, bool*);
int count_neighbours(bool**, int x, int y);


int main(void)
{
	/* Initialize graphics drawing */
	gfx_Begin();
	gfx_SetTransparentColor(0);
	gfx_SetDrawBuffer();
	gfx_FillScreen(0);

	// initialize a 2D array to store the game
	int rows = GFX_LCD_HEIGHT / TILE_SIZE;
	int columns = GFX_LCD_WIDTH / TILE_SIZE;

	bool **board = (bool **)malloc(rows * sizeof(bool *));
	for (int i = 0; i < rows; i++) {
		board[i] = (bool *)malloc(columns * sizeof(bool));
		for (int j = 0; j < columns; j++) {
			board[i][j] = false;
		}
	}

	// test glider gun

	//     123456789ABCDEF123456789ABCDEF123456
	//
	// 1   ....................................
	// 2   ........................O...........
	// 3   ......................O.O...........
	// 4   ............OO......OO............OO
	// 5   ...........O...O....OO............OO
	// 6   OO........O.....O...OO..............
	// 7   OO........O...O.OO....O.O...........
	// 8   ..........O.....O.......O...........
	// 9   ...........O...O....................
	// A   ............OO......................

	board[2][25] = true;

	board[3][23] = true;
	board[3][25] = true;

	board[4][13] = true;
	board[4][14] = true;
	board[4][21] = true;
	board[4][22] = true;
	board[4][35] = true;
	board[4][36] = true;

	board[5][12] = true;
	board[5][16] = true;
	board[5][21] = true;
	board[5][22] = true;
	board[5][35] = true;
	board[5][36] = true;

	board[6][1] = true;
	board[6][2] = true;
	board[6][11] = true;
	board[6][17] = true;
	board[6][21] = true;
	board[6][22] = true;

	board[7][1] = true;
	board[7][2] = true;
	board[7][11] = true;
	board[7][15] = true;
	board[7][17] = true;
	board[7][18] = true;
	board[7][23] = true;
	board[7][25] = true;

	board[8][11] = true;
	board[8][17] = true;
	board[8][25] = true;

	board[9][12] = true;
	board[9][16] = true;

	board[10][13] = true;
	board[10][14] = true;


	bool *paused = (bool *)malloc(sizeof(bool));
	*paused = true;

	// allocate a cursor
	cursor_t *cursor = malloc(sizeof(cursor_t));
	cursor->x = 0;
	cursor->y = 0;

	while (!(kb_Data[6] & kb_Clear)) {
		update(board, cursor, paused);
		render(board, cursor, paused);
	};


	for (int i = 0; i < rows; i++) {
		free(board[i]);
	}
	free(board);

	/* End graphics drawing */
	gfx_End();

	return 0;
}


int render(bool** board, cursor_t* cursor, bool* paused)
{
	/* render the game board (just white and black squares) */
	gfx_FillScreen(0);

	gfx_SetColor(255); // white

	int rows = GFX_LCD_HEIGHT / TILE_SIZE;
	int columns = GFX_LCD_WIDTH / TILE_SIZE;

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (board[y][x])
			{
				gfx_FillRectangle_NoClip(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
			}
		}
	}

	// render the cursor on top of the cells
	if (*paused) {
		gfx_SetColor(224); // red
		gfx_Rectangle_NoClip(cursor->x * TILE_SIZE, cursor->y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
	}

	gfx_BlitBuffer();
	return 0;
}


int update(bool** board, cursor_t* cursor, bool* paused)
{
	/*
	 *update the cells following the wikipedia rules, move the cursor,
	 *update kb_data and pause/resume.
	 */

	kb_key_t old_kb_arrows = kb_Data[7];
	kb_key_t old_kb_1 = kb_Data[1]; // for 2nd (also suppr)
	kb_key_t old_kb_6 = kb_Data[6]; // for enter

	kb_Scan();

	// update pause
	if ((kb_Data[1] & kb_2nd) && !(old_kb_1 & kb_2nd)) {
		*paused = !(*paused);
	}


	if (*paused) {
		// update a cell only on pause
		if ((kb_Data[6] & kb_Enter) && !(old_kb_6 & kb_Enter)) {
			board[cursor->y][cursor->x] = !(board[cursor->y][cursor->x]);
		}

		// move cursor
		if ((kb_Data[7] & kb_Right) && !(old_kb_arrows & kb_Right )) {
			cursor->x += 1;
		} else if ((kb_Data[7] & kb_Left) && !(old_kb_arrows & kb_Left )) {
			cursor->x -= 1;
		} else if ((kb_Data[7] & kb_Up) && !(old_kb_arrows & kb_Up )) {
			cursor->y -= 1;
		} else if ((kb_Data[7] & kb_Down) && !(old_kb_arrows & kb_Down )) {
			cursor->y += 1;
		}

		// skips board update if paused
		return 0;
	}


	// creating a new temporary board
	int rows = GFX_LCD_HEIGHT / TILE_SIZE;
	int columns = GFX_LCD_WIDTH / TILE_SIZE;

	// allocate the new board
	bool **new_board = (bool **)malloc(rows * sizeof(bool *));
	for (int i = 0; i < rows; i++) {
		new_board[i] = (bool *)malloc(columns * sizeof(bool));
		for (int j = 0; j < columns; j++)
		{
			new_board[i][j] = false;
		}
	}

	// apply rule for each cell
	for (int y = 0; y < rows; y++) {

		for (int x = 0; x < columns; x++)
		{
			int neighbours = count_neighbours(board, x, y);

			// rules
			if (board[y][x])
			{
				if (neighbours < 2) {new_board[y][x] = false;}
				if (neighbours == 2 || neighbours == 3) {new_board[y][x] = true;}
				if (neighbours > 3) {new_board[y][x] = false;}

			}
			else
			{
				if (neighbours == 3) {new_board[y][x] = true;}
			}

		}
	}

	// copying back the temp board into the "real" one
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			board[i][j] = new_board[i][j];
		}
	}

	// free the temp board
	for (int i = 0; i < rows; i++) {
		free(new_board[i]);
	}
	free(new_board);

	return 0;
}


int count_neighbours(bool** board, int x, int y)
{
	/* Return the number of alive cells around a given cell (x and y) */
	int neighbours = 0;

	int board_width = GFX_LCD_WIDTH / TILE_SIZE;
	int board_height = GFX_LCD_HEIGHT / TILE_SIZE;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (i == 0 && j == 0) {continue;} // skip self

			int neighbour_x = x + i;
			int neighbour_y = y + j;

			if (neighbour_x >= 0 && neighbour_x < board_width &&
				neighbour_y >= 0 && neighbour_y < board_height)
			{
				if (board[neighbour_y][neighbour_x]) {neighbours += 1;}
			}
		}
	}
	return neighbours;
}
