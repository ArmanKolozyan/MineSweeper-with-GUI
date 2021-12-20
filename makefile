all: main.o cell.o handle_input.o printing.o gui.o encoder_decoder.o
	gcc main.o cell.o handle_input.o printing.o gui.o -lSDL2 -Wall -o MineSweeper

main.o: Minesweeper.c cell.h datatypes.h handle_input.h macros.h printing.h gui.h encoder_decoder.h
	gcc Minesweeper.c -c -o main.o 

cell.o: cell.c cell.h datatypes.h macros.h
	gcc cell.c -c -o cell.o 

handle_input.o: handle_input.c handle_input.h cell.h datatypes.h macros.h 
	gcc handle_input.c -c -o handle_input.o

printing.o: printing.c printing.h datatypes.h macros.h 
	gcc printing.c -c -o printing.o 

gui.o: gui.c gui.h datatypes.h macros.h 
	gcc gui.c -c -o gui.o 

encoder_decoder.o: encoder_decoder.h datatypes.h
	gcc encoder_decoder.c -c -o encoder_decoder.o
