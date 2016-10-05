/// file that defines each function declared in cnn_extraction header
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cnn_extraction.h"

#define LAYERS 19
// File must be in the root directory (e.g. source)
#define PATH "vgg19_weights.txt"

/* Function adapted from http://ossguy.com/?p=284 */
int
dynamic_fgets(char** buf, signed long int* size, FILE* file)
{
    char* offset;
    int old_size;
    if(!fgets(*buf, *size, file))
        return MSG_EOF;
    if((*buf)[strlen(*buf) - 1] == '\n')
        return MSG_ENDLINE;
    do
    {
        /* we haven't read the whole line so grow the buffer */
        old_size = *size;
        *size *= 2;
        *buf = realloc(*buf, *size);
        if (NULL == *buf)
            return MSG_OUTOFMEMORY;
        offset = &((*buf)[old_size - 1]);
    }while( (fgets(offset, old_size + 1, file))
        &&  (offset[strlen(offset) - 1] != '\n'));
    return 0;
}

void
cnnParameters(char* line, char* token, int* it)
{
    /// variables
    int poz;
    int count = 0;
    /// performs the extraction of the variables contained in the file for our structure of layers
    if( !strncmp(line, "conv", 4) || !strncmp(line, "fc", 2) )
        strcpy(layer[*it].name, line);
    else if( !strncmp(line, "InputSize", 9) )
        sscanf(line, "InputSize %d", &layer[*it].input_size);
    else if( !strncmp(line, "OutputSize", 10) )
        sscanf(line, "OutputSize %d", &layer[*it].output_size);
    else if( !strncmp(line, "kW", 2) )
        sscanf(line, "kW %d", &layer[*it].kW);
    else if( !strncmp(line, "kH", 2) )
        sscanf(line, "kH %d", &layer[*it].kH);
    else if( !strncmp(line, "Bias", 4) )
        strcpy(token, "Bias");
    else if( !strncmp(line, "Weights", 7) )
        strcpy(token, "Weights");
    else if( !strncmp(token, "Bias", 4) )
    {
        layer[*it].bias = (double *) malloc(sizeof(double) * layer[*it].output_size);
        while(sscanf(line, "%lf %n", &layer[*it].bias[count], &poz))
        {
            count++;
            line += poz;
            if(count == layer[*it].output_size) break;
        }
    }
    else if( !strncmp(token, "Weights", 7) )
    {
        int max_size = layer[*it].output_size * layer[*it].input_size * layer[*it].kW * layer[*it].kH;
        layer[*it].weight = (double *) malloc(sizeof(double) * max_size);
        while(sscanf(line, "%lf %n", &layer[*it].weight[count], &poz))
        {
            count++;
            line += poz;
            if(count == max_size) break;
        }
        /// test to view the file reading process
        printf("%d %d %d %d %d %s \n", *it, layer[*it].input_size, layer[*it].output_size, layer[*it].kH, layer[*it].kW, layer[*it].name);
        (*it)++;
    }
}

int
readFile()
{
    /// variables
    int rc;
    int it = 0;
    char* line;
    char token[7];
    FILE* in_file;
    signed long int line_size = 128;
    /// allocates an initial amount of memory for file reading
    line = malloc(line_size);
    /// allocates the number of layers according to CNN used
    layer = (Layer *) malloc(sizeof(Layer) * LAYERS);
    in_file = fopen(PATH, "r");
    if(!in_file)
    {
        printf("File Unavailable.\n");
        exit(-1);
    }
    while(1)
    {
        /// read dynamically the CNN file into our C structure for layers
    	line_size = 128;
    	rc = dynamic_fgets(&line, &line_size, in_file);
        if(MSG_OUTOFMEMORY == rc)
            return rc;
        else if(MSG_EOF == rc)
            break;
        /// set the parameters for each layer of the CNN
        cnnParameters(line, token, &it);
    }
    fclose(in_file);
    free(line);
    return 0;
}

