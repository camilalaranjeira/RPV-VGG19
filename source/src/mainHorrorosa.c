#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/image.h"
#include "../include/vgg19.h"
#include "../include/cnn_extraction.h"

//defining green color for unix terminal
#define KGRN  "\x1B[32m"

#define images_dir "../images/"
#define images_dir_fold1 "../images/brazilian_coffee_scenes/fold1/"

int main(){

    PPMImage *image;

    // read the ppm image into our C structure for ppm images
   image = readPPM("/home/igor_pig/workspace/RPV-VGGLocal/source/src/arceburgo.T64.B128.L4672.R4736.ppm");
   //leitura do arquivo de pesos
   printf("Lendo o arquivo de pesos\n");
   readFile();
   printf("Leitura de pesos concluída\n");

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


    //printing featureMaps
    /*int i;
    char index[50];

    for (i=0;i<layer[17].output_size;i++) {
    	sprintf(index, "%d", i);
    	strcat(index,".ppm");
    	writePPMFromFeatureMap(index,&featureMaps[i]);
	}
	*/
    getchar();
	return 0;


}
