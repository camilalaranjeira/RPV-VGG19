/// Template file for functions defined in ppm_tools.c
#ifndef _cnn_extraction_h
#define _cnn_extraction_h

/// Structure defined to represent a error or warning return
enum ALERT
{
    MSG_EOF = 1,
    MSG_ENDLINE,
    MSG_OUTOFMEMORY
};

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

/// Function that dynamically read a file with variable-length lines
int dynamic_fgets(char** buf, signed long int* size, FILE* file);

/// Function that stores all the parameters of CNN in memory
void cnnParameters(char* line, char* token, int* it);

/// Function that reads the CNN configuration file
int readFile();

#endif // _cnn_extraction_h
