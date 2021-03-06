#include <stdio.h>
#include <stdlib.h>
#include "../../source/include/vgg19.h"
#include "../../source/include/image.h"

//defining green color for unix terminal
#define KGRN  "\x1B[32m"

//  'hello world' program, using ppm_tools
int main(){ 

    PPMImage *image;

    // read the ppm image into our C structure for ppm images
    image = readPPM("images/fig_example_square.ppm");

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
    FeatureMap *imgs[3];
    separateImageChannel(image,imgs);

	  //vetor de pesos de média considerando 3 dimensões de entrada
    //double weightVectorMedia[27] = {0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11};
    //double weightVectorMedia[54] = {0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.11,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    //double weightVectorMedia[147];
    //int i;
    //for (i  = 0 ; i < 147; i++){

    //  weightVectorMedia[i] = 1.0/49.0;
    //}  

    ////PPMImage *convolutedImage = convolutionLayer(imgs, weightVectorMedia, 7, 1, 0, 3, 1);
    //PPMImage *img = malloc(sizeof(PPMImage));
    //convoluteKernel(imgs, weightVectorMedia, 7, 1, 3, 3, img);
    ////PPMImage *convolutedImage = convolutionLayer(imgs, weightVectorMedia, 3, 1, 1, 3, 2);
    //writePPM("fig_example_convoluted2.ppm",img);
    ////writePPM("fig_example_convoluted1.ppm",&convolutedImage[1]);


   /*  TESTE CONVOLUTION LAYER */
   //double weightVectorMedia[18];
   // int i;
   // for (i  = 0 ; i < 9; i++){
   //   weightVectorMedia[i] = 1.0/9.0;
   // }
   // for (i  = 9 ; i < 18; i++){
   //   if(i != 13){ 
   //     weightVectorMedia[i] = -1.0;
   //   }else{
   //     weightVectorMedia[i] = 8;
   //   }  
   // }
   
   double weightVectorMedia[18];
   // int i;
   // for (i  = 0 ; i < 9; i++){
   //   weightVectorMedia[i] = 1.0/9.0;
   // }
   
    weightVectorMedia[0] = 1;
    weightVectorMedia[1] = 2;
    weightVectorMedia[2] = 1;
    weightVectorMedia[3] = 0;
    weightVectorMedia[4] = 0;
    weightVectorMedia[5] = 0;
    weightVectorMedia[6] = -1;
    weightVectorMedia[7] = -2;
    weightVectorMedia[8] = -1;

    // weightVectorMedia[9] = 1;
    // weightVectorMedia[10] = 0;
    // weightVectorMedia[11] = -1;
    // weightVectorMedia[12] = 2;
    // weightVectorMedia[13] = 0;
    // weightVectorMedia[14] = -2;
    // weightVectorMedia[15] = 1;
    // weightVectorMedia[16] = 0;
    // weightVectorMedia[17] = -1;

    weightVectorMedia[9] = -1;
    weightVectorMedia[10] = -1;
    weightVectorMedia[11] = -1;
    weightVectorMedia[12] = -1;
    weightVectorMedia[13] = 8;
    weightVectorMedia[14] = -1;
    weightVectorMedia[15] = -1;
    weightVectorMedia[16] = -1;
    weightVectorMedia[17] = -1;

    double bias[2] = {0,255};

    FeatureMap *featuremaps;  
    featuremaps = convolutionLayer(imgs, weightVectorMedia , 3, 1, 1, 1, 2, bias);
    writePPMFromFeatureMap("fig_example_changed1.ppm",&featuremaps[0]);
    writePPMFromFeatureMap("fig_example_changed2.ppm",&featuremaps[1]);


    /* Teste fully connected  */
    //double weightVectorMedia[301056];
    //int i;
    //for (i  = 0 ; i < 301056 ; i++){
    //  weightVectorMedia[i] = 1.0/50176.0;
    //}
  
    //FeatureMap *featuremaps;  
    //featuremaps = fullyConnectedLayer(imgs, weightVectorMedia,3,2);
    //writePPM("fig_example_changed1.ppm",&featuremaps[0]);
    //writePPM("fig_example_changed2.ppm",&featuremaps[1]);

  
    // just a pause step, telling the the job is done 
    printf("%sJob done, press any key to continue ...", KGRN);
    getchar();

    return 0;
}

