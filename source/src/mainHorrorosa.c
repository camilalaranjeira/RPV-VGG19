#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include "../include/image.h"
#include "../include/vgg19.h"
#include "../include/cnn_extraction.h"
#include <time.h>

//defining green color for unix terminal
#define KGRN  "\x1B[32m"
#define K_FOLD 5
#define DATASET_DIR "../dataset"

//defining green color for unix terminal
#define KGRN  "\x1B[32m"


void forward(char *filePath, FeatureMap *featureMaps[]){

    PPMImage *image;
    //int i;

    // read the ppm image into our C structure for ppm images
    image = readPPM(filePath);

    FeatureMap *imgs[3];
    PPMImage *scale = Scale(image, (float)224/image->x, (float)224/image->y);
    //writePPM("fig_example_changed_scale.ppm",scale);
    PPMImage *zero = ZeroPad(scale, 1);
    separateImageChannel(zero,imgs);


    clock_t tempoInicial = clock();
    //#############Primeiro Bloco##########///////////////////
    printf("Primeiro bloco de camadas convolucionais %d \n", layer[0].input_size);
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

    //##########Computando tempo de execução########////////////////
    clock_t tempoFinal = clock();
    double tempoTotal = (double)(tempoFinal - tempoInicial) / CLOCKS_PER_SEC;
    printf("Tempo total de execução do sistema por imagem: %f  %g\n", tempoTotal, featureMaps[17][4095].data[0].channel1);

    //return featureMaps;
}

int main() {
    /// variables

	  int it;
    int i,j;
    int numMaps = 4096;
    char image_path[150];
    char dataset_dir[150];
    char picture_name[150];
    char feature_path[150];
    double featureVector[4096];
    FILE* feature_file;
    int fileIndex;
    FILE* picture_file; 
    char picture_names[610][150];
    int picture_label[600];
    FeatureMap *featureMaps[18];

    /// reads the parameters of VGG 19


    printf("Lendo arquivo de pesos\n");
    readFile();
    printf("Terminada a leitura de pesos\n");
    /// reads iteratively all the images in the coffee dataset


    // primeiro dataset

    //for(it = 4; it <= K_FOLD; it++)
    //for(it = 1; it <= 1; it++)
    //{

    //  i=0;
    //	sprintf(dataset_dir, "../images/brazilian_coffee_scenes/fold%d.txt", it);
    //  //sprintf(feature_path, "../images/brazilian_coffee_scenes/fold%d/features.txt", it);
    //  //feature_file = fopen(feature_path, "a");
    //	picture_file = fopen(dataset_dir, "r");
    //	while( (fscanf(picture_file, "%s", picture_name)) != EOF ){
    //    	//printf("\nfileString: %s\n", picture_name);
    //    	if(!strncmp(picture_name, "coffee", 6))
    //        {
    //            //picture_name = picture_name + 7;
    //            picture_label[i] = 1;
    //            memmove(picture_name, picture_name + 7, sizeof(picture_name)/sizeof(char));
    //        }
    //        else
    //        {
    //            picture_label[i] = 0;
    //            memmove(picture_name, picture_name + 10, sizeof(picture_name)/sizeof(char));
    //        }
    //    	 sprintf(image_path, "../images/brazilian_coffee_scenes/fold%d/%s.ppm", it, picture_name);
    //       strcpy(picture_names[i],image_path);
    //       i++;

    //  }
    //  fclose(picture_file);
    //  //fprintf(feature_file, "\n");

    //  #pragma omp parallel for private(i,j,featureVector,featureMaps,feature_path,feature_file,fileIndex)
    //  for (i = 0; i < 476; i++){
    //    printf("%d Testando: %s\n", i, picture_names[i]);

    //    forward(picture_names[i],featureMaps);

    //    for (j = 0; j < numMaps; j++) {

    //    	//FeatureMap map = fmaps[17][j];
    //    	featureVector[j] = featureMaps[17][j].data[0].channel1;

    //    }

    //    // libera fmaps
    //    free(*featureMaps);


    //    sprintf(feature_path, "../images/brazilian_coffee_scenes/fold%d_features/features%d.txt", it,i);
    //    feature_file = fopen(feature_path, "w");

    //    
    //   fprintf(feature_file, "%d", picture_label[i]);
    //    for (fileIndex = 0; fileIndex < numMaps; fileIndex++) {
    //      //printf("numero %d: %lf\n", fileIndex, featureVector[fileIndex]);
    //    	fprintf(feature_file, ",%.15f", featureVector[fileIndex]);
    //    }

    //    fclose(feature_file);
    //    
    //  }  
    //}

	
    // SEGUNDO DATA SET //		
    
    //labels do segundo data set
    char * landUse[] = {  "agricultural","airplane","baseballdiamond","beach","buildings","chaparral","denseresidential","forest","freeway","golfcourse","harbor","intersection","mediumresidential","mobilehomepark","overpass","parkinglot","river","runway","sparseresidential","storagetanks","tenniscourt"};

    //for(it = 4; it <= K_FOLD; it++)
    for(it = 17; it < 18; it++)
    {
	
      // itera sobre as 100 images de cada categoria
      #pragma omp parallel for private(i,j,featureVector,featureMaps,feature_path,feature_file,fileIndex,image_path)
      for (i = 0; i < 100; i++){

	  printf("Teste: %02d\n", i);
          sprintf(image_path, "../images/UCMerced_LandUse/Images/%s/%s%02d.ppm", landUse[it],landUse[it],i);
          //sprintf(image_path, "../images/UCMerced_LandUse/Images/%s/%s55.ppm", landUse[it],landUse[it],i);
      	  printf("%d Testando: %s\n", i, image_path);

      	  forward(image_path,featureMaps);

      	  for (j = 0; j < numMaps; j++) {

      	  	//FeatureMap map = fmaps[17][j];
      	  	featureVector[j] = featureMaps[17][j].data[0].channel1;

      	  }

      	  // libera fmaps
      	  for(j=1; j < 18; j++){
	    int k;
	    for(k=0; k < layer[j].output_size; k++){
            	free(featureMaps[j][k].data);
	    } 
            free(featureMaps[j]);
	  }
	  free(*featureMaps);

      	  sprintf(feature_path, "../images/UCMerced_LandUse/Images/%s_features/%02d.txt", landUse[it],i);
      	  feature_file = fopen(feature_path, "w");

      	  
      	  fprintf(feature_file, "%d", it);
      	  for (fileIndex = 0; fileIndex < numMaps; fileIndex++) {
      	    //printf("numero %d: %lf\n", fileIndex, featureVector[fileIndex]);
      	  	fprintf(feature_file, ",%.15f", featureVector[fileIndex]);
      	  }

      	  fclose(feature_file);
      	}		
        
      }


    return 0;
}
