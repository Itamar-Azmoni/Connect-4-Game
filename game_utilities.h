#pragma once
#include <stdio.h>

#define ROWS_NUM 6
#define COLUMNS_NUM 7

void init_grid(char new_grid[][COLUMNS_NUM]);
int place(int column, char value);
int check_grid_full();
int check_win(int row, int col);
int check_sequence(char value, char lst[], int len);
void print_grid();

extern char grid[ROWS_NUM][COLUMNS_NUM];