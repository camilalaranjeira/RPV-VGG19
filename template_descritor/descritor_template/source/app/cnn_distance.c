#include "libcolordescriptors.h"


/**
 * Implementação padrão para computar distância L1 de dois vetores.
 */
int main(int argc, char** argv)
{
  Histogram *fv1=NULL;
  Histogram *fv2=NULL;
  double distance;

  if (argc != 3) {
    fprintf(stderr,"usage: cnn_distance fv1_path fv2_path\n");
    exit(-1);
  }

  fv1 = ReadFileHistogram(argv[1]);
  fv2 = ReadFileHistogram(argv[2]);

  distance= (double) L1Distance(fv1, fv2);
  printf("%lf\n",distance);

  DestroyHistogram(&fv1);
  DestroyHistogram(&fv2);

  return(0);
}
