CC=gcc
CFLAGS=-g3 -Wall
NAME=teste

make: 
	 $(CC) $(CFLAGS) -o $(NAME) teste.c ../../source/src/image.c ../../source/include/image.h ../../source/src/vgg19.c ../../source/include/vgg19.h         

run:
	./$(NAME)

clean: 
	 rm -rf teste *.ppm teste.dSYM
