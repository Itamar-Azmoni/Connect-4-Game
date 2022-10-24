#include "game_utilities.h"

char grid[ROWS_NUM][COLUMNS_NUM];

void init_grid(char new_grid[][COLUMNS_NUM])
{
    // Initialize the grid with new_grid values, if new_grid = NULL, initialize with zeros
    int i, j;
    if(new_grid == NULL) // Initialize with zeros
    {
        for(i = 0; i < ROWS_NUM; i++)
        {
            for(j = 0; j < COLUMNS_NUM; j++)
                grid[i][j] = 0;
        }
    }else // Initialize with new_grid values
    {
        for(i = 0; i < ROWS_NUM; i++)
        {
            for(j = 0; j < COLUMNS_NUM; j++)
                grid[i][j] = new_grid[i][j];
        }
    }
}

int place(int column, char value)
{
    // Placing the value at the right row in the column we get
    int row;
    if(grid[ROWS_NUM-1][column] != 0)
        return -1;
    for(row = 0; row < ROWS_NUM; row++)
    {
        if(grid[row][column] == 0)
        {
            grid[row][column] = value;
            break;
        }
    }
    return row; // Return the row number
}

int check_grid_full()
{
    // Check if the grid is full by checking the top row
    int i;
    for(i = 0; i < COLUMNS_NUM; i++)
    {
        if(grid[ROWS_NUM-1][i] == 0)
            return 0;
    }
    return 1;
}

int check_win(int row, int col)
{
    // Check if the value in grid[row][col] is a part of a sequence
    char val = grid[row][col];
    char lst[7];
    lst[3] = val;
    
    //check row
    lst[0] = (col-3 < 0) ? 0 : grid[row][col-3];
    lst[1] = (col-2 < 0) ? 0 : grid[row][col-2];
    lst[2] = (col-1 < 0) ? 0 : grid[row][col-1];
    lst[4] = (col+1 >= COLUMNS_NUM) ? 0 : grid[row][col+1];
    lst[5] = (col+2 >= COLUMNS_NUM) ? 0 : grid[row][col+2];
    lst[6] = (col+3 >= COLUMNS_NUM) ? 0 : grid[row][col+3];
    if(check_sequence(val, lst, sizeof(lst)))
        return val;

    //check column
    lst[0] = (row-3 < 0) ? 0 : grid[row-3][col];
    lst[1] = (row-2 < 0) ? 0 : grid[row-2][col];
    lst[2] = (row-1 < 0) ? 0 : grid[row-1][col];
    lst[4] = (row+1 >= ROWS_NUM) ? 0 : grid[row+1][col];
    lst[5] = (row+2 >= ROWS_NUM) ? 0 : grid[row+2][col];
    lst[6] = (row+3 >= ROWS_NUM) ? 0 : grid[row+3][col];
    if(check_sequence(val, lst, sizeof(lst)))
        return val;

    //check diagonal line up
    lst[0] = (row-3 < 0 || col-3 < 0) ? 0 : grid[row-3][col-3];
    lst[1] = (row-2 < 0 || col-2 < 0) ? 0 : grid[row-2][col-2];
    lst[2] = (row-1 < 0 || col-1 < 0) ? 0 : grid[row-1][col-1];
    lst[4] = (row+1 >= ROWS_NUM || col+1 >= COLUMNS_NUM) ? 0 : grid[row+1][col+1];
    lst[5] = (row+2 >= ROWS_NUM || col+2 >= COLUMNS_NUM) ? 0 : grid[row+2][col+2];
    lst[6] = (row+3 >= ROWS_NUM || col+3 >= COLUMNS_NUM) ? 0 : grid[row+3][col+3];
    if(check_sequence(val, lst, sizeof(lst)))
        return val;

    //check diagonal line down
    lst[0] = (row+3 >= ROWS_NUM || col-3 < 0) ? 0 : grid[row+3][col-3];
    lst[1] = (row+2 >= ROWS_NUM || col-2 < 0) ? 0 : grid[row+2][col-2];
    lst[2] = (row+1 >= ROWS_NUM || col-1 < 0) ? 0 : grid[row+1][col-1];
    lst[4] = (row-1 < 0 || col+1 > COLUMNS_NUM) ? 0 : grid[row-1][col+1];
    lst[5] = (row-2 < 0 || col+2 > COLUMNS_NUM) ? 0 : grid[row-2][col+2];
    lst[6] = (row-3 < 0 || col+3 > COLUMNS_NUM) ? 0 : grid[row-3][col+3];
    if(check_sequence(val, lst, sizeof(lst)))
        return val;
    
    return 0;
}

int check_sequence(char value, char lst[], int len)
{
    // check sequence in lst
    int i, seq = 0;
    for (i = 0; i < len; i++)
    {
        if(lst[i] == value)
            seq++;
        else
            seq = 0;
        if(seq == 4)
            return 1;
    }
    return 0;
}

void print_grid()
{
    // Printing the grid
    int i, j;
    printf(" 1 2 3 4 5 6 7\n");
    printf("|_ _ _ _ _ _ _|\n");
    for(i = ROWS_NUM-1; i >= 0; i--)
    {
        printf("|");
        for(j = 0; j < COLUMNS_NUM; j++){
            switch(grid[i][j])
            {
                case 0:
                    printf("O");
                    break;
                case 1:
                    printf("\033[0;31mO\033[0m");
                    break;
                case 2:
                    printf("\033[0;33mO\033[0m");
                    break;
            }
            if(j < COLUMNS_NUM-1)
                printf(" ");
        } 
        printf("|\n");
    }
    printf("|¯ ¯ ¯ ¯ ¯ ¯ ¯|\n");
}