prog:fonction_p.o main_p.o
	gcc fonction_p.o main_p.o -o prog -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf

main_p.o:main_p.c
	gcc -c main_p.c

fonction_p.o: fonction_p.c
	gcc -c fonction_p.c  
