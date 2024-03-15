#ifndef RF_AUGMENTATION_H
#define RF_AUGMENTATION_H
typedef struct augmentationObjCommon AugmentationObjCommon;
struct augmentationObjCommon {
  uint  hcut;
  uint pSize;
  uint nSize;
  uint  hcut1Cnt;
  uint  hcut2Cnt;
  uint  hcut3Cnt;
  uint  hcut4Cnt;
  uint  hcut5Cnt;
  char *permImpure;
  char *perm;
  uint  hcutCnt;
  uint   **hcut1Intr;
  uint   **hcut2Intr;  
  uint   **hcut3Intr;  
  uint   **hcut4Intr;  
  uint   **hcut5Intr;  
  double **hcut1Augm, **hcut2Augm, **hcut3Augm, **hcut4Augm, **hcut5Augm;
  double **hcut1AugmTest, **hcut2AugmTest, **hcut3AugmTest, **hcut4AugmTest, **hcut5AugmTest;
  double **xArray;
  double *yArray;
  uint     fnSize;
  double **fxArray;
  double  *fyArray;
};
typedef struct augmentationObj AugmentationObj;
struct augmentationObj {
  char *perm;
  struct augmentationObjCommon *common;
};
#endif 
