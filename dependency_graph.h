#ifndef DEPENDENCY_GRAPH_H
#define DEPENDENCY_GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include<stdbool.h>
// Forward declaration of Cell for use in the union
struct Cell;
extern int status;
typedef struct operand {
    int type_flag; 
    // type_flag = 0 for integer constants 
    // type_flag = 1 for formulas in the cell
    union {
        int constant;
        struct Cell* cell_operand;
    } operand_value;
} operand;

typedef struct Cell {
    int value;
    int operation_id;
    operand (*formula)[];
    int count_operands;
    int r;
    int c;
    struct Cell** dependents; // Array of pointers to Cells that depend on this Cell
    int count_dependents;
    struct Cell** precedents; // Array of pointers to Cells on which this Cell depends
    int count_precedents;
    int is_recalculate;
    bool is_error;
} Cell;

typedef struct Coord {
    int x;
    int y;
} Coord;

typedef struct Spreadsheet {
    int rows;
    int columns;
    Cell*** all_cells; // 2D array of pointers to Cell
} Sheet;

// Function prototypes
Sheet* initialise(int rows, int columns);
void add_dependency(Sheet* sheet, int rf, int cf, int rt, int ct);
void delete_depedency(Sheet* sheet, int rf, int cf, int rt, int ct);
void clear_precedents(Sheet* sheet, int rt, int ct);
void recalculate_dependents(Sheet* sheet, int r, int c);
// void recalculate_dependents_2(Sheet* sheet, int r, int c);
void topological_sort(Sheet* sheet, Cell* start_cell);
void dfs_topological_sort(Sheet* sheet, Cell* cell, Cell** stack, int* stack_size, bool* visited);
int detect_cycle(Cell* cell, Cell** visited, Cell** recursion_stack, int visited_count, int stack_count);
int has_cycle(Sheet* sheet, Cell* start_cell);
void calculate_cell_value(Sheet* sheet, int rt, int ct);
void assign_cell(Sheet* sheet, int r, int c, int operation_id, operand (*formula)[], int count_operands);
int min(int a, int b);
int max(int a, int b);
void print_formula(Sheet* sheet, int r, int c);
bool precedent_has_error(Sheet* sheet, int r, int c);
bool zero_div_err(Sheet* sheet, int r, int c);
// int handle_sleep(int seconds);
int handle_sleep(Sheet* sheet, int row, int col,int seconds);
int stdev(int* arr, int n);
void* sleep_thread_function(void* arg);
#endif // DEPENDENCY_GRAPH_H