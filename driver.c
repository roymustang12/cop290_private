#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include "dependency_graph.h"
#include <time.h>
#include "input.h"

void print_sheet(Sheet* sheet, int start_row, int start_col, int visible_rows, int visible_cols);
int main(int argc, char* argv[])
{
    int rows = atoi(argv[1]);
    int columns = atoi(argv[2]);
    if(!(rows >=1 && rows <= 999 && columns >=1 && columns <= 18278))
    {
        printf("Out fo bounds rows or columns");
        exit(1);
    }
    Sheet* sheet;
    sheet = initialise(rows, columns);
    char input[256];
    char status[256] ="ok";
    double execution_time = 0.0;
    bool print_flag = true;
    while(1)
    {
        printf("%.1f (%s) > ", execution_time, status);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, '\n')] = '\0';
        clock_t start = clock();
        if(strcmp(input, "q") == 0)
        {
            break;
        }
        else if (strcmp(input, "disble_output") == 0)
        {
            print_flag = false;
        }
        else if (strcmp(input, "enable_output") == 0)
        {
            print_flag = true;
        }
        parseInput(input, sheet, rows, columns, print_flag); // Remove the print flag
        if((strcmp(input, "enable_output") != 0) || (strcmp(input, "disable_output") != 0) || (strncmp(input, "scroll", 7) != 0))
        {
            parseInput(input, sheet, rows, columns, print_flag); // Remove the print flag
            assign_cell(sheet, editrow, editcolumn, operationID, formula, count_operands);
        }
        clock_t end = clock();
        double execution_time = (end - start) / (CLOCKS_PER_SEC);
        if (print_flag == false)
        {
            continue;
        }
        else if (strcmp(input, "enable_output") == 0)
        {
            continue;
        }
        else 
        {
            //print code 
            print_sheet(sheet, editrow, editcolumn, 10, 10);
        }
    }
}

void print_sheet(Sheet* sheet, int start_row, int start_col, int visible_rows, int visible_cols) {
    // Print column headers (A, B, C, etc.)
    printf("\t");
    for(int j = start_col; j < start_col + visible_cols && j < sheet->columns; j++) {
        printf("%c\t", 'A' + j);
    }
    printf("\n");
    
    // Print rows with row numbers and cell values
    for(int i = start_row; i < start_row + visible_rows && i < sheet->rows; i++) {
        // Print row number
        printf("%d\t", i + 1);
        
        // Print cell values in the row
        for(int j = start_col; j < start_col + visible_cols && j < sheet->columns; j++) {
            printf("%d\t", sheet->all_cells[i][j]->value);
        }
        printf("\n");
    }
}
