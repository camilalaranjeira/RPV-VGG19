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


FeatureMap *forward(char *filePath){

    PPMImage *image;

    // read the ppm image into our C structure for ppm images
    image = readPPM(filePath);

    FeatureMap *imgs[3];
    PPMImage *scale = Scale(image, (float)224/image->x, (float)224/image->y);
    //writePPM("fig_example_changed_scale.ppm",scale);
    PPMImage *zero = ZeroPad(scale, 1);
    separateImageChannel(zero,imgs);

    FeatureMap *featureMaps;

    clock_t tempoInicial = clock();
    //#############Primeiro Bloco##########///////////////////
    printf("Primeiro bloco de camadas convolucionais\n");
    featureMaps = convolutionLayer(imgs,layer[0].weight,layer[0].kH,1,1,layer[0].input_size, layer[0].output_size, layer[0].bias);
    featureMaps = convolutionalLayer(featureMaps, layer[1].weight,layer[1].kH,1,1,layer[1].input_size, layer[1].output_size, layer[1].bias);
    poolingLayer(featureMaps, 2,2, layer[1].input_size);

    //#############Segundo Bloco##########///////////////////
    printf("Segundo bloco de camadas convolucionais\n");
    featureMaps = convolutionalLayer(featureMaps, layer[2].weight,layer[2].kH,1,1,layer[2].input_size, layer[2].output_size, layer[2].bias);
    featureMaps = convolutionalLayer(featureMaps, layer[3].weight,layer[3].kH,1,1,layer[3].input_size, layer[3].output_size, layer[3].bias);
    poolingLayer(featureMaps, 2,2, layer[3].input_size);

    //#############Terceiro Bloco##########///////////////////
    printf("Terceiro bloco de camadas convolucionais\n");
    featureMaps = convolutionalLayer(featureMaps, layer[4].weight,layer[4].kH,1,1,layer[4].input_size, layer[4].output_size, layer[4].bias);
    featureMaps = convolutionalLayer(featureMaps, layer[5].weight,layer[5].kH,1,1,layer[5].input_size, layer[5].output_size, layer[5].bias);
    featureMaps = convolutionalLayer(featureMaps, layer[6].weight,layer[6].kH,1,1,layer[6].input_size, layer[6].output_size, layer[6].bias);
    featureMaps = convolutionalLayer(featureMaps, layer[7].weight,layer[7].kH,1,1,layer[7].input_size, layer[7].output_size, layer[7].bias);
    poolingLayer(featureMaps, 2,2, layer[7].input_size);

    //#############Quarto Bloco##########///////////////////
    printf("Quarto bloco de camadas convolucionais\n");
    featureMaps = convolutionalLayer(featureMaps, layer[8].weight,layer[8].kH,1,1,layer[8].input_size, layer[8].output_size, layer[8].bias);
    featureMaps = convolutionalLayer(featureMaps, layer[9].weight,layer[9].kH,1,1,layer[9].input_size, layer[9].output_size, layer[9].bias);
    featureMaps = convolutionalLayer(featureMaps, layer[10].weight,layer[10].kH,1,1,layer[10].input_size, layer[10].output_size, layer[10].bias);
    featureMaps = convolutionalLayer(featureMaps, layer[11].weight,layer[11].kH,1,1,layer[11].input_size, layer[11].output_size, layer[11].bias);
    poolingLayer(featureMaps, 2,2, layer[11].input_size);

    //#############Quinto Bloco##########///////////////////
    printf("Quinto bloco de camadas convolucionais\n");
    featureMaps = convolutionalLayer(featureMaps, layer[12].weight,layer[12].kH,1,1,layer[12].input_size, layer[12].output_size, layer[12].bias);
    featureMaps = convolutionalLayer(featureMaps, layer[13].weight,layer[13].kH,1,1,layer[13].input_size, layer[13].output_size, layer[13].bias);
    featureMaps = convolutionalLayer(featureMaps, layer[14].weight,layer[14].kH,1,1,layer[14].input_size, layer[14].output_size, layer[14].bias);
    featureMaps = convolutionalLayer(featureMaps, layer[15].weight,layer[15].kH,1,1,layer[15].input_size, layer[15].output_size, layer[15].bias);
    poolingLayer(featureMaps, 2,2, layer[15].input_size);

    //#############Camadas FC##########///////////////////
    //printf("Camadas Fully-Connected\n");
    featureMaps = fcLayer(featureMaps, layer[16].weight, 512, layer[16].output_size, layer[16].bias);
    featureMaps = fcLayer(featureMaps, layer[17].weight, 512, layer[17].output_size, layer[17].bias);

    //##########Computando tempo de execução########////////////////
    clock_t tempoFinal = clock();
    double tempoTotal = (double)(tempoFinal - tempoInicial) / CLOCKS_PER_SEC;
    printf("Tempo total de execução do sistema por imagem: %f\n", tempoTotal);


    return featureMaps;
    //printing featureMaps
    /*int i;
    char index[50];

    for (i=0;i<layer[1].output_size;i++) {
    	sprintf(index, "deep%d", i);
    	strcat(index,".ppm");
    	writePPMFromFeatureMap(index,&featureMaps[i]);
	}

    getchar();*/
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
    FeatureMap *fmaps;
    double featureVector[4096];
    FILE* feature_file;
    int fileIndex;
    FILE* picture_file; 
    char picture_names[610][150];

    /// reads the parameters of VGG 19


    printf("Lendo arquivo de pesos\n");
    readFile();
    printf("Terminada a leitura de pesos\n");
    /// reads iteratively all the images in the coffee dataset


    //for(it = 1; it <= K_FOLD; it++)
    for(it = 1; it <= 1; it++)
    {

      i=0;
    	sprintf(dataset_dir, "../images/brazilian_coffee_scenes/fold%d.txt", it);
      sprintf(feature_path, "../images/brazilian_coffee_scenes/fold%d/features.txt", it);
      feature_file = fopen(feature_path, "a");
    	picture_file = fopen(dataset_dir, "r");
    	while( (fscanf(picture_file, "%s", picture_name)) != EOF ){
        	//printf("\nfileString: %s\n", picture_name);
        	if(!strncmp(picture_name, "coffee", 6))
            {
                //picture_name = picture_name + 7;
                memmove(picture_name, picture_name + 7, sizeof(picture_name)/sizeof(char));
            }
            else
            {
                memmove(picture_name, picture_name + 10, sizeof(picture_name)/sizeof(char));
            }
        	 sprintf(image_path, "../images/brazilian_coffee_scenes/fold%d/%s.ppm", it, picture_name);
           strcpy(picture_names[i],image_path);
           i++;

      }
      fclose(picture_file);
      //fprintf(feature_file, "\n");
      fclose(feature_file);

      #pragma omp parallel for private(i,j,featureVector,fmaps)
      for (i = 0; i < 601; i++){
        printf("%d Testando: %s\n", i, picture_names[i]);
        fmaps = forward(picture_names[i]);
        for (j = 0; j < numMaps; j++) {
        	FeatureMap *map = &fmaps[j];
        	//printf("pixValue %g\n", map->data[0].channel1);
        	featureVector[j] = map->data[0].channel1;
        }

        //for (fileIndex = 0; fileIndex< numMaps; fileIndex++) {
        //	fprintf(feature_file, "%f ", featureVector[fileIndex]);
        //}

      }  
    }


    /// reads iteratively all the images in the UCMerced dataset
    /*struct dirent *file;
    struct dirent *folder;
    DIR* dir_path = opendir("../dataset/UCMerced/images");
    if (dir_path != NULL)
    {
        while( (folder = readdir (dir_path)) )
        {
            if(!strncmp(folder->d_name, ".", 1))
                continue;
            sprintf(dataset_dir, "%s/UCMerced/images/%s", DATASET_DIR, folder->d_name);
            DIR* img_path = opendir(dataset_dir);
            while( (file = readdir (img_path)) )
            {
                if(!strncmp(file->d_name, ".", 1))
                    continue;
                sprintf(image_path, "%s/%s", dataset_dir, file->d_name);
                /// TODO create the forward method that will receive the path to each image in dataset and return a corresponding feature vector

                fmaps = forward(image_path);

                int numMaps;
                for (numMaps=0;numMaps<4096;numMaps++) {
                	FeatureMap *map = &fmaps[numMaps];
                   	featureVector[numMaps] = map->data[0].channel1;
                }

                sprintf(feature_path, "%s/UCMerced/features/%s/features.txt", DATASET_DIR, folder->d_name);
                FILE* feature_file = fopen(feature_path, "w");
                fwrite(featureVector, sizeof(double), 4096, feature_file);
                fclose(feature_file);
            }
            (void) closedir(img_path);
        }
        (void) closedir(dir_path);
    }
    else
        perror("Couldn't open the directory");*/
    return 0;
}
