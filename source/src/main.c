#include <stdio.h>
#include <stdlib.h>
#include "../include/image.h"
#include "../include/vgg19.h"
#include "../include/cnn_extraction.h"

//defining green color for unix terminal
#define KGRN  "\x1B[32m"

#define images_dir "../images/"

void main(){

	readFile();

    PPMImage *image;

    // read the ppm image into our C structure for ppm images
    image = readPPM("/home/laranjeira/workspace/RPV-VGG19/images/cafe.ppm");

    PPMImage *scale = Scale(image, (float)224/image->x, (float)224/image->y);
    writePPM("fig_example_changed_scale.ppm",scale);

    PPMImage *zero = ZeroPad(image, 30);
    writePPM("fig_example_changed_zero.ppm",zero);

    // just a pause step, telling the the job is done
    printf("%sJob done, press any key to continue ...", KGRN);
    getchar();

}
