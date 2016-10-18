// Template file for functions defined in ppm_tools.c
#ifndef _vgg19_h
#define _vgg19_h

// Structure defined to represent a ppm image
typedef struct {
     double channel1; 
} FeatureMapPixel;

typedef struct {
     int x, y;
     FeatureMapPixel *data;
} FeatureMap;


// Definitions 
#define RGB_COMPONENT_COLOR 255
#define KRED  "\x1B[31m"

//max pooling function
void maxPool(FeatureMap *featureMap, int kernelSize, int stride);

void poolingLayer(FeatureMap *featureMap, int kernelSize, int stride, int input_size);

void fullyConnectedKernel(FeatureMap inputImage[], double *weights, int depth, FeatureMap *convolutedImage, double bias);

FeatureMap *fcLayer(FeatureMap *featureMaps, double *weights, int depth, int outputNumber, double *bias);
FeatureMap *convolutionalLayer(FeatureMap *featureMaps, double *weights, int kernelSize, int stride, int paddingSize, int depth, int outputNumber, double *bias);
//function to compute the max of an array

double max(double poolRegion[], int kernelSize);

//function to compute the convolution of a kernel over the image
void convoluteKernel(FeatureMap *inputImage[], double *weights, int kernelSize, int stride, int paddingSize, int depth, FeatureMap *convolutedImage, int initialOffset, double bias);

//generate all featuremaps for layer
FeatureMap *convolutionLayer(FeatureMap *inputImage[], double *weights, int kernelSize, int stride, int paddingSize, int depth, int outputNumber, double *bias);

//fully connected layer
FeatureMap *fullyConnectedLayer(FeatureMap inputImage[], double *weights, int depth, int outputNumber, double *bias);

// forward inside the neural network
void forward(char *filePath, FeatureMap *featureMaps[], Layer *layer);

#endif // _ppm_tools_h
