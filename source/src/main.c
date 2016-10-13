#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include "../include/image.h"
#include "../include/vgg19.h"
#include "../include/cnn_extraction.h"

//defining green color for unix terminal
#define KGRN  "\x1B[32m"
#define K_FOLD 5
#define DATASET_DIR "../dataset"

int main()
{
    /// variables
    int it;
    char image_path[150];
    char dataset_dir[150];
    char picture_name[150];
    char feature_path[150];
    double featureVector[4096];
    /// reads the parameters of VGG 19
    readFile();
    /// reads iteratively all the images in the coffee dataset
    for(it = 1; it <= K_FOLD; it++)
    {
        sprintf(dataset_dir, "%s/coffee/images/fold%d.txt", DATASET_DIR, it);
        FILE* picture_file = fopen(dataset_dir, "r");
        while( (fscanf(picture_file, "%s", picture_name)) != EOF )
        {
            if(!strncmp(picture_name, "coffee", 6))
            {
                memmove(picture_name, picture_name + 7, sizeof(picture_name)/sizeof(char));
            }
            else
            {
                memmove(picture_name, picture_name + 10, sizeof(picture_name)/sizeof(char));
            }
            sprintf(image_path, "%s/coffee/images/fold%d/%s.ppm",DATASET_DIR, it, picture_name);
            /// TODO create the forward method that will receive the path to each image in dataset and return a corresponding feature vector
            //featureVector = forward(image_path);
            /// saves the features vector in each specific folder
            sprintf(feature_path, "%s/coffee/features/fold%d/features.txt", DATASET_DIR, it);
            FILE* feature_file = fopen(feature_path, "w");
            //fwrite(featureVector, sizeof(double), 4096, feature_file);
            fclose(feature_file);
        }
        fclose(picture_file);
    }
    /// reads iteratively all the images in the UCMerced dataset
    struct dirent *file;
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
                //featureVector = forward(image_path);
                /// saves the features vector in each specific folder
                sprintf(feature_path, "%s/UCMerced/features/%s/features.txt", DATASET_DIR, folder->d_name);
                FILE* feature_file = fopen(feature_path, "w");
                //fwrite(featureVector, sizeof(double), 4096, feature_file);
                fclose(feature_file);
            }
            (void) closedir(img_path);
        }
        (void) closedir(dir_path);
    }
    else
        perror("Couldn't open the directory");
    return 0;
}
