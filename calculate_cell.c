#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<dependency_graph.c>

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