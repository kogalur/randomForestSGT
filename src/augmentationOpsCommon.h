#ifndef RF_AUGMENTATION_OPS_COMMON_H
#define RF_AUGMENTATION_OPS_COMMON_H
#include "augmentation.h"
AugmentationObjCommon *getAugmentationObjCommon(uint hcut, uint n, uint p, double **xArrayIn, uint pAugm, double **xArrayInAugm, char *permIn, double *yArrayIn);
void getAugmentationObjCommonTest(AugmentationObjCommon *obj, uint fn, double **fxArrayIn, double **fxArrayInAugm, double *fyArrayIn);
void freeAugmentationObjCommon(AugmentationObjCommon *obj);
void freeAugmentationObjCommonTestOnly(AugmentationObjCommon *obj);
#endif
