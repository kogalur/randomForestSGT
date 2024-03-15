#ifndef RF_AUGMENTATION_OPS_CUSTOM_H
#define RF_AUGMENTATION_OPS_CUSTOM_H
#include "augmentation.h"
AugmentationObjCommon *getAugmentationObjCommonCustom(uint hcut, uint n, uint p, double **xArrayIn, uint pAugm, double **xArrayInAugm, char *permIn, double *yArrayIn);
void getAugmentationObjCommonCustomTest(AugmentationObjCommon *obj, uint fn, double **fxArrayIn, double **fxArrayInAugm, double *fyArrayIn);
void freeAugmentationObjCommonCustom(AugmentationObjCommon *obj);
void freeAugmentationObjCommonCustomTestOnly(AugmentationObjCommon *obj);
AugmentationObj *getAugmentationObjCustom(AugmentationObjCommon *objCommon, uint treeID, struct node *parent);
#endif
