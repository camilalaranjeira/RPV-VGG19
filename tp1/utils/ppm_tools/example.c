#include <stdio.h>
#include <stdlib.h>
#include "ppm_tools.h"

//defining green color for unix terminal
#define KGRN  "\x1B[32m"

//  'hello world' program, using ppm_tools
void main(){ 

    PPMImage *image;

    // read the ppm image into our C structure for ppm images
    image = readPPM("fig_example.ppm");

    // Apply an example that changes its colors
    changeColorPPM(image);

    // write the image back to ppm format
    writePPM("fig_example_changed.ppm",image);

    // just a pause step, telling the the job is done 
    printf("%sJob done, press any key to continue ...", KGRN);
    getchar();

}

