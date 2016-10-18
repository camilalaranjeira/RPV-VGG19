/// file that defines each function declared in cnn_extraction header
#include "../include/cnn_extraction.h"

#define LAYERS 19
// File must be in the root directory (e.g. source)
#define PATH "vgg19_weights.txt"

void
cnnParameters(FILE* file, Layer *layer)
{
    /// variables
    int count;
    int it = 0;
    char str[150];
    int max_size;
    while( (fscanf(file,"%s", str)) != EOF )
    {
        /// performs the extraction of the variables contained in the file for our structure of layers
        if( !strncmp(str, "conv", 4) || !strncmp(str, "fc", 2) )
            strcpy(layer[it].name, str);
        else if( !strncmp(str, "InputSize", 9) )
            fscanf(file, "%d", &layer[it].input_size);
        else if( !strncmp(str, "OutputSize", 10) )
            fscanf(file, "%d", &layer[it].output_size);
        else if( !strncmp(str, "kW", 2) )
            fscanf(file, "%d", &layer[it].kW);
        else if( !strncmp(str, "kH", 2) )
            fscanf(file, "%d", &layer[it].kH);
        else if( !strncmp(str, "Bias", 4) )
        {
            layer[it].bias = (double *) malloc(sizeof(double) * layer[it].output_size);
            while(fscanf(file, "%lf", &layer[it].bias[count]))
            {
                count++;
                if(count == layer[it].output_size) break;
            }
        }
        else if( !strncmp(str, "Weights", 7) )
        {
            if(!strncmp(layer[it].name, "fc", 2))
                max_size = layer[it].output_size * layer[it].input_size;
            else
                max_size = layer[it].output_size * layer[it].input_size * layer[it].kW * layer[it].kH;
            layer[it].weight = (double *) malloc(sizeof(double) * max_size);
            while(fscanf(file, "%lf", &layer[it].weight[count]))
            {
                count++;
                if(count == max_size) break;
            }
            it++;
        }
        count = 0;
    }
}

Layer *
readFile()
{
    /// variables
    FILE* in_file;
    Layer *layer;
    /// allocates the number of layers according to CNN used
    layer = (Layer *) malloc(sizeof(Layer) * LAYERS);
    in_file = fopen(PATH, "r");
    if(!in_file)
    {
        printf("File Unavailable.\n");
        exit(-1);
    }
    /// read the CNN file and set the parameters for each layer
    cnnParameters(in_file,layer);
    fclose(in_file);
    return layer;
}
