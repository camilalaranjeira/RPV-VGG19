#include "libcolordescriptors.h"

/**
 * Função que realiza a extração de características.
 * Entrada: Caminho para a imagem e nome de arquivo para gravação do vetor de características resultante.
 */
void Extraction(char *img_path, char *fv_path)
{
  
}



/**
 * Função para computar distância L1 entre dois vetores.
 * 
 * Não é necessário alterar.
 */
double Distance(char *fv1_path, char *fv2_path)
{
  Histogram *fv1=NULL;
  Histogram *fv2=NULL;
  double distance;

  fv1 = ReadFileHistogram(fv1_path);
  fv2 = ReadFileHistogram(fv2_path);

  distance = (double) L1Distance(fv1, fv2);

  DestroyHistogram(&fv1);
  DestroyHistogram(&fv2);

  return distance;
}
