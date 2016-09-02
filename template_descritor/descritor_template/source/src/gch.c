#include "gch.h"

#include "common.h"
#include "image.h"

#define SIZE 64

typedef struct _Property {
  int color;
} Property;

typedef struct _VisualFeature {
  ulong *colorH;
  int n;
} VisualFeature;

typedef struct _CompressedVisualFeature {
  uchar *colorH;
  int n;
} CompressedVisualFeature;

Property *AllocPropertyArray(int n)
{
  Property *v=NULL;
  v = (Property *) calloc(n,sizeof(Property));
  if (v==NULL)
    Error(MSG1,"AllocPropertyArray");
  return(v);
}

VisualFeature *CreateVisualFeature(int n)
{
  VisualFeature *vf=NULL;

  vf = (VisualFeature *) calloc(1,sizeof(VisualFeature));
  if (vf != NULL) {
    vf->colorH = AllocULongArray(n);
    vf->n = n;
  } else {
    Error(MSG1,"CreateVisualFeature");
  }
  return(vf);
}

void DestroyVisualFeature(VisualFeature **vf)
{
  VisualFeature *aux;

  aux = *vf;
  if (aux != NULL) {
    if (aux->colorH != NULL) free(aux->colorH);
    free(aux);
    *vf = NULL;
  }
}

CompressedVisualFeature *CreateCompressedVisualFeature(int n)
{
  CompressedVisualFeature *cvf=NULL;

  cvf = (CompressedVisualFeature *) calloc(1,sizeof(CompressedVisualFeature));
  if (cvf != NULL) {
    cvf->colorH = AllocUCharArray(n);
    cvf->n = n;
  } else {
    Error(MSG1,"CreateCompressedVisualFeature");
  }
  return(cvf);
}

void DestroyCompressedVisualFeature(CompressedVisualFeature **cvf)
{
  CompressedVisualFeature *aux;

  aux = *cvf;
  if (aux != NULL) {
    if (aux->colorH != NULL) free(aux->colorH);
    free(aux);
    *cvf = NULL;
  }
}

int *QuantizeColors(CImage *cimg, int color_dim)
{
  ulong i;
  ulong r, g, b;
  ulong fator_g, fator_b;
  int *color, n;
  
  n = cimg->C[0]->nrows * cimg->C[0]->ncols;  

  color = AllocIntArray(n);
  
  fator_g = color_dim;
  fator_b = fator_g*color_dim;
  
  for(i=0; i<n; i++){
    r = color_dim*cimg->C[0]->val[i]/256;
    g = color_dim*cimg->C[1]->val[i]/256;
    b = color_dim*cimg->C[2]->val[i]/256;
    
    color[i] = (r + fator_g*g + fator_b*b);
  }
  return(color);
}

inline uchar ComputeNorm(float value)
{
  return((uchar)(255. * value));
}

void CompressHistogram(uchar *ch, ulong *h, ulong max, int size)
{
  int i;
  uchar v;
  
  for(i=0; i<size; i++){
    v = ComputeNorm((float) h[i] / (float) max);
    ch[i] = (uchar)(v);
  }
}

Property *ComputePixelsProperties(CImage *cimg)
{
  Property *p=NULL;
  int *color, i, n;
  
  n = cimg->C[0]->nrows * cimg->C[0]->ncols;  
  
  p = AllocPropertyArray(n);
  
  color = QuantizeColors(cimg, 4);
  for(i=0; i<n; i++) 
    p[i].color=color[i];
  
  free(color);
  return(p);
}

VisualFeature *ComputeHistograms(Property *p, Image *mask, 
                                 int npixels, int *npoints)
{
  VisualFeature *vf=NULL;
  ulong i;
  
  vf = CreateVisualFeature(SIZE);
  for(i=0; i<SIZE; i++){
    vf->colorH[i] = 0;
  }
  
  *npoints = 0;
  for(i=0; i<npixels; i++)
    if (mask->val[i]) {
      vf->colorH[p[i].color]++;
      (*npoints)++;
    }
  return(vf);
}

CompressedVisualFeature *CompressHistograms(VisualFeature *vf, int npixels)
{
  CompressedVisualFeature *cvf=NULL;
  
  cvf = CreateCompressedVisualFeature(SIZE);
  CompressHistogram(cvf->colorH, vf->colorH, npixels, SIZE);
  
  return(cvf);
}

CompressedVisualFeature *ReadCompressedVisualFeatures(char *filename)
{
  CompressedVisualFeature *cvf=NULL;
  FILE *fp;
  int i, n;
  uchar c;

  fp = fopen(filename,"r");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  fscanf(fp,"%d\n",&n);
  cvf = CreateCompressedVisualFeature(n);
  for (i=0; i<n; i++) {
    fscanf(fp,"%c\n",&c);
    cvf->colorH[i] = c;
  }
  fclose(fp);
  return(cvf);
}

void WriteCompressedVisualFeatures(CompressedVisualFeature *cvf,char *filename)
{
  FILE *fp;
  int i;
  
  fp = fopen(filename,"w");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  
  fprintf(fp,"%d\n",SIZE);
  for(i=0;i<SIZE;i++)
    fprintf(fp, "%c", cvf->colorH[i]);
  
  fclose(fp);
}

CompressedVisualFeature *ExtractCompressedVisualFeatures(CImage *cimg, 
                                                          Image *mask)
{
  CompressedVisualFeature *cvf=NULL;
  VisualFeature *vf;
  Property *p;
  int npixels;
  int npoints;
  
  npixels = cimg->C[0]->nrows * cimg->C[0]->ncols;
  
  p = ComputePixelsProperties(cimg);
  vf = ComputeHistograms(p, mask, npixels, &npoints);
  cvf = CompressHistograms(vf, npoints);
  
  free(p);
  DestroyVisualFeature(&vf);
  return(cvf);
}

Histogram *GCH(CImage *cimg, Image *mask)
{
  Histogram *histogram = NULL;
  CompressedVisualFeature *cvf;
  int i;
  
  cvf = ExtractCompressedVisualFeatures(cimg, mask);

  histogram = CreateHistogram(SIZE);  
  for (i=0; i<SIZE; i++)
    histogram->v[i] = cvf->colorH[i];
  DestroyCompressedVisualFeature(&cvf);

  return(histogram);
}

