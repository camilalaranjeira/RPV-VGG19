#include <stdio.h>
#include <stdlib.h>
#include "../include/image.h"
#include "../include/vgg19.h"
#include "../include/cnn_extraction.h"

//defining green color for unix terminal
#define KGRN  "\x1B[32m"

#define images_dir "../images/"
#define images_dir_fold1 "../images/brazilian_coffee_scenes/fold1/"

int main(){

  char picture_name[150];
  char temp_string[150];
  FeatureMap *inputChannels[3];
  FeatureMap *featureMaps;
  PPMImage *image;

  strcpy(temp_string,images_dir);
  strcat(temp_string,"brazilian_coffee_scenes/fold1_nomes.txt");
  FILE *picture_names  = fopen(temp_string, "r");

  printf("Comecei a ler os pesos e a arquitetura\n");
  // Read  VGG19 weights into layers struct
	readFile();

  printf("comecei a ler as imagens\n");
  /// Read the name of the pictures used in fold1
  while( (fscanf(picture_names,"%s", picture_name)) != EOF ){
  
    // read the ppm image into our C structure for ppm images
    printf("%s\n",picture_name);
    strcpy(temp_string,images_dir_fold1);
    strcat(temp_string,picture_name);
    strcat(temp_string,".ppm");
    printf("%s\n",temp_string);
    image = readPPM("/Users/rapha/Desktop/arceburgo.T64.B128.L4672.R4736.ppm");
    PPMImage *scale = Scale(image, (float)224/image->x, (float)224/image->y);
    //writePPM("fig_example_changed_scale.ppm",scale);
    PPMImage *zero = ZeroPad(scale, 0);
    writePPM("fig_example_changed_scale.ppm",zero);

    // First convolution layer
    separateImageChannel(zero, inputChannels);
    featureMaps = convolutionLayer(inputChannels,layer[0].weight,layer[0].kH,1,1,layer[0].input_size, layer[0].output_size, layer[0].bias);
    //FeatureMap *convolutionLayer(FeatureMap *inputImage[], double *weights, int kernelSize, int stride, int paddingSize, int depth, int outputNumber, double *bias); 
    writePPMFromFeatureMap("deep.ppm",&featureMaps[0]);
    
    
    break;
  }  

	return 0;


}
