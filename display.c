#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ROWS 999
#define MAX_COLS 18278
#define VIEWPORT_SIZE 10

int sheet[MAX_ROWS][MAX_COLS]; // Spreadsheet storage
int start_row = 0, start_col = 0; // Viewport starting position
int current_row = 0, current_col = 0; // Current focused cell
int display_cell = 0, display_row = 0, display_column = 0; // Display mode counters

void get_col_label(int col, char *label) {
    int index = 0;
    col++; // 1-based index
    while (col > 0) {
        col--; 
        label[index++] = 'A' + (col % 26);
        col /= 26;
    }
    label[index] = '\0';
    for (int i = 0; i < index / 2; i++) {
        char temp = label[i];
        label[i] = label[index - i - 1];
        label[index - i - 1] = temp;
    }
}

void display_sheet(int rows, int cols) {
    system("clear");
    printf("   ");
    for (int c = start_col; c < start_col + VIEWPORT_SIZE && c < cols; c++) {
        char label[4] = "";
        get_col_label(c, label);
        printf("%4s", label);
    }
    printf("\n");

    for (int r = start_row; r < start_row + VIEWPORT_SIZE && r < rows; r++) {
        printf("%3d", r + 1);
        for (int c = start_col; c < start_col + VIEWPORT_SIZE && c < cols; c++) {
            printf("%4d", sheet[r][c]);
        }
        printf("\n");
    }
}

void display_single_cell(int row, int col) {
    system("clear");
    char label[4] = "";
    get_col_label(col, label);
    printf("   %s\n", label);
    printf("%3d %4d\n", row + 1, sheet[row][col]);
}

void display_row_view(int row, int cols) {
    system("clear");
    printf("Row %d:\n", row + 1);
    printf("   ");
    for (int c = current_col; c < current_col + VIEWPORT_SIZE && c < cols; c++) {
        char label[4] = "";
        get_col_label(c, label);
        printf("%4s", label);
    }
    printf("\n%3d", row + 1);
    for (int c = current_col; c < current_col + VIEWPORT_SIZE && c < cols; c++) {
        printf("%4d", sheet[row][c]);
    }
    printf("\n");
}

void display_column_view(int col, int rows) {
    system("clear");
    char label[4] = "";
    get_col_label(col, label);
    printf("Column %s:\n", label);
    for (int r = current_row; r < current_row + VIEWPORT_SIZE && r < rows; r++) {
        printf("%3d %4d\n", r + 1, sheet[r][col]);
    }
}

void parse_cell_reference(char *input, int *row, int *col) {
    *col = 0;
    int i = 0;
    while (isalpha(input[i])) {
        *col = *col * 26 + (toupper(input[i]) - 'A' + 1);
        i++;
    }
    *col -= 1;
    *row = atoi(&input[i]) - 1;
}

void update_display(int rows, int cols) {
    if (display_cell) {
        display_single_cell(current_row, current_col);
    } else if (display_row) {
        display_row_view(current_row, cols);
    } else if (display_column) {
        display_column_view(current_col, rows);
    } else {
        start_row = current_row;
        start_col = current_col;
        display_sheet(rows, cols);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <rows> <cols>\n", argv[0]);
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    
    if (rows < 1 || rows > MAX_ROWS || cols < 1 || cols > MAX_COLS) {
        printf("Invalid spreadsheet size.\n");
        return 1;
    }

    // Initialize sheet
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            sheet[r][c] = 0;

    display_sheet(rows, cols);
    char command[50];
    
    while (1) {
        printf("Command: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;
        
        if (strcmp(command, "q") == 0) break;
        
        // Handle navigation commands
        if (strcmp(command, "w") == 0) {
            if (display_cell) {
                display_cell = 0;
                display_column = 1;
                current_row=current_row-10;
            } else if (display_column) {
                if (current_row > 0) {
                    current_row=current_row-10;
                }
            } else if (display_row) {
                display_row = 0;
                current_row=current_row-10;
            } else {
                if (current_row > 0) {
                    current_row = (current_row > VIEWPORT_SIZE) ? current_row - VIEWPORT_SIZE : 0;
                }
            }
        }
        else if (strcmp(command, "s") == 0) {
            if (display_cell) {
                display_cell = 0;
                display_column = 1;
                current_row=current_row+10;
            } else if (display_column) {
                if (current_row + 1 < rows) {
                    current_row=current_row+10;
                }
            } else if (display_row) {
                display_row = 0;
                current_row=current_row+10;
            } else {
                if (current_row + VIEWPORT_SIZE < rows) {
                    current_row += VIEWPORT_SIZE;
                }
            }
        }
        else if (strcmp(command, "a") == 0) {
            if (display_cell) {
                display_cell = 0;
                display_row = 1;
                current_col=current_col+10;
            } else if (display_row) {
                if (current_col > 0) {
                    current_col=current_col+10;
                }
            } else if (display_column) {
                display_column = 0;
                current_col=current_col+10;
            } else {
                if (current_col > 0) {
                    current_col = (current_col > VIEWPORT_SIZE) ? current_col - VIEWPORT_SIZE : 0;
                }
            }
        }
        else if (strcmp(command, "d") == 0) {
            if (display_cell) {
                display_cell = 0;
                display_row = 1;
                current_col=current_col-10;
            } else if (display_row) {
                if (current_col + 1 < cols) {
                    current_col=current_col-10;
                }
            } else if (display_column) {
                display_column = 0;
                current_col=current_col-10;
            } else {
                if (current_col + VIEWPORT_SIZE < cols) {
                    current_col += VIEWPORT_SIZE;
                }
            }
        }
        else if (strncmp(command, "scroll_to ", 10) == 0) {
            int new_row, new_col;
            parse_cell_reference(command + 10, &new_row, &new_col);
            if (new_row >= 0 && new_row < rows && new_col >= 0 && new_col < cols) {
                current_row = new_row;
                current_col = new_col;
                display_cell = 1;
                display_row = 0;
                display_column = 0;
            }
        }
        
        update_display(rows, cols);
    }
    return 0;
}