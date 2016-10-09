IDIR =../include
CFLAGS=-I$(IDIR)

_DEPS = image.h \
		vgg19.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

$(ODIR)/%.o: %.c $(DEPS)
	gcc -Wall -g3 -c -o $@ $< $(CFLAGS)

main: image.c main.c
	gcc -lm -Wall -g3 -pg -o vgg19 image.c main.c vgg19.c cnn_extraction.c -I.
	gcc -lm -Wall -g3 -pg -o vgg19-read image.c teste_read.c vgg19.c cnn_extraction.c -I.
