// Template file for functions defined in ppm_tools.c
#ifndef _ppm_tools_h 
#define _ppm_tools_h
#include "../vgg19/vgg19.h"

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

// Function that receives a C structured  FeatureMap and write it as a true ppm image in the path "filename" given 
void writePPMFromFeatureMap(const char *filename, FeatureMap *img);

// Function to split an input image into three 1-channel feature maps. To be used on the first layer
void separateImageChannel(PPMImage *img, FeatureMap *imgs[]);


#endif // _ppm_tools_h
