FLAGS = -Wall -g -O0

all : sudoku test

sudoku : sudoku.o cnp.o
	gcc sudoku.o cnp.o -o sudoku $(FLAGS)

sudoku.o : sudoku.c
	gcc sudoku.c -o sudoku.o -c $(FLAGS)

test : test.o cnp.o
	gcc test.o cnp.o -o test $(FLAGS)

test.o : test.c cnp.h
	gcc test.c -o test.o -c $(FLAGS)
cnp.o : cnp.c cnp.h
	gcc cnp.c -o cnp.o -c $(FLAGS)

clean :
	rm -rf *.o sudoku test
