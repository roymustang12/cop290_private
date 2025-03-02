# Makefile for Spreadsheet Program and Test Runner

CC = gcc
CFLAGS = -Wall -Wextra -g -lm
DEPS = dependency_graph_final.h input.h cell.h hash_table.h

all: sheet test_runner

sheet: driver.c dependency_graph_final.c input.c hash_table.c $(DEPS)
	$(CC) -o sheet driver.c dependency_graph_final.c input.c hash_table.c $(CFLAGS)

test_runner: test_runner.c
	$(CC) -o test_runner test_runner.c $(CFLAGS)

test: sheet test_runner
	./test_runner test_cases.txt expected_output.txt
	
report:
	pdflatex report.tex
	pdflatex report.tex  # Run twice to fix references

clean:
	rm -f sheet test_runner output.txt
