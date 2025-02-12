#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include "dependency_graph.h"




typedef struct operand {
    int type_flag; // 0 for integer constants, 1 for cell references
    union {
        int constant;
        int* cell_operand;
    } operand_value;
} operand;


int operationID;
int editrow;
int editcolumn;
int count_operands;
operand (*formula)[];


// Function Prototypes

void parseCellName(const char* cellName, int* row, int* col);
void parseInput(const char* input,Sheet* spreadsheet , int rows, int cols,bool *disable_output);
int isArithmeticExpression(const char* expression);
int isFunction(const char* expression);

// Main Function
// int main() {
//     int rows, cols;
//     bool disable_output =0;
//     scanf("%d", &rows);
//     scanf("%d", &cols);

//     if (rows < 1 || rows > 999 || cols < 1 || cols > 18278) {
//         printf("Error: Invalid spreadsheet dimensions.\n");
//         return 1;
//     }

//     // Create the spreadsheet
//     // int** spreadsheet = createSpreadsheet(rows + 1, cols + 1); // Add extra row/col for headers

//     // Example input loop
//     char input[256];
//     while (1) {
//         printf("> ");
//         fgets(input, sizeof(input), stdin);
//         input[strcspn(input, "\n")] = '\0'; // Remove trailing newline

//         if (strcmp(input, "q") == 0) break; // Quit command
//         parseInput(input, spreadsheet, rows, cols,disable_output);
//     }

//     // Free the spreadsheet memory
//     // freeSpreadsheet(spreadsheet, rows + 1);

//     return 0;
// }

// Function Definitions

// Create a dynamic 2D array
// int** createSpreadsheet(int rows, int cols) {
//     int** spreadsheet = (int**)malloc(rows * sizeof(int*));
//     for (int i = 0; i < rows; i++) {
//         spreadsheet[i] = (int*)calloc(cols, sizeof(int)); // Initialize with 0
//     }
//     return spreadsheet;
// }

// // Free the allocated memory for the spreadsheet
// void freeSpreadsheet(int** spreadsheet, int rows) {
//     for (int i = 0; i < rows; i++) {
//         free(spreadsheet[i]);
//     }
//     free(spreadsheet);
// }

// Convert cell name (e.g., A1) to row and column indices
void parseCellName(const char* cellName, int* row, int* col) {
    int i = 0;
    *col = 0;

    // Extract column part (letters)
    while (true) {
        if (cellName[i]-'0' >=0 &&cellName[i]-'0' <=9){
            break;
        }
        else if (cellName[i]-'A'>=26 || cellName[i]-'A' <0  ){
            printf("invalidInput");
        }
        *col = *col * 26 + (toupper(cellName[i]) - 'A' + 1);
        i++;
    }

    // Extract row part (numbers)
    if (!isalpha(cellName+1) && !isArithmeticExpression(cellName+1))
    *row = atoi(&cellName[i]);
    else{
        printf("invalidInput");
    }
}

// Parse input command
void parseInput(const char* input, Sheet* spreadsheet, int rows, int cols,bool* disable_output) {
    char cellName[16];
    char expression[128];
    

    if (sscanf(input, "%[^=]=%s", cellName, expression) == 2) {
    
      
        parseCellName(cellName, &editrow, &editcolumn);

        if (editrow <= 0 || editrow > rows || editcolumn <= 0 || editcolumn > cols) {
            printf("Error: Invalid cell reference.\n");
            return;
        }

        if (isdigit(expression[0]) || expression[0] == '-' || expression[0] == '+'||(isalpha(expression[0])&& !strchr(expression,'(')) ) {
            if(!isaplha(expression) && !isArithmeticExpression(expression+1)){
                count_operands=1;
                formula=(operand*)malloc(sizeof(operand));
                (*formula)[0].type_flag = 0; // Constant
                (*formula)[0].operand_value.constant = atoi(expression);
                operationID = 1; // Cell assignment with a constant
            }  
            else if (isaplha(expression) && !isArithmeticExpression(expression+1))  {
                int row1,col1;
                parseCellName(cellName, &row1, &col1);
                if (row1 <= 0 || row1 > rows || col1 <= 0 || col1 > cols) {
                    printf("Error: Invalid cell reference.\n");
                    return;
                }
                count_operands=1;
                formula=(operand*)malloc(sizeof(operand));
                (*formula)[0].type_flag = 1; // Constant
                (*formula)[0].operand_value.cell_operand = spreadsheet->all_cells[row1][col1];
                operationID = 2; // Cell assignment with a constant

            }  
            else if (isaplha(expression) && isArithmeticExpression(expression+1)) {
                count_operands=2;
                formula=(operand*)malloc(sizeof(operand)*2);
                char operand1[16], operand2[16], op;
                if (!(expression[0]=='+'|| expression[0]=='-')){

                    if (sscanf(expression, "%[^+-*/]%c%s", operand1, &op, operand2) == 3) {
                        int val1, val2;
                        if (isalpha(operand1[0])) {
                            int r1, c1;
                            parseCellName(operand1, &r1, &c1);
                            (*formula)[0].type_flag = 1; // Constant
                            (*formula)[0].operand_value.cell_operand = spreadsheet->all_cells[r1][c1] ;
                            
                            operationID = AssignValue(&op); // Cell assignment with a constant  // remember here emre ko operation id assignment karna hai with +,-,/,*
                            
                        } else {
                            (*formula)[0].type_flag = 0; // Constant
                            (*formula)[0].operand_value.constant = atoi(operand1) ;
                            operationID = AssignValue(&op); // Cell assignment with a constant
                        }

                        if (isalpha(operand2[0])) {
                            int r1, c1;
                            parseCellName(operand2, &r1, &c1);
                            (*formula)[1].type_flag = 1; // Constant
                            (*formula)[1].operand_value.cell_operand = spreadsheet->all_cells[r1][c1] ;
                            operationID = AssignValue(&op); // Cell assignment with a constant
                            
                        } else {
                            (*formula)[1].type_flag = 0; // Constant
                            (*formula)[1].operand_value.constant = atoi(operand2) ;
                            operationID = AssignValue(&op); // Cell assignment with a constant
                        }
                      
                    }
            }
            else if((expression[0]=='+'|| expression[0]=='-')){
                if (sscanf(expression+1, "%[^+-*/]%c%s", operand1, &op, operand2) == 3) {
                    int val1, val2;
                    if (isalpha(operand1[0])) {
                        int r1, c1;
                        parseCellName(operand1, &r1, &c1);
                        (*formula)[0].type_flag = 1; // Constant
                        (*formula)[0].operand_value.cell_operand = spreadsheet->all_cells[r1][c1] ;
                        operationID = AssignValue(&op); // Cell assignment with a constant  // remember here emre ko operation id assignment karna hai with +,-,/,*
                        
                    } else {
                        int value=atoi(operand1);
                        if(expression[0]=="-"){
                            value=value*(-1);
                        }
                        (*formula)[0].type_flag = 0; // Constant
                        (*formula)[0].operand_value.constant = value ;
                        operationID = AssignValue(&op); // Cell assignment with a constant
                    }

                    if (isalpha(operand2[0])) {
                        int r1, c1;
                        parseCellName(operand2, &r1, &c1);
                        (*formula)[1].type_flag = 1; // Constant
                        (*formula)[1].operand_value.cell_operand = spreadsheet->all_cells[r1][c1];
                        operationID = AssignValue(&op); // Cell assignment with a constant
                        
                    } else {
                        (*formula)[1].type_flag = 0; // Constant
                        (*formula)[1].operand_value.constant = atoi(operand2) ;
                        operationID = AssignValue(&op); // Cell assignment with a constant
                    }
                  
                }
           } 
         } 
        } else if (isFunction(expression)) {
           
          
            
            char functionName[16], range[64];
            if (sscanf(expression, "%[^()](%[^)])", functionName, range) == 2) {
                if (functionName =="MIN"){
                    operationID=7;
                }
                else if (functionName =="MAX"){
                    operationID=8;
                }
                else if (functionName =="AVG"){
                    operationID=9;                
                }
                else if (functionName =="SUM"){
                    operationID=10;      
                }
                else if (functionName =="STDEV"){
                    operationID=11;  
                }
                else if (functionName =="SLEEP"){
                    operationID=12;  
                }
                if(functionName =="SLEEP"){
                    count_operands=1;
                    formula=(operand*)malloc(sizeof(operand));
                    char extractedvalue[16];
                    if(sscanf(range, "( %[^)] )", extractedvalue) == 1){
                    
                    if (isalpha(extractedvalue)) {
                        int r3, c3;
                        parseCellName(extractedvalue, &r3, &c3);
                        (*formula)[0].type_flag = 1; // Constant
                        (*formula)[0].operand_value.cell_operand = spreadsheet->all_cells[r3][c3] ;             
                    } else {
                        (*formula)[0].type_flag = 0; // Constant
                        (*formula)[0].operand_value.constant = atoi(extractedvalue) ;
                       
                    }
                }

                }
                else{
                   
                    int r3, c3, r4, c4;
                    char start[16], end[16];
                    if (sscanf(range, "%[^:]:%s", start, end) == 2) {
                        parseCellName(start, &r3, &c3);
                        parseCellName(end, &r4, &c4);
                        if(r3>r3 || c3 > c4){ printf("Invalid range");
                            return ;}
                        count_operands=(r4-r3+1)*(c4-c3+1);
                        formula=(operand*)malloc(sizeof(operand)*(r4-r3+1)*(c4-c3+1));
                       
                        int count=0;
                            for (int i = r3; i <= r4; i++) {
                                for (int j = c3; j <= c4; j++) {
                                    (*formula)[count].type_flag = 1; // Constant
                                    (*formula)[count].operand_value.cell_operand = spreadsheet->all_cells[i][j] ;
                                    count++;
                                  
                                } 
                            }
                        
                        }
                    }
                }
       
            } else {
                printf("Error: Invalid function call.\n");
            }
        } 
    

}

// Check if an expression is arithmetic
int isArithmeticExpression(const char* expression) {
    return strpbrk(expression, "+-*/") != NULL;
}

// Check if an expression is a function
int isFunction(const char* expression) {
    return strstr(expression, "MIN(") || strstr(expression, "MAX(") ||
           strstr(expression, "AVG(") || strstr(expression, "SUM(") || strstr(expression, "STDEV(") || strstr(expression, "SLEEP(");
}

// Perform arithmetic operation
int AssignValue(char *op){
 if (*op =="+"){return 3;}
 else if (*op =="-"){return 4;}
 else if (*op =="*"){return 5;}
 else if (*op =="/"){return 6;}
 else {
    printf("Error");
 }

}




