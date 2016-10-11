// file that defines each function declared in ppm_tools header
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/vgg19.h"

// 
double max(double poolRegion[], int kernelSize) {

	int i;
	double maxValue = poolRegion[0];
	for (i=1;i<kernelSize*kernelSize;i++) {
		if (poolRegion[i]>maxValue) {
			maxValue = poolRegion[i];
		}
	}

	return maxValue;
}


//function to perform max pooling on an array
FeatureMap *maxPool(FeatureMap *featureMap, int kernelSize, int stride) {

	//instantiate pooled image
	FeatureMap *pooledImage = (FeatureMap *)malloc(sizeof(FeatureMap));


	// check if img exists
	if(featureMap){

		//compute size of pooled image (feature map)
		int pooledDimension = (featureMap->x-kernelSize)/stride+1;


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
			pooledImage->data[dataIndex].channel1 = max(poolRegion, kernelSize);
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
//function that convolutes a kernel over the image
void convoluteKernel(FeatureMap *inputImage[], double *weights, int kernelSize, int stride, int paddingSize, int depth, FeatureMap *convolutedImage, int initialOffset, double bias) {

	//x and y dimensions are equal
	FeatureMap *sampleImage = inputImage[0];
	int convDimension = (sampleImage->x-kernelSize/*+2*paddingSize*/)/stride+1;

	//allocate memory for the output convoluted image
	convolutedImage->data = (FeatureMapPixel*)malloc(convDimension * convDimension * sizeof(FeatureMapPixel));
	convolutedImage->x = convDimension; convolutedImage->y = convDimension;


	//printf("convDimension %d\n", convDimension);
	//printf("sample dimension %d\n", sampleImage->x);
	//getchar();


	//iterate over each image pixel
	int i,j,z,k;
	int lineCounter = 0;
	int weightOffset = kernelSize*kernelSize;
	int paddingOffset = (paddingSize*sampleImage->x) + paddingSize; //isso daqui reflete os zeros inseridos

	//printf("paddingOffset %d\n", paddingOffset);
	//getchar();


	int checker = 0;
    //varre só nos pixels e desconsidera os 0s do padding
	for(i=paddingOffset+initialOffset;i<(sampleImage->x * sampleImage->y)-(paddingOffset+initialOffset);i = i+stride){

		//varre os diferentes feature maps
		for (z=0;z<depth;z++) {

			int weightIndex = 0;
			double pixValue = 0;

			for (k=-1*kernelSize/2;k<=kernelSize/2;k++) {
				for (j=-1*kernelSize/2;j<=kernelSize/2;j++) {
					pixValue += inputImage[z]->data[(k)*inputImage[z]->x+i+j].channel1*weights[z*weightOffset+weightIndex]; 
					weightIndex++;
				}
			}

      // Add the bias 
      pixValue = pixValue + bias;
    
      //if (bias != 0) {  
      //  printf("Pixel value: %f\n", pixValue);
      //}     
      
		  convolutedImage->data[checker].channel1 = pixValue;
		  if (convolutedImage->data[checker].channel1<0) {
		  	convolutedImage->data[checker].channel1 = 0; //reLu
		  }

		}// end for Z

		checker++;

		if (checker%(convDimension)==0) {
			i = i+2*paddingSize;
			lineCounter++;
		}



  }// end for i

}



// Generate all featuremaps for the layer
FeatureMap *convolutionLayer(FeatureMap *inputImage[], double *weights, int kernelSize, int stride, int paddingSize, int depth, int outputNumber, double *bias){
    
    FeatureMap *featuremaps = malloc(outputNumber * sizeof(FeatureMap));

    //Generate all featuremaps 
    for(int i = 0; i < outputNumber; i++){

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
    for(int i = 0; i < outputNumber; i++){

      double *updated_weight  = weights + (kernelSize * depth * kernelSize * i);
      convoluteKernel(inputImage,updated_weight,kernelSize,1,0,depth, &featuremaps[i],(kernelSize * kernelSize)/2,bias[i]);
        
    }  
    
    return featuremaps;


}  





