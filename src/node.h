#ifndef RF_NODE_H
#define RF_NODE_H
#include "shared/nodeBase.h"
typedef struct node Node;
struct node {
  struct nodeBase base;
  struct augmentationObj *augm;
  unsigned int nSize;
  double eRisk;
  double eRiskOOB;
  double eRiskCart;
  double eRiskCartOOB;
  double *beta;
  double *yHatAbsolute;
  double *yHatAbsoluteLasso;
  double *yHatAbsoluteCart;
  double mean;
  uint hcut;
  char outcome;
  struct splitInfoDerived *splitInfoDerived;
  unsigned int  tstMembrSizeAlloc;
  unsigned int  tstMembrSize;
  unsigned int *tstMembrIndx;
};
#endif
