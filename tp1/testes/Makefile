CC=gcc
CFLAGS=-g3 -Wall
NAME=teste

make: 
	 $(CC) $(CFLAGS) -o $(NAME) teste.c ../utils/ppm_tools/ppm_tools.c ../utils/ppm_tools/ppm_tools.h ../utils/vgg19/vgg19.c ../utils/vgg19/vgg19.h         

run:
	./$(NAME)

clean: 
	 rm -rf teste *.ppm teste.dSYM
