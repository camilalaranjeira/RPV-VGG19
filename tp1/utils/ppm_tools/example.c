#include <stdio.h>
#include <stdlib.h>
#include "ppm_tools.h"

//defining green color for unix terminal
#define KGRN  "\x1B[32m"

//  'hello world' program, using ppm_tools
void main(){ 

    PPMImage *image;

    // read the ppm image into our C structure for ppm images
    image = readPPM("fig_example_square.ppm");

    // Apply an example that changes its colors
    //changeColorPPM(image);

    // write the image back to ppm format
    //writePPM("fig_example_changed.ppm",image);


    //teste da função que separa os canais de uma imagem de entrada
    //PPMImage *imgs[3];
    //separateImageChannel(image,imgs);

    //writePPM("fig_example_changed1.ppm",imgs[0]);
    //writePPM("fig_example_changed2.ppm",imgs[1]);
    //writePPM("fig_example_changed3.ppm",imgs[2]);

    //teste da função de max pooling
    //PPMImage *pooledImage = maxPool(image,2,2);
    //writePPM("fig_example_maxpooled.ppm",pooledImage);


    //teste da convolução com kernel de média (blur)
    //
    PPMImage *imgs[3];
    separateImageChannel(image,imgs);

	  //vetor de pesos de média considerando 3 dimensões de entrada
    //double weightVectorMedia[27] = {0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11};
    double weightVectorMedia[54] = {0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

    //PPMImage *convolutedImage = convoluteKernel(imgs, weightVectorMedia, 3, 1, 1, 3, 64);
    PPMImage *convolutedImage = convolutionLayer(imgs, weightVectorMedia, 3, 1, 1, 3, 2);
    writePPM("fig_example_convoluted0.ppm",&convolutedImage[0]);
    writePPM("fig_example_convoluted1.ppm",&convolutedImage[1]);


    // just a pause step, telling the the job is done 
    printf("%sJob done, press any key to continue ...", KGRN);
    getchar();

}

