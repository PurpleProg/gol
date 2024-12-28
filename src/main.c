#include <graphx.h>
#include <keypadc.h>
#include <stdbool.h>
#include <stdlib.h>

#define TILE_SIZE 8


int render(bool**);
int update(bool**);
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
    for (int i = 0; i < rows; i++)
    {
        board[i] = (bool *)malloc(columns * sizeof(bool));
        for (int j = 0; j < columns; j++)
        {
            board[i][j] = false;
        }
    }

    //    1 2 3
    // 1    X
    // 2      X
    // 3  X X X

    // test fill planer
    board[3][1] = true;
    board[3][2] = true;
    board[3][3] = true;
    board[2][3] = true;
    board[1][2] = true;

    do {
        update(board);
              render(board);
              kb_Scan();
    } while (!(kb_Data[6] & kb_Clear));


    for (int i = 0; i < rows; i++)
    {
        free(board[i]);
    }
    free(board);

    /* End graphics drawing */
    gfx_End();

    return 0;
}


int render(bool** board)
{
    gfx_FillScreen(0);
    gfx_SetColor(255);

    for (int y = 0; y < 30; y++)
    {
        for (int x = 0; x < 40; x++)
        {
            if (board[y][x])
            {
                gfx_FillRectangle_NoClip(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
            }
        }
    }
    gfx_BlitBuffer();
    return 0;
}

int update(bool** board)
{
    // copying the board
    int rows = GFX_LCD_HEIGHT / TILE_SIZE;
    int columns = GFX_LCD_WIDTH / TILE_SIZE;

    bool **new_board = (bool **)malloc(rows * sizeof(bool *));
    for (int i = 0; i < rows; i++)
    {
        new_board[i] = (bool *)malloc(columns * sizeof(bool));
        for (int j = 0; j < columns; j++)
        {
            new_board[i][j] = false;
        }
    }

    // apply rule for each cell
    for (int y = 0; y < rows; y++)
    {
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

    // copying back the board
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            board[i][j] = new_board[i][j];
        }
    }
    for (int i = 0; i < rows; i++) {
        free(new_board[i]);
    }
    free(new_board);

    return 0;
}


int count_neighbours(bool** board, int x, int y)
{
    if ( (x <= 0 || (x*TILE_SIZE) >= GFX_LCD_WIDTH) || (y <= 0 || (y*TILE_SIZE) >= GFX_LCD_HEIGHT))
    {
        return 0; // kill the cell if it get close to the wall
    }


    int neighbours = 0;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (i == 0 && j == 0) {continue;} // skip self
            if (board[y + i][x + j]) {neighbours += 1;}

        }
    }
    return neighbours ;
}