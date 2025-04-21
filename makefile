jeu: main.o perso.o
	gcc main.o perso.o -o jeu -g -lSDL -lSDL_image -lSDL_ttf -lm

main.o: main.c
	gcc -c main.c -g -I/usr/include/SDL

perso.o: perso.c
	gcc -c perso.c -g -I/usr/include/SDL
