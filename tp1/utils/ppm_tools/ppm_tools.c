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


unsigned char max(unsigned char poolRegion[], int kernelSize) {

	int i;
	unsigned char maxValue = poolRegion[0];
	for (i=1;i<kernelSize*kernelSize;i++) {
		if (poolRegion[i]>maxValue) {
			maxValue = poolRegion[i];
		}
	}

	return maxValue;
}


//function to perform max pooling on an array
PPMImage *maxPool(PPMImage *featureMap, int kernelSize, int stride) {

	//instantiate pooled image
	PPMImage *pooledImage = (PPMImage *)malloc(sizeof(PPMImage));


	// check if img exists
	if(featureMap){

		//compute size of pooled image (feature map)
		int pooledDimension = (featureMap->x-kernelSize)/stride+1;


		//allocate memory for the output pooled image
		pooledImage->data = (PPMPixel*)malloc(pooledDimension * pooledDimension * sizeof(PPMPixel));
		pooledImage->x = pooledDimension; pooledImage->y = pooledDimension;


		//iterate over each image pixel
		//remember that each feature map contains data only on the red channel
		int i,j,k;
		int dataIndex = 0;
		unsigned char poolRegion[kernelSize*kernelSize];

		for(i=0;i<(featureMap->x*featureMap->y);i = i+stride){

			//catch pixels on the delimited region and put them into the poolregion
			for (j=0;j<kernelSize;j++) {
				for (k=0;k<kernelSize;k++) {
					poolRegion[j*kernelSize+k] = featureMap->data[i+k+j*featureMap->x].red;
				}
			}


			//perform the max pooling
			pooledImage->data[dataIndex].red = max(poolRegion, kernelSize);
			dataIndex++;

			//performs a line offset
			if (i!=0 && i%(featureMap->x-stride)==0) {
				i = i+(kernelSize-1)*(featureMap->x);
			}

		}

	}
	return pooledImage;
}

//function that convolutes a kernel over the image
PPMImage *convoluteKernel(PPMImage *inputImage[], double *weights, int kernelSize, int stride, int paddingSize, int depth, int numOutputs) {

	//instantiate a convoluted image
	PPMImage *convolutedImage = (PPMImage *)malloc(sizeof(PPMImage));

	//compute size of convoluted image (feature map)
	//x and y dimensions are equal
	PPMImage *sampleImage = inputImage[0];
	int convDimension = (sampleImage->x-kernelSize+2*paddingSize)/stride+1;

	//allocate memory for the output convoluted image
	convolutedImage->data = (PPMPixel*)malloc(convDimension * convDimension * sizeof(PPMPixel));
	convolutedImage->x = convDimension; convolutedImage->y = convDimension;

	//iterate over each image pixel
	int i,j,z;
	int weightOffset = kernelSize*kernelSize;
	int stepOffset = weightOffset/2;


	int paddingOffset = (paddingSize*sampleImage->x) + paddingSize; //isso daqui reflete os zeros inseridos

		//varre só nos pixels e desconsidera os 0s do padding
		for(i=paddingOffset;i<(sampleImage->x*sampleImage->y)-paddingOffset;i = i+stride){

			//varre os diferentes feature maps
			for (z=0;z<depth;z++) {

				int weightIndex = 0;
				int line = 0;
				double pixValue = 0;

				for (j=-1*stepOffset;j<=stepOffset;j++) {
					pixValue += inputImage[z]->data[line*sampleImage->x+i+j].red*weights[z*weightOffset+weightIndex];

					weightIndex++;
					if ((j+1)%(kernelSize)==0) { //tem que deslocar esse índice pra acessar a posição certa (linha) dos dados da imagem
						line++;
					}

				}
				convolutedImage->data[i-paddingOffset].red = pixValue;

				if (convolutedImage->data[i-paddingOffset].red<0) {
					convolutedImage->data[i-paddingOffset].red = 0; //reLu
				}
			}


		}

	return convolutedImage;
}


//function to split input image into 3 separate channels (feature maps)
//each resultant feature map store values into the red channel (0s into other channels)
void separateImageChannel(PPMImage *img, PPMImage *imgs[]) {

	int i;

	///check if image is null
	  if(img){

		  //generate one feature map for each image channel
	      PPMImage *feature_map_img_red = (PPMImage *)malloc(sizeof(PPMImage));
	      PPMImage *feature_map_img_green = (PPMImage *)malloc(sizeof(PPMImage));
	      PPMImage *feature_map_img_blue = (PPMImage *)malloc(sizeof(PPMImage));

	      //allocate memory to store the image data
	      feature_map_img_red->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));
	      feature_map_img_green->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));
	      feature_map_img_blue->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

	      //set image size
	      feature_map_img_red->x = img->x;feature_map_img_red->y = img->y;
	      feature_map_img_green->x = img->x;feature_map_img_green->y = img->y;
	      feature_map_img_blue->x = img->x;feature_map_img_blue->y = img->y;


	      //iterates over each image pixel
	      for(i=0;i<img->x*img->y;i++){
	    	  feature_map_img_red->data[i].red = img->data[i].red;
	    	  feature_map_img_green->data[i].red = img->data[i].green;
	    	  feature_map_img_blue->data[i].red = img->data[i].blue;

	    	  //insert 0s into other channels
	    	  feature_map_img_red->data[i].green = 0;
	          feature_map_img_green->data[i].green = 0;
	    	  feature_map_img_blue->data[i].green = 0;

	    	  feature_map_img_red->data[i].blue = 0;
	    	  feature_map_img_green->data[i].blue = 0;
	    	  feature_map_img_blue->data[i].blue = 0;

	      }

	      //insert into an array
	      imgs[0] = feature_map_img_red;
	      imgs[1] = feature_map_img_green;
	      imgs[2] = feature_map_img_blue;
	  }
}


// Convolution function
double convolution(PPMImage *img, int kernel_size, int coord, double weights[]){

  // return variable
  double convolution_result = 0.0;
 
  //kernel of sets
  int off_set = floor(kernel_size*kernel_size/2);

  
  for(int i = coord - off_set - 1 ; i < coord + off_set; i++ ){

    convolution_result +=  weights[i] * img->data[i].red;
    convolution_result +=  weights[i+9] * img->data[i].green;
    convolution_result +=  weights[i+18] * img->data[i].blue;

  }
 
  // Return as ReLU
  return (convolution_result <= 0) ? 0 : convolution_result;

}



// convolution layer
//PPMImage *img[] convolution_layer(PPMImage *img, int channel_depth, int stride, int kernel_size, int output_size, double weights[]) { 

  // Variables (lembrar de dar Free depois) 
  //PPMImage *imgs[] = (PPMImage *)malloc(sizeof(PPMImage) * output_size);
  //int i,j;

  //// check if img exisits
  //if(img){

  //  for(j=0; j < channel_depth; j++){

  //    PPMImage *feature_map_img;
  //    
  //    //iterates over each image pixel
  //    for(i=0;i<img->x*img->y;i++){
  //          
  //         // calls convolution for each pixel (channels are not colors anymore)
  //         feature_map_img->data[i].red = convolution(img, kernel_size, i, weights);
  //    }
  //
  //    // here feature_map is ready
  //    imgs[j] = feature_map_img;
  //  } 
  //}

  
//}  


