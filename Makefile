C_FLAGS =

default:	
	rm -rf *.o
	gcc $(C_FLAGS) *.c -o shell
	rm -rf *~
