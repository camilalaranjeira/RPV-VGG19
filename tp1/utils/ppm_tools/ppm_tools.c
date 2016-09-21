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


// Function that receives a C structured  FeatureMap and write it as a true ppm image in the path "filename" given 
void writePPMFromFeatureMap(const char *filename, FeatureMap *img)
{

    // Open the file that will hold the ppm image
    FILE *fp;
    fp = fopen(filename, "wb");

    //instance a PPMPixel
    PPMPixel *data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

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

    //convert FeatureMapPixel to PPMPixel
    int i;
    for ( i=0 ; i < img->x * img->y; i++){
      data[i].red = img->data[i].channel1;
      data[i].green = 0;
      data[i].blue = 0;
    }  

    // write the pixel data
    fwrite(data, 3 * img->x, img->y, fp);
    
    free(data);
    //close file
    fclose(fp);
}





//function to split input image into 3 separate channels (feature maps)
//each resultant feature map store values into the red channel (0s into other channels)
void separateImageChannel(PPMImage *img, FeatureMap *imgs[]) {

	int i;

	///check if image is null
	  if(img){

		  //generate one feature map for each image channel
	      FeatureMap *feature_map_img_red = (FeatureMap *)malloc(sizeof(FeatureMap));
	      FeatureMap *feature_map_img_green = (FeatureMap *)malloc(sizeof(FeatureMap));
	      FeatureMap *feature_map_img_blue = (FeatureMap *)malloc(sizeof(FeatureMap));

	      //allocate memory to store the image data
	      feature_map_img_red->data = (FeatureMapPixel*)malloc(img->x * img->y * sizeof(FeatureMapPixel));
	      feature_map_img_green->data = (FeatureMapPixel*)malloc(img->x * img->y * sizeof(FeatureMapPixel));
	      feature_map_img_blue->data = (FeatureMapPixel*)malloc(img->x * img->y * sizeof(FeatureMapPixel));

	      //set image size
	      feature_map_img_red->x = img->x;feature_map_img_red->y = img->y;
	      feature_map_img_green->x = img->x;feature_map_img_green->y = img->y;
	      feature_map_img_blue->x = img->x;feature_map_img_blue->y = img->y;


	      //iterates over each image pixel
	      for(i=0;i<img->x*img->y;i++){
	    	  feature_map_img_red->data[i].channel1 = img->data[i].red;
	    	  feature_map_img_green->data[i].channel1 = img->data[i].green;
	    	  feature_map_img_blue->data[i].channel1 = img->data[i].blue;

	      }

	      //insert into an array
	      imgs[0] = feature_map_img_red;
	      imgs[1] = feature_map_img_green;
	      imgs[2] = feature_map_img_blue;
	  }
}

