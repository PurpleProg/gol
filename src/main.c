#include <graphx.h>
#include <keypadc.h>
#include <stdbool.h>
#include <stdlib.h>


int render(bool (*board)[30][40]);
int update(bool (*board)[30][40]);
int count_neighbours(bool (*board)[30][40], int x, int y);


int main(void)
{
    /* Initialize graphics drawing */
    gfx_Begin();
    gfx_SetTransparentColor(0);
    gfx_SetDrawBuffer();
    gfx_FillScreen(0);


    const int TILE_SIZE = 8;
    bool board[30][40];

    // test fill
    board[1][1] = true;
    board[1][2] = true;
    board[1][3] = true;
    board[1][4] = true;
    board[1][5] = true;
    board[1][6] = true;
    board[1][7] = true;


    do {
        update(&board);
        render(&board);
        kb_Scan();
    } while (!(kb_Data[6] & kb_Clear));


    /* End graphics drawing */
    gfx_End();
    return 0;
}


int render(bool (*board)[30][40])
{
    gfx_FillScreen(0);
    gfx_SetColor(255);

    for (int y = 0; y < 30; y++)
    {
        for (int x = 0; x < 40; x++)
        {
            if ((*board)[y][x])
            {
                gfx_FillRectangle_NoClip(x*8, y*8, 8, 8);
            }
        }
    }
    gfx_BlitBuffer();
    return 0;
}

int update(bool (*board)[30][40])
{
    // copying the board
    bool new_board[30][40];
    for (int y = 0; y < 30; y++)
    {
        for (int x = 0; x < 40; x++)
        {
            new_board[y][x] = (*board)[y][x];
        }
    }
    // apply rule for each cell
    for (int y = 0; y < 30; y++)
    {
        for (int x = 0; x < 40; x++)
        {
            int neighbours = count_neighbours(board, x, y);

            // rules
            if ((*board)[y][x])
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
    for (int y = 0; y < 30; y++)
    {
        for (int x = 0; x < 40; x++)
        {
            (*board)[y][x] = new_board[y][x];
        }
    }

    return 0;
}


int count_neighbours(bool (*board)[30][40], int x, int y)
{
    int neighbours = 0;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (i == 0 && j == 0) {continue;} // skip self
            if ((*board)[y + i][x + j]) {neighbours += 1;}

        }
    }
    return neighbours ;
}
