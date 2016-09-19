// Template file for functions defined in ppm_tools.c
#ifndef _ppm_tools_h
#define _ppm_tools_h

// Structure defined to represent a ppm image
typedef struct {
     unsigned char red,green,blue;
} PPMPixel;

typedef struct {
     int x, y;
     PPMPixel *data;
} PPMImage;


// Definitions 
#define RGB_COMPONENT_COLOR 255
#define KRED  "\x1B[31m"

// Function that create a C structure representation of a ppm image file
PPMImage *readPPM(const char *filename);

// Function that receives a C structured  PPMImage and write it as a true ppm image in the path "filename" given 
void writePPM(const char *filename, PPMImage *img);

// Function to split an input image into three 1-channel feature maps. To be used on the first layer
void separateImageChannel(PPMImage *img, PPMImage *imgs[]);

// Sample function that changes the RGB of each pixel of the C structured ppm image
void changeColorPPM(PPMImage *img);

// Convolution function
double convolution(PPMImage *img, int kernel_size, int coord, double weights[]);

//max pooling function
PPMImage *maxPool(PPMImage *featureMap, int kernelSize, int stride);

//function to compute the max of an array
unsigned char max(unsigned char poolRegion[], int kernelSize);

//function to compute the convolution of a kernel over the image
PPMImage *convoluteKernel(PPMImage *inputImage[], double *weights, int kernelSize, int stride, int paddingSize, int depth, int numOutputs);


#endif // _ppm_tools_h
