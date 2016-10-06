/// Template file for functions defined in cnn_extraction.c
#ifndef _cnn_extraction_h
#define _cnn_extraction_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Structure defined to represent a CNN layer
typedef struct
{
    int kW;
    int kH;
    double *bias;
    char name[15];
    double *weight;
    int input_size;
    int output_size;
}Layer;
Layer *layer;

/// Function that stores all the parameters of CNN in memory
void cnnParameters(FILE *file);

/// Function that reads the CNN configuration file
int readFile();

#endif // _cnn_extraction_h
