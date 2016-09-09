// file that defines each function declared in ppm_tools header
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ppm_tools.h"


// Function that create a C structure representation of a ppm image file given as parameter @filename
PPMImage *readPPM(const char *filename)
{
  
  //variables 
  char buff[16];
  PPMImage *img;
  FILE *fp;
  int c, rgb_comp_color;

  //open PPM file for reading

  fp = fopen(filename, "rb");
  if (!fp) {
       fprintf(stderr, "%sUnable to open file '%s'\n", KRED,filename);
       exit(1);
  }

  
  //read image format, the first 2 bytes of the file. 
  if (!fgets(buff, sizeof(buff), fp)) {
       perror(filename);
       exit(1);
  }

  // check if the format is valid, it must be "P6", which is the ppm image's  magic number =P
  if (buff[0] != 'P' || buff[1] != '6') {
       fprintf(stderr, "%sInvalid image format (must be 'P6')\n", KRED);
       exit(1);
  }

  //alloc memory form image 
  img = (PPMImage *)malloc(sizeof(PPMImage));
  if (!img) {
       fprintf(stderr, "%sUnable to allocate memory\n",KRED);
       exit(1);
  }

  // The PPM format allows for comments, so we need to check if any was given
  c = getc(fp);
  while (c == '#') {
  while (getc(fp) != '\n') ;
       c = getc(fp);
  }
  ungetc(c, fp);

  //read image size information
  if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
       fprintf(stderr, "%sInvalid image size (error loading '%s')\n", KRED, filename);
       exit(1);
  }

  //read rgb component
  if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
       fprintf(stderr, "%sInvalid rgb component (error loading '%s')\n", KRED, filename);
       exit(1);
  }

  //check rgb component depth
  if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
       fprintf(stderr, "%s'%s' does not have 8-bits components\n", KRED, filename);
       exit(1);
  }
  while (fgetc(fp) != '\n') ;

  //memory allocation for pixel data,  the structured C ppm image
  img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

  // memory failure check
  if (!img) {
       fprintf(stderr, "%sUnable to allocate memory\n", KRED);
       exit(1);
  }

  //read pixel data from file, the RGB component
  if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
       fprintf(stderr, "%sError loading image '%s'\n", KRED, filename);
       exit(1);
  }

  // Close the file and return the C structured ppm image
  fclose(fp);
  return img;
}



// Function that receives a C structured  PPMImage and write it as a true ppm image in the path "filename" given 
void writePPM(const char *filename, PPMImage *img)
{

    // Open the file that will hold the ppm image
    FILE *fp;
    fp = fopen(filename, "wb");

    // check for opening failures
    if (!fp) {
         fprintf(stderr, "%sUnable to open file '%s'\n", KRED, filename);
         exit(1);
    }

    //write the header file
    //
    //image format, must be the ppm image's magic number
    fprintf(fp, "P6\n");


    //write image size
    fprintf(fp, "%d %d\n",img->x,img->y);

    // rgb component depth, which was defined as constant in ppm_tools.h
    fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

    // write the pixel data
    fwrite(img->data, 3 * img->x, img->y, fp);

    //close file
    fclose(fp);
}


// Naive function just to prove that the pixels could be changed and reorganized in ppm format
// it takes a C structured ppm image and iterate over each pixel and changes its RGB info.
void changeColorPPM(PPMImage *img)
{
    // variable
    int i;

    // check if img existsz
    if(img){

         //iterates over each image pixel
         for(i=0;i<img->x*img->y;i++){
              img->data[i].red=RGB_COMPONENT_COLOR-img->data[i].red;
              img->data[i].green=RGB_COMPONENT_COLOR-img->data[i].green;
              img->data[i].blue=RGB_COMPONENT_COLOR-img->data[i].blue;
         }
    }
}



// Convolution function
double convolution(PPMImage *img, int kernel_size, int coord, int channel, double weights[]){

  // return variable
  double convolution_result = 0.0;
 
  //kernel of sets
  int off_set = floor(kernel_size*kernel_size/2);

  // check the channel 
  
  
  for(int i = coord - off_set - 1 ; i < coord + off_set; i++ ){

    switch(channel) {
       // Red 
       case 0 :
         convolution_result +=  weights[i] * img->data[i].red;
         //img->data[i].red  = weights[i] * img->data[i].red;
         break;
       // Green
       case 1 :
         convolution_result +=  weights[i] * img->data[i].green;
         //img->data[i].green = weights[i] * img->data[i].green;
         break;
       // Blue
       case 2 :
         convolution_result +=  weights[i] * img->data[i].blue;
         //img->data[i].blue = weights[i] * img->data[i].blue;
         break;
    }

  }
 
  return convolution_result;

}  

