all:
	gcc src/*.c -o CSVT -O3

debug:
	gcc src/*.c -o CSVT -g