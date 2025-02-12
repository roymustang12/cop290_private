#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<input.c>
#include <unistd.h>

struct Cell;
int status = 0;
typedef struct operand
{
    int type_flag;
    //type_flat = 0 for integer constants 
    //type_flag = 1 for formulas in the cell
    union {
        int constant;
        Cell* cell_operand;
    } operand_value;

}operand;

typedef struct  Cell
{
    int value;
    int operation_id;
    operand (*formula)[];
    int count_operands;
    int r;
    int c;
    struct Cell** dependents; //This is a pointer to an array of pointers which point to all the cells which  are dependent on it
    int  count_dependents;
    struct Cell** precedents;//This is a pointer to an array of pointers which point to all the cells on which the current cell is dependent 
    int count_precedents;
    int is_recalculate;
}Cell;

typedef struct Coord
{
    int x;
    int y;
};

//Sheet contains a 2D array of Cell pointers 
//all_cell is a pointer to the sheet
//all_cells contain a 
typedef struct Spreadsheet
{
    int rows;
    int columns;
    Cell*** all_cells; //pointer to an array of pointers which point to an array of pointers which refer to Cell
}Sheet;

Sheet* initialise(int rows, int columns)
{
    Sheet* sheet = (Sheet*)malloc(sizeof(Sheet));
    sheet->rows = rows;
    sheet->columns = columns;
    sheet->all_cells = (Cell***)malloc(sizeof(Cell**) * rows);
    for(int i = 0; i < sheet->rows; i++)
    {
        sheet->all_cells[i] = (Cell**)malloc(sizeof(Cell*) * columns);
        for(int j = 0;j < sheet->columns; j++)
        {
           sheet->all_cells[i][j] = (Cell*)malloc(sizeof(Cell));
           sheet->all_cells[i][j]->r = i;
           sheet->all_cells[i][j]->c = j;
           sheet->all_cells[i][j]->value = 0;
           sheet->all_cells[i][j]->operation_id = -1; //operand_id = -1 when cell is not assigned any formula
           sheet->all_cells[i][j]->count_dependents = 0;
           sheet->all_cells[i][j]->count_precedents = 0;
           sheet->all_cells[i][j]->formula = NULL; // Assuming a maximum of 10 operands for simplicity
           sheet->all_cells[i][j]->dependents = NULL;
           sheet->all_cells[i][j]->precedents = NULL;
           sheet->all_cells[i][j]->is_recalculate = 0;
           sheet->all_cells[i][j]->count_operands = 0;
            //0 when no recalculation is needed 
        }
    }
    return sheet;
}

//Cell rt,ct contains the cell rf,cf in the formula expresssion
//Add rt,ct to the dependents of rf,cf
//Add rf,cf to the precedents of rt,ct
void add_dependency(Sheet* sheet, int rf, int cf, int rt, int ct)
{
    Cell* dependent_cell = sheet->all_cells[rt][ct];
    Cell* precedent_cell = sheet->all_cells[rf][cf];
    precedent_cell->count_dependents+=1;
    precedent_cell->dependents = realloc(precedent_cell->dependents,sizeof(Cell*) * precedent_cell->count_dependents);
    precedent_cell->dependents[precedent_cell->count_dependents - 1] = dependent_cell;

    dependent_cell->count_precedents+=1;
    dependent_cell->precedents = realloc(dependent_cell->precedents,sizeof(Cell*) * dependent_cell->count_precedents);
    dependent_cell->precedents[dependent_cell->count_precedents - 1] = precedent_cell;

}

//Cell rt,ct contains the cell rf,cf in the formula expresssion
//When updating the formula of rt,ct we need to remove the cell rt,ct 
//list of depedents of rf,cf 
void delete_depedency(Sheet* sheet, int rf, int cf, int rt, int ct)
{
    Cell* dependent_cell = sheet->all_cells[rt][ct];
    Cell* precedent_cell = sheet->all_cells[rf][cf];
    int target_index = -1;
    for(int i = 0; i < precedent_cell->count_dependents; i++)
    {
        if(precedent_cell->dependents[i]->r == rt && precedent_cell->dependents[i]->c == ct)
        {
            target_index = i;
            break;
        }
    }
    //Ab yaha se memory hata and resize
    if (target_index != -1)
    {
        for(int i = target_index; i < target_index - 1; i++)
        {
            precedent_cell->dependents[i] = precedent_cell->dependents[i+1];
        }
        precedent_cell->count_dependents-=1;
        if(precedent_cell->count_dependents > 0)
        {
            precedent_cell->dependents = realloc(precedent_cell->dependents, sizeof(Cell*) * precedent_cell->count_dependents);
        }
        else
        {
            free(precedent_cell->dependents);
            precedent_cell->dependents = NULL;
        }
    }
} 

//When a cell is assigned an expression we need to clear all the old precedents of this cell
void clear_precedents(Sheet* sheet, int rt, int ct)
{
    Cell* target_cell = sheet->all_cells[rt][ct];
    free(target_cell->precedents);
    target_cell->precedents = NULL;
    target_cell->count_precedents = 0;
}

//On updation of value of cell at r,c we need to update all
//the cells dependent on the cell recently assigned 
void recalculate_dependents(Sheet* sheet, int r, int c)
{   if (has_cycle(sheet, sheet->all_cells[r][c]) == 0)
        {Cell** q = (Cell**)malloc(sizeof(Cell*) * sheet->rows * sheet->columns);
        int front = 0;
        int rear = 0;
        Cell* assigned_cell = sheet->all_cells[r][c];
        //Add the immediate dependents of the assigned cell to the queue
        for(int i = 0; i < assigned_cell->count_dependents; i++)
        {
            q[rear++] = (assigned_cell->dependents)[i];

        }

        while(front < rear)
        {
            Cell* current_cell = q[front];
            front = front + 1;
            calculate_cell_value(sheet, current_cell->r, current_cell->c);
            //Add the dependents of the current_cell to the queue
            for(int i = 0; i < current_cell->count_dependents;i++)
            {
                q[rear++] = current_cell->dependents[i];
            }
        }

        free(q);}
    else{
        status = 17;
    }
}

int detect_cycle(Cell* cell, Cell** visited, Cell** recursion_stack, int visited_count, int stack_count) {
    // Add current cell to visited and recursion stack
    visited[visited_count++] = cell;
    recursion_stack[stack_count++] = cell;
    
    // Check all cells this cell depends on
    for(int i = 0; i < cell->count_precedents; i++) {
        Cell* precedent = cell->precedents[i];
        
        // Check if cell is in recursion stack (cycle found)
        for(int j = 0; j < stack_count; j++) {
            if(precedent == recursion_stack[j]) {
                return 1;
            }
        }
        
        // Check if cell hasn't been visited
        int already_visited = 0;
        for(int j = 0; j < visited_count; j++) {
            if(precedent == visited[j]) {
                already_visited = 1;
                break;
            }
        }
        
        // If not visited, recursively check for cycles
        if(!already_visited) {
            if(detect_cycle(precedent, visited, recursion_stack, 
                          visited_count, stack_count)) {
                return 1;
            }
        }
    }
    
    // Remove current cell from recursion stack
    stack_count--;
    return 0;
}

// Wrapper function to initialize arrays and call cycle detection
int has_cycle(Sheet* sheet, Cell* start_cell) {
    Cell** visited = malloc(sheet->rows * sheet->columns * sizeof(Cell*));
    Cell** recursion_stack = malloc(sheet->rows * sheet->columns * sizeof(Cell*));
    
    int result = detect_cycle(start_cell, visited, recursion_stack, 0, 0);
    
    free(visited);
    free(recursion_stack);
    return result;
}


void calculate_cell_value(Sheet* sheet, int rt, int ct)
{
    Cell* target_cell = sheet->all_cells[rt][ct];
    switch(target_cell->operation_id)
    {
        case 1 :
                {int value = (*(target_cell->formula))[0].operand_value.constant;
                target_cell->value = value;
                break;}
        case 2 :
                {int value = (*(target_cell->formula))[0].operand_value.cell_operand->value;
                target_cell->value = value;
                break;}
        case 3:
                {int flag1 = (*(target_cell->formula))[0].type_flag;
                int flag2 = (*(target_cell->formula))[1].type_flag;
                int value1, value2;
                if (flag1 == 0)
                {
                    value1 = (*(target_cell->formula))[0].operand_value.constant;
                }
                else
                {
                    value1 = (*(target_cell->formula))[0].operand_value.cell_operand->value;
                }
                if (flag2 == 0)
                {
                    value2 = (*(target_cell->formula))[1].operand_value.constant;
                }
                else
                {
                    value2 = (*(target_cell->formula))[1].operand_value.cell_operand->value;
                }
                target_cell->value = value1 + value2;
                break;}
        case 4:
                {int flag1 = (*(target_cell->formula))[0].type_flag;
                int flag2 = (*(target_cell->formula))[1].type_flag;
                int value1, value2;
                if (flag1 == 0)
                {
                    value1 = (*(target_cell->formula))[0].operand_value.constant;
                }
                else
                {
                    value1 = (*(target_cell->formula))[0].operand_value.cell_operand->value;
                }
                if (flag2 == 0)
                {
                    value2 = (*(target_cell->formula))[1].operand_value.constant;
                }
                else
                {
                    value2 = (*(target_cell->formula))[1].operand_value.cell_operand->value;
                }
                target_cell->value = value1 - value2;
                break;}
    case 5 :
                {int flag1 = (*(target_cell->formula))[0].type_flag;
                int flag2 = (*(target_cell->formula))[1].type_flag;
                int value1, value2;
                if (flag1 == 0)
                {
                    value1 = (*(target_cell->formula))[0].operand_value.constant;
                }
                else
                {
                    value1 = (*(target_cell->formula))[0].operand_value.cell_operand->value;
                }
                if (flag2 == 0)
                {
                    value2 = (*(target_cell->formula))[1].operand_value.constant;
                }
                else
                {
                    value2 = (*(target_cell->formula))[1].operand_value.cell_operand->value;
                }
                target_cell->value = value1 * value2;
                break;}
    case 6 :
                {int flag1 = (*(target_cell->formula))[0].type_flag;
                int flag2 = (*(target_cell->formula))[1].type_flag;
                int value1, value2;
                if (flag1 == 0)
                {
                    value1 = (*(target_cell->formula))[0].operand_value.constant;
                }
                else
                {
                    value1 = (*(target_cell->formula))[0].operand_value.cell_operand->value;
                }
                if (flag2 == 0)
                {
                    value2 = (*(target_cell->formula))[1].operand_value.constant;
                }
                else
                {
                    value2 = (*(target_cell->formula))[1].operand_value.cell_operand->value;
                }
                target_cell->value = value1 / value2;
                break;}
    case 7 :
                {int values[target_cell->count_operands];
                int flag;
                for(int i = 0; i < target_cell->count_operands ; i++)
                {
                    flag = (*(target_cell->formula))[i].type_flag;
                    if (flag == 0)
                    {
                        values[i] = (*(target_cell->formula))[i].operand_value.constant;
                    }
                    else
                    {
                        values[i] = (*(target_cell->formula))[i].operand_value.cell_operand->value;
                    }
                }
                int temp = values[0];
                for(int i = 0; i < target_cell->count_operands; i++)
                {
                    temp = min(temp, values[i]);
                }
                target_cell->value = temp;
                break;}
        case 8 : 
                {
                int values[target_cell->count_operands];
                int flag;
                for(int i = 0; i < target_cell->count_operands ; i++)
                {
                    flag = (*(target_cell->formula))[i].type_flag;
                    if (flag == 0)
                    {
                        values[i] = (*(target_cell->formula))[i].operand_value.constant;
                    }
                    else
                    {
                        values[i] = (*(target_cell->formula))[i].operand_value.cell_operand->value;
                    }
                }
                int temp = values[0];
                for(int i = 0; i < target_cell->count_operands; i++)
                {
                   temp = max(temp, values[i]);
                }
                target_cell->value = temp;
                break;   
                }
        case 9:
                {int values[target_cell->count_operands];
                int flag;
                for(int i = 0; i < target_cell->count_operands ; i++)
                {
                    flag = (*(target_cell->formula))[i].type_flag;
                    if (flag == 0)
                    {
                        values[i] = (*(target_cell->formula))[i].operand_value.constant;
                    }
                    else
                    {
                        values[i] = (*(target_cell->formula))[i].operand_value.cell_operand->value;
                    }
                }
                int temp = 0;
                for(int i = 0; i < target_cell->count_operands; i++)
                {
                   temp = temp + values[i];
                }
                target_cell->value = temp/target_cell->count_operands;
                break;}
        case 10 :
                {
                    int values[target_cell->count_operands];
                int flag;
                for(int i = 0; i < target_cell->count_operands ; i++)
                {
                    flag = (*(target_cell->formula))[i].type_flag;
                    if (flag == 0)
                    {
                        values[i] = (*(target_cell->formula))[i].operand_value.constant;
                    }
                    else
                    {
                        values[i] = (*(target_cell->formula))[i].operand_value.cell_operand->value;
                    }
                }
                int temp = 0;
                for(int i = 0; i < target_cell->count_operands; i++)
                {
                    temp = temp + values[i];
                }
                target_cell->value = temp;
                break;
                }
        case 11:
                {
                int values[target_cell->count_operands];
                int flag;
                for(int i = 0; i < target_cell->count_operands ; i++)
                {
                    flag = (*(target_cell->formula))[i].type_flag;
                    if (flag == 0)
                    {
                        values[i] = (*(target_cell->formula))[i].operand_value.constant;
                    }
                    else
                    {
                        values[i] = (*(target_cell->formula))[i].operand_value.cell_operand->value;
                    }
                }
                // Calculate mean
                double sum = 0.0;
                for (int i = 0; i < target_cell->count_operands; i++)
                {
                    sum += values[i];
                }
                double mean = sum / target_cell->count_operands;
                
                // Calculate squared differences and their sum
                double sq_sum = 0.0;
                for (int i = 0; i < target_cell->count_operands; i++)
                {
                    double diff = values[i] - mean;
                    sq_sum += diff * diff;
                }
                
                // Calculate standard deviation (using population std deviation formula)
                double std_dev = sqrt(sq_sum / target_cell->count_operands);
                
                // Optionally round or convert to int if needed.
                target_cell->value = (int)std_dev;
                break;
                }
        case 12:
                {int seconds;
                if ((*(target_cell->formula))[0].type_flag == 0) {
                    seconds = (*(target_cell->formula))[0].operand_value.constant;
                } else {
                    seconds = (*(target_cell->formula))[0].operand_value.cell_operand->value;
                }
                target_cell->value = handle_sleep(seconds);
                break;}
    }
}

int handle_sleep(int seconds) {
    sleep(seconds);  
    return seconds;}

//When a cell is assigned a formula 
//We need to update its attributes 
//Remove the old formula and assign the new formula
//Update number of operands 
//Remove its old precdents and delete teh cell from the dependent list of its precedents
//For all the cell in the cells in the formula add the current cell to their dependents list 

void assign_cell(Sheet* sheet, int r, int c,int operation_id, operand (*formula)[],int count_operands)
{
    Cell* target_cell = sheet->all_cells[r][c];
    for(int i = 0; i < target_cell->count_precedents; i++)
    {
        Cell* curr = (target_cell->precedents)[i];
        delete_depedency(sheet,curr->r,curr->c,r,c);
    }
    clear_precedents(sheet, r, c);
    target_cell->formula = formula;
    target_cell->operation_id = operation_id;
    target_cell->count_operands = count_operands;
    for(int i = 0; i < count_operands; i++)
    {
        if((*formula)[i].type_flag == 1)
        {
            Cell* precedent_cell = (*formula)[i].operand_value.cell_operand;
            add_dependency(sheet,precedent_cell->r,precedent_cell->c,r,c);            
        }
    }
    calculate_cell_value(sheet, r, c);
    recalculate_dependents(sheet, r, c);
}

;