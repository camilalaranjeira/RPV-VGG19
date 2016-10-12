#include "../include/image.h"


// Function that create a C structure representation of a ppm image file given as parameter @filename
PPMImage *readPPM(const char *filename)
{
  //variables
  char buff[16];
  PPMImage *img;
  FILE *fp;
  int c, rgb_comp_color;

  //open PPM file for reading
  fp = fopen(filename, "rb");
  if (!fp) {
       fprintf(stderr, "%sUnable to open file '%s'\n", KRED,filename);
       exit(1);
  }

  //read image format, the first 2 bytes of the file.
  if (!fgets(buff, sizeof(buff), fp)) {
       perror(filename);
       exit(1);
  }

  // check if the format is valid, it must be "P6", which is the ppm image's  magic number =P
  if (buff[0] != 'P' || buff[1] != '6') {
       fprintf(stderr, "%sInvalid image format (must be 'P6')\n", KRED);
       exit(1);
  }

  // The PPM format allows for comments, so we need to check if any was given
  c = getc(fp);
  while (c == '#') {
  while (getc(fp) != '\n') ;
       c = getc(fp);
  }
  ungetc(c, fp);

  //read image size information
  int rows, cols;
  if (fscanf(fp, "%d %d", &rows, &cols) != 2) {
       fprintf(stderr, "%sInvalid image size (error loading '%s')\n", KRED, filename);
       exit(1);
  }

  //read rgb component
  if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
       fprintf(stderr, "%sInvalid rgb component (error loading '%s')\n", KRED, filename);
       exit(1);
  }

  //check rgb component depth
  if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
       fprintf(stderr, "%s'%s' does not have 8-bits components\n", KRED, filename);
       exit(1);
  }
  while (fgetc(fp) != '\n') ;

  //memory allocation for pixel data,  the structured C ppm image
//  img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

  img = CreateImage(cols, rows);
  // memory failure check
  if (!img) {
       fprintf(stderr, "%sUnable to allocate memory\n", KRED);
       exit(1);
  }

  //read pixel data from file, the RGB component
  if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
       fprintf(stderr, "%sError loading image '%s'\n", KRED, filename);
       exit(1);
  }

  // Close the file and return the C structured ppm image
  fclose(fp);
  return img;
}



// Function that receives a C structured  PPMImage and write it as a true ppm image in the path "filename" given
void writePPM(const char *filename, PPMImage *img)
{
    // Open the file that will hold the ppm image
    FILE *fp;
    fp = fopen(filename, "wb");

    // check for opening failures
    if (!fp) {
         fprintf(stderr, "%sUnable to open file '%s'\n", KRED, filename);
         exit(1);
    }

    //write the header file
    //
    //image format, must be the ppm image's magic number
    fprintf(fp, "P6\n");


    //write image size
    fprintf(fp, "%d %d\n",img->x,img->y);

    // rgb component depth, which was defined as constant in ppm_tools.h
    fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

    // write the pixel data
    fwrite(img->data, 3 * img->x, img->y, fp);

    //close file
    fclose(fp);
}

// Function that receives a C structured  FeatureMap and write it as a true ppm image in the path "filename" given
void writePPMFromFeatureMap(const char *filename, FeatureMap *img)
{

    // Open the file that will hold the ppm image
    FILE *fp;
    fp = fopen(filename, "wb");

    //instance a PPMPixel
    PPMPixel *data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

    // check for opening failures
    if (!fp) {
         fprintf(stderr, "%sUnable to open file '%s'\n", KRED, filename);
         exit(1);
    }

    //write the header file
    //
    //image format, must be the ppm image's magic number
    fprintf(fp, "P6\n");


    //write image size
    fprintf(fp, "%d %d\n",img->x,img->y);

    // rgb component depth, which was defined as constant in ppm_tools.h
    fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

    //convert FeatureMapPixel to PPMPixel
    int i;
    for ( i=0 ; i < img->x * img->y; i++){
      data[i].red = img->data[i].channel1;
      data[i].green = 0;
      data[i].blue = 0;
    }

    // write the pixel data
    fwrite(data, 3 * img->x, img->y, fp);

    free(data);
    //close file
    fclose(fp);
}

//function to split input image into 3 separate channels (feature maps)
//each resultant feature map store values into the red channel (0s into other channels)
void separateImageChannel(PPMImage *img, FeatureMap *imgs[]) {

	int i;

	///check if image is null
	  if(img){

		  //generate one feature map for each image channel
	      FeatureMap *feature_map_img_red = (FeatureMap *)malloc(sizeof(FeatureMap));
	      FeatureMap *feature_map_img_green = (FeatureMap *)malloc(sizeof(FeatureMap));
	      FeatureMap *feature_map_img_blue = (FeatureMap *)malloc(sizeof(FeatureMap));

	      //allocate memory to store the image data
	      feature_map_img_red->data = (FeatureMapPixel*)malloc(img->x * img->y * sizeof(FeatureMapPixel));
	      feature_map_img_green->data = (FeatureMapPixel*)malloc(img->x * img->y * sizeof(FeatureMapPixel));
	      feature_map_img_blue->data = (FeatureMapPixel*)malloc(img->x * img->y * sizeof(FeatureMapPixel));

	      //set image size
	      feature_map_img_red->x = img->x;feature_map_img_red->y = img->y;
	      feature_map_img_green->x = img->x;feature_map_img_green->y = img->y;
	      feature_map_img_blue->x = img->x;feature_map_img_blue->y = img->y;


	      //iterates over each image pixel
	      for(i=0;i<img->x*img->y;i++){
	    	  feature_map_img_red->data[i].channel1 = img->data[i].red;
	    	  feature_map_img_green->data[i].channel1 = img->data[i].green;
	    	  feature_map_img_blue->data[i].channel1 = img->data[i].blue;

	      }

	      //insert into an array
	      imgs[0] = feature_map_img_red;
	      imgs[1] = feature_map_img_green;
	      imgs[2] = feature_map_img_blue;
	  }
}

void Error(char *msg,char *func)
{ /* It prints error message and exits
                                    the program. */
  fprintf(stderr,"Error:%s in %s\n",msg,func);
  exit(-1);
}

PPMImage *CreateImage(int ncols, int nrows)
{
  PPMImage *img=NULL;
  int i;

  img = (PPMImage *) calloc(1,sizeof(PPMImage));
  if (img == NULL) Error(MSG1,"CreateImage");

  img->x = nrows;
  img->y = ncols;
  img->data = (PPMPixel *) calloc(nrows*ncols, sizeof(PPMPixel));
  if(img->data == NULL) Error(MSG1,"CreateImage");

  img->tbrow = calloc(nrows, sizeof(int));
  img->tbrow[0]=0;
  for (i=1; i < nrows; i++)
	  img->tbrow[i]=img->tbrow[i-1]+ncols;
  return(img);
}

bool ValidPixel(PPMImage *img, int x, int y)
{
  if ((x >= 0)&&(x < img->y)&&
      (y >= 0)&&(y < img->x))
    return(true);
  else
    return(false);
}

PPMImage *Scale(PPMImage *img, float Sx, float Sy)
{
  float S[2][2],x,y,d1,d2,d3,d4,Ix1[3],Ix2[3],If[3];
  PPMImage *scl;
  Pixel u,v,prev,next;

  if (Sx == 0.0) Sx = 1.0;
  if (Sy == 0.0) Sy = 1.0;

  S[0][0] = 1.0/Sx;
  S[0][1] = 0;
  S[1][0] = 0;
  S[1][1] = 1.0/Sy;

  scl = CreateImage((int)(img->y*Sx + 0.5),(int)(img->x*Sy + 0.5));

  for (v.y=0; v.y < scl->x; v.y++)
  {
    for (v.x=0; v.x < scl->y; v.x++)
    {
    	x = ((v.x-scl->y/2.)*S[0][0] + (v.y-scl->x/2.)*S[0][1])
		+ img->y/2.;
		y = ((v.x-scl->y/2.)*S[1][0] + (v.y-scl->x/2.)*S[1][1])
		+ img->x/2.;
		u.x = (int)(x+0.5);
		u.y = (int)(y+0.5);
		if (ValidPixel(img,u.x,u.y))
		{
			if (x < u.x) {
			  next.x = u.x;
			  prev.x = u.x - 1;
			} else {
			  next.x = u.x + 1;
			  prev.x = u.x;
			}
			d1 = next.x - x;
			d2 = x - prev.x;
			if (y < u.y) {
				next.y = u.y;
				prev.y = u.y - 1;
			} else {
				next.y = u.y + 1;
				prev.y = u.y;
			}
			d3 = next.y - y;
			d4 = y - prev.y;

			if (ValidPixel(img,prev.x,prev.y)&&ValidPixel(img,next.x,prev.y))
			{
				// RED
				Ix1[0] = d1*img->data[prev.x+img->tbrow[prev.y]].red +
				d2*img->data[next.x+img->tbrow[prev.y]].red;

				//GREEN
				Ix1[1] = d1*img->data[prev.x+img->tbrow[prev.y]].green +
				d2*img->data[next.x+img->tbrow[prev.y]].green;

				//BLUE
				Ix1[2] = d1*img->data[prev.x+img->tbrow[prev.y]].blue +
				d2*img->data[next.x+img->tbrow[prev.y]].blue;
			}
			else
			{
				Ix1[0] = img->data[u.x+img->tbrow[u.y]].red;
				Ix1[1] = img->data[u.x+img->tbrow[u.y]].green;
				Ix1[2] = img->data[u.x+img->tbrow[u.y]].blue;
			}

			if (ValidPixel(img,prev.x,next.y)&&ValidPixel(img,next.x,next.y))
			{
				//RED
				Ix2[0] = d1*img->data[prev.x+img->tbrow[next.y]].red +
				d2*img->data[next.x+img->tbrow[next.y]].red;

				//GREEN
				Ix2[1] = d1*img->data[prev.x+img->tbrow[next.y]].green +
				d2*img->data[next.x+img->tbrow[next.y]].green;

				//BLUE
				Ix2[2] = d1*img->data[prev.x+img->tbrow[next.y]].blue +
				d2*img->data[next.x+img->tbrow[next.y]].blue;

			}
			else
			{
				Ix2[0] = img->data[u.x+img->tbrow[u.y]].red;
				Ix2[1] = img->data[u.x+img->tbrow[u.y]].green;
				Ix2[2] = img->data[u.x+img->tbrow[u.y]].blue;
			}

			If[0] = d3*Ix1[0] + d4*Ix2[0];
			If[1] = d3*Ix1[1] + d4*Ix2[1];
			If[2] = d3*Ix1[2] + d4*Ix2[2];

			scl->data[v.x+scl->tbrow[v.y]].red = (int)If[0];
			scl->data[v.x+scl->tbrow[v.y]].green = (int)If[1];
			scl->data[v.x+scl->tbrow[v.y]].blue = (int)If[2];
		}
    }
  }
  return(scl);
}


void *featureMapZeroPad(FeatureMap *fmap, int n) {

		FeatureMap *zeroMap = (FeatureMap *)malloc(sizeof(FeatureMap));;

		zeroMap->data = (FeatureMapPixel*)malloc((fmap->x+2*n) * (fmap->y+2*n) * sizeof(FeatureMapPixel));
		zeroMap->x = fmap->x+2*n;
		zeroMap->y = fmap->y+2*n;

		int i,j,x,y;
		j = 0;
		for (y=0; y < zeroMap->y; y++) {
		  for (x=0; x < zeroMap->x; x++) {
			if(!(x < n || y < n || x > (fmap->x -1 + n) || y > (fmap->y - 1 + n))) {
			  i = y * zeroMap->y + x;
			  zeroMap->data[i] = fmap->data[j];
			  j++;
			}
	       }
		}

		fmap->data = zeroMap->data;
		fmap->x = zeroMap->x;
		fmap->y = zeroMap->y;
		free(zeroMap);

}



PPMImage *ZeroPad(PPMImage *img, int n)
{
	PPMImage *eimg; /* expanded image with zeros to the left and to the
				  bottom of the input image */
	int i,j,x,y;
	eimg = CreateImage(img->y+(2*n),img->x+(2*n));

	j = 0;
	for (y=0; y < eimg->x; y++)
	{
		for (x=0; x < eimg->y; x++)
		{
			if(!(x < n || y < n || x > (img->x -1 + n) || y > (img->y - 1 + n)))
			{
				i = y * eimg->y + x;
				eimg->data[i] = img->data[j];
				j++;
			}
		}
	}
	return(eimg);
}
