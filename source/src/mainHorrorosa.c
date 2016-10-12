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

    PPMImage *image;

    // read the ppm image into our C structure for ppm images
   image = readPPM("/home/igor_pig/workspace/RPV-VGGLocal/source/src/arceburgo.T64.B128.L4672.R4736.ppm");
   readFile();


    FeatureMap *imgs[3];
    PPMImage *scale = Scale(image, (float)224/image->x, (float)224/image->y);
    //writePPM("fig_example_changed_scale.ppm",scale);
    PPMImage *zero = ZeroPad(scale, 1);
    separateImageChannel(zero,imgs);

    FeatureMap *featureMaps;

    /////////////////////////////////////
    //1st convLayer - primeira camada////////////////////
    featureMaps = convolutionLayer(imgs,layer[0].weight,layer[0].kH,1,1,layer[0].input_size, layer[0].output_size, layer[0].bias);

    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////
    //2nd convLayer - segunda camada//////////////////////////////
    int k=0;
    for (k=0;k<layer[1].input_size;k++) {
    	featureMapZeroPad(&featureMaps[k], 1);
    }

    //convertendo pra um ponteiro de ponteiro
    FeatureMap *f2Maps[layer[1].input_size];
    for (k=0;k<layer[1].input_size;k++) {
    	f2Maps[k] = &featureMaps[k];
    }
    featureMaps = convolutionLayer(f2Maps,layer[1].weight,layer[1].kH,1,1,layer[1].input_size, layer[1].output_size, layer[1].bias);

    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////
    //1st poolingLayer - terceira camada//////////////////////
    for (k=0;k<layer[1].input_size;k++) {
    	maxPool(&featureMaps[k], 2, 2);
    }


    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////
    //3rd convLayer - quarta camada//////////////////////////////
    k=0;
    for (k=0;k<layer[2].input_size;k++) {
      	featureMapZeroPad(&featureMaps[k], 1);
    }

    //convertendo pra um ponteiro de ponteiro
    FeatureMap *f3Maps[layer[2].input_size];
    for (k=0;k<layer[2].input_size;k++) {
    	f3Maps[k] = &featureMaps[k];
    }
    featureMaps = convolutionLayer(f3Maps,layer[2].weight,layer[2].kH,1,1,layer[2].input_size, layer[2].output_size, layer[2].bias);

    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////
    //4th convLayer - quinta camada//////////////////////////////
    k=0;
    for (k=0;k<layer[3].input_size;k++) {
       	featureMapZeroPad(&featureMaps[k], 1);
    }

    //convertendo pra um ponteiro de ponteiro
    FeatureMap *f4Maps[layer[3].input_size];
    for (k=0;k<layer[3].input_size;k++) {
      	f4Maps[k] = &featureMaps[k];
    }
    featureMaps = convolutionLayer(f4Maps,layer[3].weight,layer[3].kH,1,1,layer[3].input_size, layer[3].output_size, layer[3].bias);


    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////
    //2nd poolingLayer - sexta camada//////////////////////////////
    for (k=0;k<layer[3].input_size;k++) {
       	maxPool(&featureMaps[k], 2, 2);
    }

    //se quiser printar tudo, faz um for e mostra os featureMaps todos
    writePPMFromFeatureMap("deep0.ppm",&featureMaps[0]);


    getchar();
	return 0;


}
