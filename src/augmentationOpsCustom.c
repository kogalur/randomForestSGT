
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "augmentationOpsCustom.h"
#include "splitUtil.h"
#include "node.h"
#include "shared/nrutil.h"
AugmentationObjCommon *getAugmentationObjCommonCustom(uint hcut, uint n, uint p, double **xArrayIn, uint pAugm, double **xArrayInAugm, char *permIn, double *yArrayIn) {
  AugmentationObjCommon *obj;
  uint hcutCnt, hcut1Cnt, hcut2Cnt, hcut3Cnt, hcut4Cnt, hcut5Cnt; 
  char *permImpure;
  char  *perm;
  uint **hcut1Intr, **hcut2Intr, **hcut3Intr, **hcut4Intr, **hcut5Intr; 
  double **hcut1Augm, **hcut2Augm, **hcut3Augm, **hcut4Augm, **hcut5Augm;
  double **xArray;
  char *perm1, *perm2, *perm3, *perm4, *perm5;
  uint j, ll;
  obj = (AugmentationObjCommon*) gblock((size_t) sizeof(AugmentationObjCommon));
  obj -> hcut = 2;
  obj -> nSize = n;
  obj -> pSize = p;
  obj -> yArray = yArrayIn;
  hcut1Cnt = hcut2Cnt = hcut3Cnt = hcut4Cnt = hcut5Cnt = 0;
  hcut1Intr = hcut2Intr = hcut3Intr = hcut4Intr = hcut5Intr = NULL;
  obj -> hcut1AugmTest = obj -> hcut2AugmTest = obj -> hcut3AugmTest = obj -> hcut4AugmTest = obj -> hcut5AugmTest = NULL;
  obj -> fnSize = 0;
  obj -> fxArray = NULL;
  obj -> fyArray = NULL;
  hcut1Cnt = p;
  hcut2Cnt = pAugm;
  permImpure = obj -> permImpure = cvector(1, p);
  for (j = 1; j <= p; j++) {
    permImpure[j] = permIn[j];    
    if (permImpure[j]) {
      permImpure[j] = getVarianceSinglePass(RF_identityMembershipIndexSize,  
                                            RF_identityMembershipIndex,      
                                            xArrayIn[j],
                                            NULL,
                                            NULL);
    }
  }
  hcutCnt = hcut1Cnt + hcut2Cnt;
  perm1 = perm2 = perm3 = perm4 = perm5 = NULL; 
  perm1 = cvector(1, hcut1Cnt);
  hcut1Intr = (uint **) new_vvector(1, 1, NRUTIL_UPTR);
  hcut1Intr[1] = uivector(1, hcut1Cnt);
  ll = 0;
  for (j = 1; j <= hcut1Cnt; j++) {
    ll++;
    hcut1Intr[1][ll] = j;
    if (permImpure[j]) {
      perm1[ll] = TRUE;
    }
    else {
      perm1[ll] = FALSE;
    }
  }
  perm2 = cvector(1, hcut2Cnt);
  hcut2Intr = (uint **) new_vvector(1, 1, NRUTIL_UPTR);
  hcut2Intr[1] = uivector(1, hcut2Cnt);
  ll = 0;
  for (j = 1; j <= hcut2Cnt; j++) {
    ll++;
    hcut2Intr[1][ll] = j;
    perm2[ll] = TRUE;
  }
  obj -> hcutCnt  = hcutCnt;
  obj -> hcut1Cnt = hcut1Cnt;
  obj -> hcut2Cnt = hcut2Cnt;
  obj -> hcut3Cnt = hcut3Cnt;
  obj -> hcut4Cnt = hcut4Cnt;
  obj -> hcut5Cnt = hcut5Cnt;
  obj -> hcut1Intr = hcut1Intr;
  obj -> hcut2Intr = hcut2Intr;
  obj -> hcut3Intr = hcut3Intr;
  obj -> hcut4Intr = hcut4Intr;
  obj -> hcut5Intr = hcut5Intr;
  hcut1Augm = hcut2Augm = hcut3Augm = hcut4Augm = hcut5Augm = NULL;
  hcut1Augm = xArrayIn;
  hcut2Augm = xArrayInAugm;
  obj -> hcut1Augm = hcut1Augm;
  obj -> hcut2Augm = hcut2Augm;
  obj -> hcut3Augm = hcut3Augm;
  obj -> hcut4Augm = hcut4Augm;
  obj -> hcut5Augm = hcut5Augm;
  xArray = obj -> xArray = (double **) new_vvector(1, hcutCnt, NRUTIL_DPTR);
  perm = obj -> perm = cvector(1, hcutCnt);
  ll = 0;
  for (j = 1; j <= hcut1Cnt; j++) {
    ll++;
    xArray[ll] = hcut1Augm[j];
    perm[ll] = perm1[j];
  }
  free_cvector(perm1, 1, hcut1Cnt);
  for (j = 1; j <= hcut2Cnt; j++) {
    ll++;
    xArray[ll] = hcut2Augm[j];
    perm[ll] = perm2[j];
  }
  free_cvector(perm2, 1, hcut2Cnt);
  return obj;
}
void getAugmentationObjCommonCustomTest(AugmentationObjCommon *obj, uint fn, double **fxArrayIn, double **fxArrayInAugm, double *fyArrayIn) {
  uint hcutCnt, hcut1Cnt, hcut2Cnt;
  double **hcut1AugmTest, **hcut2AugmTest, **hcut3AugmTest, **hcut4AugmTest, **hcut5AugmTest;
  double **fxArray;
  uint j, ll;
  hcutCnt  = obj -> hcutCnt;
  hcut1Cnt = obj -> hcut1Cnt;
  hcut2Cnt = obj -> hcut2Cnt;
  obj -> fyArray = fyArrayIn;
  obj -> fnSize = fn;
  hcut1AugmTest = hcut2AugmTest = hcut3AugmTest = hcut4AugmTest = hcut5AugmTest = NULL;
  hcut1AugmTest = fxArrayIn;
  hcut2AugmTest = fxArrayInAugm;
  obj -> hcut1AugmTest = hcut1AugmTest;
  obj -> hcut2AugmTest = hcut2AugmTest;
  obj -> hcut3AugmTest = hcut3AugmTest;
  obj -> hcut4AugmTest = hcut4AugmTest;
  obj -> hcut5AugmTest = hcut5AugmTest;
  fxArray = obj -> fxArray = (double **) new_vvector(1, hcutCnt, NRUTIL_DPTR);
  ll = 0;
  for (j = 1; j <= hcut1Cnt; j++) {
    ll++;
    fxArray[ll] = hcut1AugmTest[j];
  }
  for (j = 1; j <= hcut2Cnt; j++) {
    ll++;
    fxArray[ll] = hcut2AugmTest[j];
  }
}
void freeAugmentationObjCommonCustom(AugmentationObjCommon *obj) {
  uint hcutCnt;
  uint pSize;
  char *permImpure;
  char *perm;
  uint hcut1Cnt, hcut2Cnt;
  uint **hcut1Intr, **hcut2Intr;
  hcutCnt  = obj -> hcutCnt;
  pSize = obj -> pSize;
  hcut1Cnt = obj -> hcut1Cnt;
  hcut2Cnt = obj -> hcut2Cnt;
  permImpure = obj -> permImpure;
  hcut1Intr = obj -> hcut1Intr;
  hcut2Intr = obj -> hcut2Intr;
  perm      = obj -> perm;
  free_cvector(permImpure, 1, pSize);
  free_uivector(hcut1Intr[1], 1, hcut1Cnt);
  free_new_vvector(hcut1Intr, 1, 1, NRUTIL_UPTR);
  free_uivector(hcut2Intr[1], 1, hcut2Cnt);
  free_new_vvector(hcut2Intr, 1, 1, NRUTIL_UPTR);
  free_new_vvector(obj -> xArray, 1, hcutCnt, NRUTIL_DPTR);
  free_cvector(perm, 1, hcutCnt);
  if (obj -> fxArray != NULL) {
    free_new_vvector(obj -> fxArray, 1, hcutCnt, NRUTIL_DPTR);
  }
  free_gblock(obj, (size_t) sizeof(AugmentationObjCommon));
}
void freeAugmentationObjCommonCustomTestOnly(AugmentationObjCommon *obj) {
  uint hcutCnt;
  hcutCnt  = obj -> hcutCnt;
  if (obj -> fxArray != NULL) {
    free_new_vvector(obj -> fxArray, 1, hcutCnt, NRUTIL_DPTR);
    obj -> fxArray = NULL;
  }
}
AugmentationObj *getAugmentationObjCustom(AugmentationObjCommon *objCommon, uint treeID, Node *parent) {
  AugmentationObj *obj;
  char *permImpure;
  char *perm;
  NodeBase *parentBase;
  uint hcut;
  uint hcutCnt;
  uint hcut1Cnt, hcut2Cnt;
  double **xArray;
  char *perm1, *perm2, *perm3, *perm4, *perm5;
  uint j, ll;
  uint p;
  obj = (AugmentationObj*) gblock((size_t) sizeof(AugmentationObj));
  obj -> common = objCommon;
  hcutCnt = objCommon -> hcutCnt;
  hcut     = objCommon -> hcut;
  hcut1Cnt = objCommon -> hcut1Cnt;
  hcut2Cnt = objCommon -> hcut2Cnt;
  p  = objCommon -> pSize;
  xArray = objCommon -> xArray;
  parentBase = (NodeBase*) parent;
  permImpure = parentBase -> permissible;
  perm1 = perm2 = perm3 = perm4 = perm5 = NULL;
  for (j = 1; j <= p; j++) {
    if (permImpure[j] == TRUE) {
      permImpure[j] = getVarianceSinglePass(parentBase -> repMembrSize,
                                            parentBase -> repMembrIndx,
                                            xArray[j],
                                            NULL,
                                            NULL);
    }
  }
  if ((hcut == 0) || (hcut >= 1)) {
    perm1 = cvector(1, hcut1Cnt);
    ll = 0;
    for (j = 1; j <= hcut1Cnt; j++) {
      ll++;
      if (permImpure[j]) {
        perm1[ll] = TRUE;
      }
      else {
        perm1[ll] = FALSE;
      }
    }
    if (hcut >= 2) {
      perm2 = cvector(1, hcut2Cnt);
      ll = 0;
      for (j = 1; j <= hcut2Cnt; j++) {
        ll++;
        perm2[ll] = TRUE;
      }
    }
  }
  perm = obj -> perm = cvector(1, hcutCnt);
  if ((hcut == 0) || (hcut >= 1)) {
    ll = 0;
    for (j = 1; j <= hcut1Cnt; j++) {
      ll++;
      perm[ll] = perm1[j];
    }
    free_cvector(perm1, 1, hcut1Cnt);
    if (hcut >= 2) {
      for (j = 1; j <= hcut2Cnt; j++) {
        ll++;
        perm[ll] = perm2[j];
      }
      free_cvector(perm2, 1, hcut2Cnt);
    }
  }
  return obj;
}
