IDIR =../include
CFLAGS=-I$(IDIR)

_DEPS = image.h \
		vgg19.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

$(ODIR)/%.o: %.c $(DEPS)
	gcc -c -o $@ $< $(CFLAGS)

main: image.c main.c
	gcc -lm -o vgg19 image.c main.c vgg19.c cnn_extraction.c -I.
