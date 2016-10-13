#ifndef INCLUDE_IMAGE_H_
#define INCLUDE_IMAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vgg19.h"

#define MSG1  "Cannot allocate memory space in"
#define MSG2  "Cannot open file in"

// Structure defined to represent a ppm image
typedef struct {
     unsigned char red,green,blue;
} PPMPixel;

typedef struct {
     int x, y;
     PPMPixel *data;
     int * tbrow;
} PPMImage;

//Just a structure to facilitate references to pixels (x,y)
typedef struct _pixel {
  int x,y;
} Pixel;

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

void Error(char *msg,char *func);
PPMImage *CreateImage(int ncols, int nrows);
bool    ValidPixel(PPMImage *img, int x, int y);
PPMImage *Scale(PPMImage *img, float Sx, float Sy);
PPMImage  *ZeroPad(PPMImage *img, int n);
void featureMapZeroPad(FeatureMap *img, int n);


#endif /* INCLUDE_IMAGE_H_ */
