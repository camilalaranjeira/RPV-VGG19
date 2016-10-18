// file that defines each function declared in ppm_tools header
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/cnn_extraction.h"
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

  int i;
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

    // initicializa tudo com zero
    for(i = 0; i < pooledDimension * pooledDimension; i++){
      pooledImage->data[i].channel1 = 0.0;
    }


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

   featureMaps = fullyConnectedLayer(featureMaps, weights, depth, outputNumber, bias);

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

  // inicializa todos os pixel com zero
  for(i = 0; i < convDimension * convDimension; i++){
    convolutedImage->data[i].channel1 = 0.0;
  }


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


void fullyConnectedKernel(FeatureMap inputImage[], double *weights, int depth, FeatureMap *convolutedImage, double bias) {

	//x and y dimensions are equal
	FeatureMap sampleImage = inputImage[0];
	int kernelSize = sampleImage.x;
	int stride = 1;
	int convDimension = (sampleImage.x-kernelSize/*+2*paddingSize*/)/stride+1;
	int weightOffset = kernelSize*kernelSize;

	//allocate memory for the output convoluted image
	convolutedImage->data = (FeatureMapPixel*)malloc(convDimension * convDimension * sizeof(FeatureMapPixel));
	convolutedImage->x = convDimension; convolutedImage->y = convDimension;

	int i;
  for(i = 0; i < convDimension * convDimension; i++){
    convolutedImage->data[i].channel1 = 0.0;
  }  

	double pixValue;
  pixValue = 0.0;
	for(i=0;i<(sampleImage.x * sampleImage.y); i= i+stride){

		//varre os diferentes feature maps
		int z;
		for (z=0;z<depth;z++) {
			int weightIndex = 0;
				pixValue += inputImage[z].data[i].channel1*weights[z*weightOffset+weightIndex];
				//pixValue += inputImage[z].data[i].channel1;
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

    //printf("Libernado memoria convolution\n");
    //for(i=0; i < depth; i++){
    //  printf("%d \n", i);
    //  printf("%g \n", inputImage[i]->data[0].channel1);
    //}  
    //printf("Terminei memoria convolution\n");
    return featuremaps;
}  


// Fully connected layer
FeatureMap *fullyConnectedLayer(FeatureMap inputImage[], double *weights, int depth, int outputNumber, double *bias){

    FeatureMap *featuremaps = malloc(outputNumber * sizeof(FeatureMap));

    // Get the kernel size based on the first image from the input
    int kernelSize = inputImage[0].x;


    //Generate all featuremaps 
    int i;
    for(i = 0; i < outputNumber; i++){
      double *updated_weight = weights + (kernelSize * depth * kernelSize * i);
      fullyConnectedKernel(inputImage,updated_weight,depth, &featuremaps[i],bias[i]);
    }  
    
    return featuremaps;

}  

// function that recives an input image and pass it into the neural network
void forward(char *filePath, FeatureMap *featureMaps[], Layer *layer){

    PPMImage *image;

    // read the ppm image into our C structure for ppm images
    image = readPPM(filePath);

    FeatureMap *imgs[3];
    PPMImage *scale = Scale(image, (float)224/image->x, (float)224/image->y);
    PPMImage *zero = ZeroPad(scale, 1);
    separateImageChannel(zero,imgs);


    //#############Primeiro Bloco##########///////////////////
    printf("Primeiro bloco de camadas convolucionais\n");
    featureMaps[0] = convolutionLayer(imgs,layer[0].weight,layer[0].kH,1,1,layer[0].input_size, layer[0].output_size, layer[0].bias);
    featureMaps[1] = convolutionalLayer(featureMaps[0], layer[1].weight,layer[1].kH,1,1,layer[1].input_size, layer[1].output_size, layer[1].bias);
    poolingLayer(featureMaps[1], 2,2, layer[1].input_size);

    //#############Segundo Bloco##########///////////////////
    printf("Segundo bloco de camadas convolucionais\n");
    featureMaps[2] = convolutionalLayer(featureMaps[1], layer[2].weight,layer[2].kH,1,1,layer[2].input_size, layer[2].output_size, layer[2].bias);
    featureMaps[3] = convolutionalLayer(featureMaps[2], layer[3].weight,layer[3].kH,1,1,layer[3].input_size, layer[3].output_size, layer[3].bias);
    poolingLayer(featureMaps[3], 2,2, layer[3].input_size);

    //#############Terceiro Bloco##########///////////////////
    printf("Terceiro bloco de camadas convolucionais\n");
    featureMaps[4] = convolutionalLayer(featureMaps[3], layer[4].weight,layer[4].kH,1,1,layer[4].input_size, layer[4].output_size, layer[4].bias);
    featureMaps[5] = convolutionalLayer(featureMaps[4], layer[5].weight,layer[5].kH,1,1,layer[5].input_size, layer[5].output_size, layer[5].bias);
    featureMaps[6] = convolutionalLayer(featureMaps[5], layer[6].weight,layer[6].kH,1,1,layer[6].input_size, layer[6].output_size, layer[6].bias);
    featureMaps[7] = convolutionalLayer(featureMaps[6], layer[7].weight,layer[7].kH,1,1,layer[7].input_size, layer[7].output_size, layer[7].bias);
    poolingLayer(featureMaps[7], 2,2, layer[7].input_size);

    //#############Quarto Bloco##########///////////////////
    printf("Quarto bloco de camadas convolucionais\n");
    featureMaps[8] = convolutionalLayer(featureMaps[7], layer[8].weight,layer[8].kH,1,1,layer[8].input_size, layer[8].output_size, layer[8].bias);
    featureMaps[9] = convolutionalLayer(featureMaps[8], layer[9].weight,layer[9].kH,1,1,layer[9].input_size, layer[9].output_size, layer[9].bias);
    featureMaps[10] = convolutionalLayer(featureMaps[9], layer[10].weight,layer[10].kH,1,1,layer[10].input_size, layer[10].output_size, layer[10].bias);
    featureMaps[11] = convolutionalLayer(featureMaps[10], layer[11].weight,layer[11].kH,1,1,layer[11].input_size, layer[11].output_size, layer[11].bias);
    poolingLayer(featureMaps[11], 2,2, layer[11].input_size);

    //#############Quinto Bloco##########///////////////////
    printf("Quinto bloco de camadas convolucionais\n");
    featureMaps[12] = convolutionalLayer(featureMaps[11], layer[12].weight,layer[12].kH,1,1,layer[12].input_size, layer[12].output_size, layer[12].bias);
    featureMaps[13] = convolutionalLayer(featureMaps[12], layer[13].weight,layer[13].kH,1,1,layer[13].input_size, layer[13].output_size, layer[13].bias);
    featureMaps[14] = convolutionalLayer(featureMaps[13], layer[14].weight,layer[14].kH,1,1,layer[14].input_size, layer[14].output_size, layer[14].bias);
    featureMaps[15] = convolutionalLayer(featureMaps[14], layer[15].weight,layer[15].kH,1,1,layer[15].input_size, layer[15].output_size, layer[15].bias);
    poolingLayer(featureMaps[15], 2,2, layer[15].input_size);

    //#############Camadas FC##########///////////////////
    printf("Camadas Fully-Connected\n");
    featureMaps[16] = fcLayer(featureMaps[15], layer[16].weight, 512, layer[16].output_size, layer[16].bias);
    featureMaps[17] = fcLayer(featureMaps[16], layer[17].weight, 4096, layer[17].output_size, layer[17].bias);

}





