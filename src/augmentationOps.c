
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "augmentationOps.h"
#include "splitUtil.h"
#include "node.h"
#include "shared/nrutil.h"
AugmentationObj *getAugmentationObj(AugmentationObjCommon *objCommon, uint treeID, Node *parent) {
  AugmentationObj *obj;
  char *permImpure;
  char *perm;
  NodeBase *parentBase;
  uint hcut;
  uint hcutCnt;
  uint hcut1Cnt, hcut2Cnt, hcut3Cnt, hcut4Cnt, hcut5Cnt; 
  double **xArray;
  char *perm1, *perm2, *perm3, *perm4, *perm5;
  uint j, k, m, q, ll;
  uint p;
  obj = (AugmentationObj*) gblock((size_t) sizeof(AugmentationObj));
  obj -> common = objCommon;
  hcutCnt = objCommon -> hcutCnt;
  hcut     = objCommon -> hcut;
  hcut1Cnt = objCommon -> hcut1Cnt;
  hcut2Cnt = objCommon -> hcut2Cnt;
  hcut3Cnt = objCommon -> hcut3Cnt;
  hcut4Cnt = objCommon -> hcut4Cnt;
  hcut5Cnt = objCommon -> hcut5Cnt;
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
        if (permImpure[j]) {
          perm2[ll] = TRUE;
        }
        else {
          perm2[ll] = FALSE;
        }
      }
      if (hcut >= 3) {
        perm3 = cvector(1, hcut3Cnt);
        ll = 0;
        for (j = 1; j <= p; j++) {
          for (k = j+1; k <= p; k++) {
            ll++;
            if (permImpure[j] && permImpure[k]) {
              perm3[ll] = TRUE;
            }
            else {
              perm3[ll] = FALSE;
            }
          }
        }
        if (hcut >= 4) {
          perm4 = cvector(1, hcut4Cnt);
          ll = 0;
          for (j = 1; j <= p; j++) {
            for (k = j+1; k <= p; k++) {
              for (m = k+1; m <= p; m++) {
                ll++;
                if (permImpure[j] && permImpure[k] && permImpure[m]) {
                  perm4[ll] = TRUE;
                }
                else {
                  perm4[ll] = FALSE;
                }
              }
            }
          }
          if (hcut >= 5) {
            perm5 = cvector(1, hcut5Cnt);
            ll = 0;
            for (j = 1; j <= p; j++) {
              for (k = j+1; k <= p; k++) {
                for (m = k+1; m <= p; m++) {
                  for (q = m+1; q <= p; q++) {
                    ll++;
                    if (permImpure[j] && permImpure[k] && permImpure[m] && permImpure[q]) {
                      perm5[ll] = TRUE;
                    }
                    else {
                      perm5[ll] = FALSE;
                    }
                  }
                }
              }
            }
          }
        }
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
      if (hcut >= 3) {
        for (j = 1; j <= hcut3Cnt; j++) {
          ll++;
          perm[ll] = perm3[j];
        }
        free_cvector(perm3, 1, hcut3Cnt);        
        if (hcut >= 4) {
          for (j = 1; j <= hcut4Cnt; j++) {
            ll++;
            perm[ll] = perm3[j];
          }
          free_cvector(perm4, 1, hcut4Cnt);
          if (hcut >= 5) {
            for (j = 1; j <= hcut5Cnt; j++) {
              ll++;
              perm[ll] = perm5[j];
            }
            free_cvector(perm5, 1, hcut5Cnt);
          }
        }
      }
    }
  }
  return obj;
}
void freeAugmentationObj(AugmentationObj *obj) {
  if (obj -> perm != NULL) {
    free_cvector(obj -> perm, 1, obj -> common -> hcutCnt);
  }
  free_gblock(obj, (size_t) sizeof(AugmentationObj));
}
