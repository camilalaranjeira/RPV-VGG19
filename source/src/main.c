#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include "../include/cnn_extraction.h"
#include "../include/image.h"
#include "../include/vgg19.h"
#include <time.h>

//defining green color for unix terminal
#define KGRN  "\x1B[32m"
#define K_FOLD 5
#define DATASET_DIR "../dataset"

//defining green color for unix terminal
#define KGRN  "\x1B[32m"


void run_coffee_database(Layer *layer) {

  // auxiliar variables
  int it,i,k,j,fileIndex,nImages;    
  int numMaps = 4096;
  char image_path[150];
  char dataset_dir[150];
  char picture_name[150];
  char feature_path[150];
  double featureVector[4096];
  FILE* feature_file;
  FILE* picture_file; 
  char picture_names[610][150];
  int picture_label[600];
  FeatureMap *featureMaps[18];

  for(it = 1; it <= K_FOLD; it++)
  {

    nImages=0;

    //abre o arquivo que contem o nome de todas as imagens do fold
  	sprintf(dataset_dir, "../images/brazilian_coffee_scenes/fold%d.txt", it);
  	picture_file = fopen(dataset_dir, "r");

    // le o nome das imagens e limpa a label
  	while( (fscanf(picture_file, "%s", picture_name)) != EOF ){
      	if(!strncmp(picture_name, "coffee", 6))
          {
              picture_label[nImages] = 1;
              memmove(picture_name, picture_name + 7, sizeof(picture_name)/sizeof(char));
          }
          else
          {
              picture_label[nImages] = 0;
              memmove(picture_name, picture_name + 10, sizeof(picture_name)/sizeof(char));
          }

         // salva 
      	 sprintf(image_path, "../images/brazilian_coffee_scenes/fold%d/%s.ppm", it, picture_name);
         strcpy(picture_names[nImages],image_path);
         nImages++;

    }
    fclose(picture_file);

    // Chama paralelamente a rede para cada uma das imagens lidas anteriormente
    //#pragma omp parallel for private(i,j,featureVector,featureMaps,feature_path,feature_file,fileIndex)
    for (i = 0; i < nImages; i++){

      printf("Executando a rede com: [%d]%s\n", i, picture_names[i]);

      // chama o forward da rede 
      forward(picture_names[i],featureMaps,layer);

      // Le as features geradas na ultima camada fully connected da rede, na VGG19, será 4096 featuremaps com 1 pixel cada em uma única camada.
      for (j = 0; j < numMaps; j++) {
      	featureVector[j] = featureMaps[17][j].data[0].channel1;

      }

      // libera memoria alocada pela forward
  	  for(j=1; j < 18; j++){
	      for(k=0; k < layer[j].output_size; k++){
              	free(featureMaps[j][k].data);
	      } 
        free(featureMaps[j]);
	    }
	    free(*featureMaps);

      // abre o arquivo para salvar as features de cada imagem
      sprintf(feature_path, "../images/brazilian_coffee_scenes/fold%d_features/features%d.txt", it,i);
      feature_file = fopen(feature_path, "w");

      // salva todas as features geradas juntamente com o label da classe na primeira posição
      printf("teste: %d\n", picture_label[i]);
      fprintf(feature_file, "%d", picture_label[i]);
      for (fileIndex = 0; fileIndex < numMaps; fileIndex++) {
      	fprintf(feature_file, ",%.15f", featureVector[fileIndex]);
      }

      fclose(feature_file);
      
    }//end for i  
  }//end for k_fold
}//end run_coffee_database 


void run_UCMerced_LandUser(Layer *layer) {

    // variaveis auxiliares
    char * landUse[] = {  "agricultural","airplane","baseballdiamond","beach","buildings","chaparral","denseresidential","forest","freeway","golfcourse","harbor","intersection","mediumresidential","mobilehomepark","overpass","parkinglot","river","runway","sparseresidential","storagetanks","tenniscourt"};
    int it,i,j,fileIndex;
    int numMaps = 4096;
    char image_path[150];
    char feature_path[150];
    FILE* feature_file;
    double featureVector[4096];
    FeatureMap *featureMaps[18];


    for(it = 0; it < 21; it++)
    {
	
      // itera sobre as 100 images de cada categoria
      #pragma omp parallel for private(i,j,featureVector,featureMaps,feature_path,feature_file,fileIndex,image_path)
      for (i = 0; i < 100; i++){

      	printf("Teste: %02d\n", i);
        sprintf(image_path, "../images/UCMerced_LandUse/Images/%s/%s%02d.ppm", landUse[it],landUse[it],i);
        //sprintf(image_path, "../images/UCMerced_LandUse/Images/%s/%s55.ppm", landUse[it],landUse[it],i);
      	printf("%d Testando: %s\n", i, image_path);

      	forward(image_path,featureMaps,layer);

      	for (j = 0; j < numMaps; j++) {
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
      }// for j		

        
   }// for it
}
  
int main() {

    /// variables
    Layer *layer;
  

    /// reads the parameters of VGG 19
    printf("Lendo arquivo de pesos\n");
    layer = readFile();
    printf("Terminada a leitura de pesos\n");


    //Roda a  VGG19 para o dataset de Coffe
    run_coffee_database(layer);

    	
    //Roda a  VGG19 para o dataset de  LandUse
    run_UCMerced_LandUser(layer);
    
    
    return 0;
}
