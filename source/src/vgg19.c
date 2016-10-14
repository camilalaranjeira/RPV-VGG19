// file that defines each function declared in ppm_tools header
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/vgg19.h"
#include "../include/image.h"


// 
double max(double poolRegion[], int kernelSize) {

	int i;
	double maxValue = poolRegion[0];
	for (i=1;i<kernelSize*kernelSize;i++) {
		//printf("poolregion: %g, ", poolRegion[i]);
		if (poolRegion[i]>maxValue) {
			maxValue = poolRegion[i];
		}
	}
	return maxValue;
}


//function to perform max pooling on an array
void maxPool(FeatureMap *featureMap, int kernelSize, int stride) {

	//instantiate pooled image
	FeatureMap *pooledImage = (FeatureMap *)malloc(sizeof(FeatureMap));
	// check if img exists
	if(featureMap){

		//compute size of pooled image (feature map)
		int pooledDimension = (featureMap->x-kernelSize)/stride+1;
		//printf("pooledDimension %d", pooledDimension);

		//allocate memory for the output pooled image
		pooledImage->data = (FeatureMapPixel*)malloc(pooledDimension * pooledDimension * sizeof(FeatureMapPixel));
		pooledImage->x = pooledDimension; pooledImage->y = pooledDimension;


		//iterate over each image pixel
		//remember that each feature map contains data only on the red channel
		int i,j,k;
		int dataIndex = 0;
		double poolRegion[kernelSize*kernelSize];

		for(i=0;i<(featureMap->x*featureMap->y);i = i+stride){

			//catch pixels on the delimited region and put them into the poolregion
			for (j=0;j<kernelSize;j++) {
				for (k=0;k<kernelSize;k++) {
					poolRegion[j*kernelSize+k] = featureMap->data[i+k+j*featureMap->x].channel1;
				}
			}


			//perform the max pooling
			//printf("pooled data: %g", max(poolRegion, kernelSize));
			pooledImage->data[dataIndex].channel1 = max(poolRegion, kernelSize);
			dataIndex++;

			//performs a line offset
			if (i!=0 && i%(featureMap->x-stride)==0) {
				i = i+(kernelSize-1)*(featureMap->x);
			}

		}

	}

	featureMap->data = pooledImage->data;
	featureMap->x = pooledImage->x;
	featureMap->y = pooledImage->y;
	free(pooledImage);
}


void poolingLayer(FeatureMap *featureMap, int kernelSize, int stride,int input_size) {
	int k;

	for (k=0;k<input_size;k++) {
		maxPool(&featureMap[k], kernelSize, stride);
	}
}

FeatureMap *convolutionalLayer(FeatureMap *featureMaps, double *weights, int kernelSize, int stride, int paddingSize, int depth, int outputNumber, double *bias) {
	int k=0;
  for (k=0;k<depth;k++) {
  	featureMapZeroPad(&featureMaps[k], 1);
  }

  //convertendo pra um ponteiro de ponteiro
  FeatureMap *fMaps[depth];
  for (k=0;k<depth;k++) {
	  fMaps[k] = &featureMaps[k];
  }
  featureMaps = convolutionLayer(fMaps,weights,kernelSize,1,1,depth, outputNumber, bias);

  return featureMaps;
}

FeatureMap *fcLayer(FeatureMap *featureMaps, double *weights, int depth, int outputNumber, double *bias) {

   //convertendo pra um ponteiro de ponteiro
   int k=0;
   FeatureMap *fMaps[depth];
   for (k=0;k<depth;k++) {
	   fMaps[k] = &featureMaps[k];
   }
   featureMaps = fullyConnectedLayer(fMaps, weights, depth, outputNumber, bias);

   return featureMaps;
}


//function that convolutes a kernel over the image
void convoluteKernel(FeatureMap *inputImage[], double *weights, int kernelSize, int stride, int paddingSize, int depth, FeatureMap *convolutedImage, int initialOffset, double bias) {

	//x and y dimensions are equal
	FeatureMap *sampleImage = inputImage[0];
	int convDimension = (sampleImage->x-kernelSize/*+2*paddingSize*/)/stride+1;

	//allocate memory for the output convoluted image
	convolutedImage->data = (FeatureMapPixel*)malloc(convDimension * convDimension * sizeof(FeatureMapPixel));
	convolutedImage->x = convDimension; convolutedImage->y = convDimension;

	//iterate over each image pixel
	int i,j,z,k;
	int lineCounter = 0;
	int weightOffset = kernelSize*kernelSize;
	int paddingOffset = (paddingSize*sampleImage->x) + paddingSize; //isso daqui reflete os zeros inseridos

	int checker = 0;

	//varre só nos pixels e desconsidera os 0s do padding
	double pixValue;
	for(i=paddingOffset+initialOffset;i<(sampleImage->x * sampleImage->y)-(paddingOffset+initialOffset);i = i+stride){

		//varre os diferentes feature maps
		pixValue = 0;
		for (z=0;z<depth;z++) {
			int weightIndex = 0;
			for (k=-1*kernelSize/2;k<=kernelSize/2;k++) {
				for (j=-1*kernelSize/2;j<=kernelSize/2;j++) {
					pixValue += inputImage[z]->data[(k)*inputImage[z]->x+i+j].channel1*weights[z*weightOffset+weightIndex]; 
					weightIndex++;
				}
			}
		}// end for Z
		pixValue += bias;
		if (pixValue<0) { //isso é a reLU
			pixValue = 0;
		}

		//if (depth==512)
		//	printf("pixValue: %g\n", pixValue);

		convolutedImage->data[checker].channel1 = pixValue;

		checker++;
		if (checker%(convDimension)==0) {
			if (stride==1)
				i = i+2*paddingSize;
			else {
				i = i+paddingSize;
				i = i+(stride-1)*sampleImage->x;
			}
			lineCounter++;
		}
  }// end for i
}


void fullyConnectedKernel(FeatureMap *inputImage[], double *weights, int depth, FeatureMap *convolutedImage, double bias) {

	//x and y dimensions are equal
	FeatureMap *sampleImage = inputImage[0];
	int kernelSize = sampleImage->x;
	int stride = 1;
	int convDimension = (sampleImage->x-kernelSize/*+2*paddingSize*/)/stride+1;
	int weightOffset = kernelSize*kernelSize;

	//allocate memory for the output convoluted image
	convolutedImage->data = (FeatureMapPixel*)malloc(convDimension * convDimension * sizeof(FeatureMapPixel));
	convolutedImage->x = convDimension; convolutedImage->y = convDimension;

	double pixValue;
	int i;
	for(i=0;i<(sampleImage->x * sampleImage->y); i= i+stride){

		//varre os diferentes feature maps
		pixValue = 0;
		int z;
		for (z=0;z<depth;z++) {
			int weightIndex = 0;
				pixValue += inputImage[z]->data[i].channel1*weights[z*weightOffset+weightIndex];
				weightIndex++;

		}// end for Z

		pixValue += bias;
		if (pixValue<0) { //isso é a reLU
			pixValue = 0;
		}

		//printf("pixValue: %g\n", pixValue);
	}
	//essas camadas só geram 1 pixel
	convolutedImage->data[0].channel1 = pixValue;
}


// Generate all featuremaps for the layer
FeatureMap *convolutionLayer(FeatureMap *inputImage[], double *weights, int kernelSize, int stride, int paddingSize, int depth, int outputNumber, double *bias){
    
    FeatureMap *featuremaps = malloc(outputNumber * sizeof(FeatureMap));

    //Generate all featuremaps 
    int i;
    for(i = 0; i < outputNumber; i++){
      double *updated_weight  = weights + (kernelSize * depth * kernelSize * i);
      convoluteKernel(inputImage,updated_weight,kernelSize,stride,paddingSize,depth, &featuremaps[i],0, bias[i]);
    }  
    return featuremaps;
}  


// Fully connected layer
FeatureMap *fullyConnectedLayer(FeatureMap *inputImage[], double *weights, int depth, int outputNumber, double *bias){

    FeatureMap *featuremaps = malloc(outputNumber * sizeof(FeatureMap));

    // Get the kernel size based on the first image from the input
    int kernelSize = inputImage[0]->x;

    //Generate all featuremaps 
    int i;
    for(i = 0; i < outputNumber; i++){
      double *updated_weight  = weights + (kernelSize * depth * kernelSize * i);
      fullyConnectedKernel(inputImage,updated_weight,depth, &featuremaps[i],bias[i]);
      //convoluteKernel(inputImage,updated_weight,kernelSize,1,0,depth, &featuremaps[i],(kernelSize * kernelSize)/2,bias[i]);
    }  
    
    return featuremaps;

}  





