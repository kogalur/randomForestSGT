
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "augmentationOpsCommon.h"
#include "splitUtil.h"
#include "node.h"
#include "shared/nrutil.h"
#include "shared/factorOps.h"
AugmentationObjCommon *getAugmentationObjCommon(uint hcut, uint n, uint p, double **xArrayIn, uint pAugm, double **xArrayInAugm, char *permIn, double *yArrayIn) {
  AugmentationObjCommon *obj;
  uint hcutCnt, hcut1Cnt, hcut2Cnt, hcut3Cnt, hcut4Cnt, hcut5Cnt; 
  char *permImpure;
  char  *perm;
  uint **hcut1Intr, **hcut2Intr, **hcut3Intr, **hcut4Intr, **hcut5Intr; 
  double **hcut1Augm, **hcut2Augm, **hcut3Augm, **hcut4Augm, **hcut5Augm;
  double **xArray;
  char *perm1, *perm2, *perm3, *perm4, *perm5;
  uint i, j, k, m, q, ll;
  obj = (AugmentationObjCommon*) gblock((size_t) sizeof(AugmentationObjCommon));
  obj -> hcut = hcut;
  obj -> nSize = n;
  obj -> pSize = p;
  obj -> yArray = yArrayIn;
  hcut1Cnt = hcut2Cnt = hcut3Cnt = hcut4Cnt = hcut5Cnt = 0;
  hcut1Intr = hcut2Intr = hcut3Intr = hcut4Intr = hcut5Intr = NULL;
  obj -> hcut1AugmTest = obj -> hcut2AugmTest = obj -> hcut3AugmTest = obj -> hcut4AugmTest = obj -> hcut5AugmTest = NULL;
  obj -> fnSize = 0;
  obj -> fxArray = NULL;
  obj -> fyArray = NULL;
  if ((hcut == 0) || (hcut >= 1)) {
    hcut1Cnt = p;
    if (hcut >= 2) {
      hcut2Cnt = p;
      if (hcut >= 3) {
        nChooseK(p, 2, EXACT, &hcut3Cnt);
        if (hcut >= 4) {
          nChooseK(p, 3, EXACT, &hcut4Cnt);
          if (hcut >= 5) {
            nChooseK(p, 4, EXACT, &hcut5Cnt);
          }
        }
      }
    }
  }
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
  hcutCnt = hcut1Cnt + hcut2Cnt + hcut3Cnt + hcut4Cnt + hcut5Cnt;
  perm1 = perm2 = perm3 = perm4 = perm5 = NULL; 
  if ((hcut == 0) || (hcut >= 1)) {
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
    if (hcut >= 2) {
      perm2 = cvector(1, hcut2Cnt);
      hcut2Intr = (uint **) new_vvector(1, 2, NRUTIL_UPTR);
      hcut2Intr[1] = uivector(1, hcut2Cnt);
      hcut2Intr[2] = uivector(1, hcut2Cnt);
      ll = 0;
      for (j = 1; j <= hcut2Cnt; j++) {
        ll++;
        hcut2Intr[1][ll] = hcut2Intr[2][ll] = j;
        if (permImpure[j]) {
          perm2[ll] = TRUE;
        }
        else {
          perm2[ll] = FALSE;
        }
      }
      if (hcut >= 3) {
        perm3 = cvector(1, hcut3Cnt);
        hcut3Intr = (uint **) new_vvector(1, 2, NRUTIL_UPTR);
        hcut3Intr[1] = uivector(1, hcut3Cnt);
        hcut3Intr[2] = uivector(1, hcut3Cnt);
        ll = 0;
        for (j = 1; j <= p; j++) {
          for (k = j+1; k <= p; k++) {
            ll++;
            hcut3Intr[1][ll] = j;
            hcut3Intr[2][ll] = k; 
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
          hcut4Intr = (uint **) new_vvector(1, 3, NRUTIL_UPTR);
          hcut4Intr[1] = uivector(1, hcut4Cnt);
          hcut4Intr[2] = uivector(1, hcut4Cnt);
          hcut4Intr[3] = uivector(1, hcut4Cnt);
          ll = 0;
          for (j = 1; j <= p; j++) {
            for (k = j+1; k <= p; k++) {
              for (m = k+1; m <= p; m++) {
                ll++;
                hcut4Intr[1][ll] = j;
                hcut4Intr[2][ll] = k;
                hcut4Intr[3][ll] = m;
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
            hcut5Intr = (uint **) new_vvector(1, 4, NRUTIL_UPTR);
            hcut5Intr[1] = uivector(1, hcut5Cnt);
            hcut5Intr[2] = uivector(1, hcut5Cnt);
            hcut5Intr[3] = uivector(1, hcut5Cnt);
            hcut5Intr[4] = uivector(1, hcut5Cnt);
            ll = 0;
            for (j = 1; j <= p; j++) {
              for (k = j+1; k <= p; k++) {
                for (m = k+1; m <= p; m++) {
                  for (q = m+1; q <= p; q++) {
                    ll++;
                    hcut5Intr[1][ll] = j;
                    hcut5Intr[2][ll] = k;
                    hcut5Intr[3][ll] = m;
                    hcut5Intr[4][ll] = q;
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
  if ((hcut == 0) || (hcut >= 1)) {
    hcut1Augm = xArrayIn;
    if (hcut >= 2) {
      hcut2Augm = (double **) new_vvector(1, hcut2Cnt, NRUTIL_DPTR);
      for (ll = 1; ll <= hcut2Cnt; ll++) {
        if (perm2[ll] == TRUE) {
          hcut2Augm[ll] = dvector(1, n);
        }
        else {
          hcut2Augm[ll] = NULL;
        }
      }
      if (hcut >= 3) {
        hcut3Augm = (double **) new_vvector(1, hcut3Cnt, NRUTIL_DPTR);
        for (ll = 1; ll <= hcut3Cnt; ll++) {
          if (perm3[ll] == TRUE) {
            hcut3Augm[ll] = dvector(1, n);
          }
          else {
            hcut3Augm[ll] = NULL;
          }
        }
        if (hcut >= 4) {
          hcut4Augm = (double **) new_vvector(1, hcut4Cnt, NRUTIL_DPTR);
          for (ll = 1; ll <= hcut4Cnt; ll++) {
            if (perm4[ll] == TRUE) {
              hcut4Augm[ll] = dvector(1, n);
            }
            else {
              hcut4Augm[ll] = NULL;
            }
          }
          if (hcut >= 5) {
            hcut5Augm = (double **) new_vvector(1, hcut5Cnt, NRUTIL_DPTR);
            for (ll = 1; ll <= hcut5Cnt; ll++) {
              if (perm5[ll] == TRUE) {
                hcut5Augm[ll] = dvector(1, n);
              }
              else {
                hcut5Augm[ll] = NULL;
              }
            }
          }
        }
      }
    }
  }
  obj -> hcut1Augm = hcut1Augm;
  obj -> hcut2Augm = hcut2Augm;
  obj -> hcut3Augm = hcut3Augm;
  obj -> hcut4Augm = hcut4Augm;
  obj -> hcut5Augm = hcut5Augm;
  if ((hcut == 0) || (hcut >= 1)) {
    hcut1Augm = xArrayIn;
    if (hcut >= 2) {
      for (ll = 1; ll <= hcut2Cnt; ll++) {
        if (perm2[ll] == TRUE) {
          for (i = 1; i <= n; i++) {
            hcut2Augm[ll][i] = xArrayIn[hcut2Intr[1][ll]][i] * xArrayIn[hcut2Intr[2][ll]][i];
          }
        }
      }
      if (hcut >= 3) {
        for (ll = 1; ll <= hcut3Cnt; ll++) {
          if (perm3[ll] == TRUE) {
            for (i = 1; i <= n; i++) {
              hcut3Augm[ll][i] = xArrayIn[hcut3Intr[1][ll]][i] * xArrayIn[hcut3Intr[2][ll]][i];
            }
          }
        }
        if (hcut >= 4) {
          for (ll = 1; ll <= hcut4Cnt; ll++) {
            if (perm4[ll] == TRUE) {
              for (i = 1; i <= n; i++) {
                hcut4Augm[ll][i] = xArrayIn[hcut4Intr[1][ll]][i] * xArrayIn[hcut4Intr[2][ll]][i] * xArrayIn[hcut4Intr[3][ll]][i];
              }
            }
          }
          if (hcut >= 5) {
            for (ll = 1; ll <= hcut5Cnt; ll++) {
              if (perm5[ll] == TRUE) {
                for (i = 1; i <= n; i++) {
                  hcut5Augm[ll][i] = xArrayIn[hcut5Intr[1][ll]][i] * xArrayIn[hcut5Intr[2][ll]][i] * xArrayIn[hcut5Intr[3][ll]][i] * xArrayIn[hcut5Intr[4][ll]][i];
                }
              }
            }
          }
        }
      }
    }
  }
  xArray = obj -> xArray = (double **) new_vvector(1, hcutCnt, NRUTIL_DPTR);
  perm = obj -> perm = cvector(1, hcutCnt);
  if ((hcut == 0) || (hcut >= 1)) {
    ll = 0;
    for (j = 1; j <= hcut1Cnt; j++) {
      ll++;
      xArray[ll] = hcut1Augm[j];
      perm[ll] = perm1[j];
    }
    free_cvector(perm1, 1, hcut1Cnt);
    if (hcut >= 2) {
      for (j = 1; j <= hcut2Cnt; j++) {
        ll++;
        xArray[ll] = hcut2Augm[j];
        perm[ll] = perm2[j];
      }
      free_cvector(perm2, 1, hcut2Cnt);
      if (hcut >= 3) {
        for (j = 1; j <= hcut3Cnt; j++) {
          ll++;
          xArray[ll] = hcut3Augm[j];
          perm[ll] = perm3[j];
        }
        free_cvector(perm3, 1, hcut3Cnt);        
        if (hcut >= 4) {
          for (j = 1; j <= hcut4Cnt; j++) {
            ll++;
            xArray[ll] = hcut4Augm[j];
            perm[ll] = perm3[j];
          }
          free_cvector(perm4, 1, hcut4Cnt);
          if (hcut >= 5) {
            for (j = 1; j <= hcut5Cnt; j++) {
              ll++;
              xArray[ll] = hcut5Augm[j];
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
void getAugmentationObjCommonTest(AugmentationObjCommon *obj,
                                  uint     fn,
                                  double **fxArrayIn,
                                  double **fxArrayInAugm,
                                  double  *fyArrayIn) {
  uint hcut;
  uint n;
  uint hcutCnt, hcut1Cnt, hcut2Cnt, hcut3Cnt, hcut4Cnt, hcut5Cnt; 
  char *perm;
  char *perm1, *perm2, *perm3, *perm4, *perm5;
  uint  **hcut2Intr, **hcut3Intr, **hcut4Intr, **hcut5Intr; 
  double **hcut1Augm, **hcut2Augm, **hcut3Augm, **hcut4Augm, **hcut5Augm;
  double **xArrayIn;
  double **xArray;
  double **fxArray;
  uint i, j, ll;
  hcut = obj -> hcut;
  perm = obj -> perm;
  hcutCnt  = obj -> hcutCnt;
  hcut1Cnt = obj -> hcut1Cnt;
  hcut2Cnt = obj -> hcut2Cnt;
  hcut3Cnt = obj -> hcut3Cnt;
  hcut4Cnt = obj -> hcut4Cnt;
  hcut5Cnt = obj -> hcut5Cnt;
  hcut2Intr = obj -> hcut2Intr;
  hcut3Intr = obj -> hcut3Intr;
  hcut4Intr = obj -> hcut4Intr;
  hcut5Intr = obj -> hcut5Intr;
  obj -> fyArray = fyArrayIn;
  obj -> fnSize = fn;
  xArrayIn = fxArrayIn;
  n = fn;
  hcut1Augm = hcut2Augm = hcut3Augm = hcut4Augm = hcut5Augm = NULL;
  perm1 = perm2 = perm3 = perm4 = perm5 = NULL; 
  j = 0;
  if ((hcut == 0) || (hcut >= 1)) {
    perm1 = cvector(1, hcut1Cnt);
    for (ll = 1; ll <= hcut1Cnt; ll++) {
      perm1[ll] = perm[++j];
    }
    if (hcut >= 2) {
      perm2 = cvector(1, hcut2Cnt);
      for (ll = 1; ll <= hcut2Cnt; ll++) {
        perm2[ll] = perm[++j];        
      }
      if (hcut >= 3) {
        perm3 = cvector(1, hcut3Cnt);
        for (ll = 1; ll <= hcut3Cnt; ll++) {
          perm3[ll] = perm[++j];
        }
        if (hcut >= 4) {
          perm4 = cvector(1, hcut4Cnt);          
          for (ll = 1; ll <= hcut4Cnt; ll++) {
            perm4[ll] = perm[++j];
          }
          if (hcut >= 5) {
            perm5 = cvector(1, hcut5Cnt);            
            for (ll = 1; ll <= hcut5Cnt; ll++) {
              perm5[ll] = perm[++j];
            }
          }
        }
      }
    }
  }
  if ((hcut == 0) || (hcut >= 1)) {
    hcut1Augm = xArrayIn;
    if (hcut >= 2) {
      hcut2Augm = (double **) new_vvector(1, hcut2Cnt, NRUTIL_DPTR);
      for (ll = 1; ll <= hcut2Cnt; ll++) {
        if (perm2[ll] == TRUE) {
          hcut2Augm[ll] = dvector(1, n);
        }
        else {
          hcut2Augm[ll] = NULL;
        }
      }
      if (hcut >= 3) {
        hcut3Augm = (double **) new_vvector(1, hcut3Cnt, NRUTIL_DPTR);
        for (ll = 1; ll <= hcut3Cnt; ll++) {
          if (perm3[ll] == TRUE) {
            hcut3Augm[ll] = dvector(1, n);
          }
          else {
            hcut3Augm[ll] = NULL;
          }
        }
        if (hcut >= 4) {
          hcut4Augm = (double **) new_vvector(1, hcut4Cnt, NRUTIL_DPTR);
          for (ll = 1; ll <= hcut4Cnt; ll++) {
            if (perm4[ll] == TRUE) {
              hcut4Augm[ll] = dvector(1, n);
            }
            else {
              hcut4Augm[ll] = NULL;
            }
          }
          if (hcut >= 5) {
            hcut5Augm = (double **) new_vvector(1, hcut5Cnt, NRUTIL_DPTR);
            for (ll = 1; ll <= hcut5Cnt; ll++) {
              if (perm5[ll] == TRUE) {
                hcut5Augm[ll] = dvector(1, n);
              }
              else {
                hcut5Augm[ll] = NULL;
              }
            }
          }
        }
      }
    }
  }
  obj -> hcut1AugmTest = hcut1Augm;
  obj -> hcut2AugmTest = hcut2Augm;
  obj -> hcut3AugmTest = hcut3Augm;
  obj -> hcut4AugmTest = hcut4Augm;
  obj -> hcut5AugmTest = hcut5Augm;
  if ((hcut == 0) || (hcut >= 1)) {
    hcut1Augm = xArrayIn;
    if (hcut >= 2) {
      for (ll = 1; ll <= hcut2Cnt; ll++) {
        if (perm2[ll] == TRUE) {
          for (i = 1; i <= n; i++) {
            hcut2Augm[ll][i] = xArrayIn[hcut2Intr[1][ll]][i] * xArrayIn[hcut2Intr[2][ll]][i];
          }
        }
      }
      if (hcut >= 3) {
        for (ll = 1; ll <= hcut3Cnt; ll++) {
          if (perm3[ll] == TRUE) {
            for (i = 1; i <= n; i++) {
              hcut3Augm[ll][i] = xArrayIn[hcut3Intr[1][ll]][i] * xArrayIn[hcut3Intr[2][ll]][i];
            }
          }
        }
        if (hcut >= 4) {
          for (ll = 1; ll <= hcut4Cnt; ll++) {
            if (perm4[ll] == TRUE) {
              for (i = 1; i <= n; i++) {
                hcut4Augm[ll][i] = xArrayIn[hcut4Intr[1][ll]][i] * xArrayIn[hcut4Intr[2][ll]][i] * xArrayIn[hcut4Intr[3][ll]][i];
              }
            }
          }
          if (hcut >= 5) {
            for (ll = 1; ll <= hcut5Cnt; ll++) {
              if (perm5[ll] == TRUE) {
                for (i = 1; i <= n; i++) {
                  hcut5Augm[ll][i] = xArrayIn[hcut5Intr[1][ll]][i] * xArrayIn[hcut5Intr[2][ll]][i] * xArrayIn[hcut5Intr[3][ll]][i] * xArrayIn[hcut5Intr[4][ll]][i];
                }
              }
            }
          }
        }
      }
    }
  }
  fxArray = obj -> fxArray = (double **) new_vvector(1, hcutCnt, NRUTIL_DPTR);
  xArray = fxArray;
  if ((hcut == 0) || (hcut >= 1)) {
    ll = 0;
    for (j = 1; j <= hcut1Cnt; j++) {
      ll++;
      xArray[ll] = hcut1Augm[j];
    }
    free_cvector(perm1, 1, hcut1Cnt);
    if (hcut >= 2) {
      for (j = 1; j <= hcut2Cnt; j++) {
        ll++;
        xArray[ll] = hcut2Augm[j];
      }
      free_cvector(perm2, 1, hcut2Cnt);
      if (hcut >= 3) {
        for (j = 1; j <= hcut3Cnt; j++) {
          ll++;
          xArray[ll] = hcut3Augm[j];
        }
        free_cvector(perm3, 1, hcut3Cnt);        
        if (hcut >= 4) {
          for (j = 1; j <= hcut4Cnt; j++) {
            ll++;
            xArray[ll] = hcut4Augm[j];
          }
          free_cvector(perm4, 1, hcut4Cnt);
          if (hcut >= 5) {
            for (j = 1; j <= hcut5Cnt; j++) {
              ll++;
              xArray[ll] = hcut5Augm[j];
            }
            free_cvector(perm5, 1, hcut5Cnt);
          }
        }
      }
    }
  }
}
void freeAugmentationObjCommon(AugmentationObjCommon *obj) {
  uint hcut;
  uint hcutCnt;
  uint nSize, pSize;
  char *permImpure;
  char *perm;
  uint hcut1Cnt, hcut2Cnt, hcut3Cnt, hcut4Cnt, hcut5Cnt; 
  uint **hcut1Intr, **hcut2Intr, **hcut3Intr, **hcut4Intr, **hcut5Intr; 
  double **hcut2Augm, **hcut3Augm, **hcut4Augm, **hcut5Augm;
  double **hcut2AugmTest, **hcut3AugmTest, **hcut4AugmTest, **hcut5AugmTest;
  double **xArray;
  uint ll;
  hcut     = obj -> hcut;
  hcutCnt  = obj -> hcutCnt;
  nSize = obj -> nSize;
  pSize = obj -> pSize;
  hcut1Cnt = obj -> hcut1Cnt;
  hcut2Cnt = obj -> hcut2Cnt;
  hcut3Cnt = obj -> hcut3Cnt;
  hcut4Cnt = obj -> hcut4Cnt;
  hcut5Cnt = obj -> hcut5Cnt;
  permImpure = obj -> permImpure;
  hcut1Intr = obj -> hcut1Intr;
  hcut2Intr = obj -> hcut2Intr;
  hcut3Intr = obj -> hcut3Intr;
  hcut4Intr = obj -> hcut4Intr;
  hcut5Intr = obj -> hcut5Intr;
  hcut2Augm = obj -> hcut2Augm;
  hcut3Augm = obj -> hcut3Augm;
  hcut4Augm = obj -> hcut4Augm;
  hcut5Augm = obj -> hcut5Augm;
  xArray    = obj -> xArray;
  perm      = obj -> perm;
  free_cvector(permImpure, 1, pSize);
  if ((hcut == 0) || (hcut >= 1)) {
    free_uivector(hcut1Intr[1], 1, hcut1Cnt);
    free_new_vvector(hcut1Intr, 1, 1, NRUTIL_UPTR);
    if (hcut >= 2) {
      free_uivector(hcut2Intr[1], 1, hcut2Cnt);
      free_uivector(hcut2Intr[2], 1, hcut2Cnt);
      free_new_vvector(hcut2Intr, 1, 2, NRUTIL_UPTR);
      for (ll = 1; ll <= hcut2Cnt; ll++) {
        free_dvector(hcut2Augm[ll], 1, nSize);
      }
      free_new_vvector(hcut2Augm, 1, hcut2Cnt, NRUTIL_DPTR);
      if (hcut >= 3) {
        free_uivector(hcut3Intr[1], 1, hcut3Cnt);
        free_uivector(hcut3Intr[2], 1, hcut3Cnt);
        free_new_vvector(hcut3Intr, 1, 2, NRUTIL_UPTR);
        for (ll = 1; ll <= hcut3Cnt; ll++) {
          free_dvector(hcut3Augm[ll], 1, nSize);
        }
        free_new_vvector(hcut3Augm, 1, hcut3Cnt, NRUTIL_DPTR);
        if (hcut >= 4) {
          free_uivector(hcut4Intr[1], 1, hcut4Cnt);
          free_uivector(hcut4Intr[2], 1, hcut4Cnt);
          free_uivector(hcut4Intr[3], 1, hcut4Cnt);
          free_new_vvector(hcut4Intr, 1, 3, NRUTIL_UPTR);
          for (ll = 1; ll <= hcut4Cnt; ll++) {
            free_dvector(hcut4Augm[ll], 1, nSize);
          }
          free_new_vvector(hcut4Augm, 1, hcut4Cnt, NRUTIL_DPTR);
          if (hcut >= 5) {
            free_uivector(hcut5Intr[1], 1, hcut5Cnt);
            free_uivector(hcut5Intr[2], 1, hcut5Cnt);
            free_uivector(hcut5Intr[3], 1, hcut5Cnt);
            free_uivector(hcut5Intr[4], 1, hcut5Cnt);
            free_new_vvector(hcut5Intr, 1, 4, NRUTIL_UPTR);
            for (ll = 1; ll <= hcut5Cnt; ll++) {
              free_dvector(hcut5Augm[ll], 1, nSize);
            }
            free_new_vvector(hcut5Augm, 1, hcut5Cnt, NRUTIL_DPTR);
          }
        }
      }
    }
  }
  free_new_vvector(xArray, 1, hcutCnt, NRUTIL_DPTR);
  free_cvector(perm, 1, hcutCnt);
  if (obj -> fxArray != NULL) {
    hcut2AugmTest = obj -> hcut2AugmTest;
    hcut3AugmTest = obj -> hcut3AugmTest;
    hcut4AugmTest = obj -> hcut4AugmTest;
    hcut5AugmTest = obj -> hcut5AugmTest;
    if ((hcut == 0) || (hcut >= 1)) {
      if (hcut >= 2) {
        for (ll = 1; ll <= hcut2Cnt; ll++) {
          free_dvector(hcut2AugmTest[ll], 1, nSize);
        }
        free_new_vvector(hcut2AugmTest, 1, hcut2Cnt, NRUTIL_DPTR);
        if (hcut >= 3) {
          for (ll = 1; ll <= hcut3Cnt; ll++) {
            free_dvector(hcut3AugmTest[ll], 1, nSize);
          }
          free_new_vvector(hcut3AugmTest, 1, hcut3Cnt, NRUTIL_DPTR);
          if (hcut >= 4) {
            for (ll = 1; ll <= hcut4Cnt; ll++) {
              free_dvector(hcut4AugmTest[ll], 1, nSize);
            }
            free_new_vvector(hcut4AugmTest, 1, hcut4Cnt, NRUTIL_DPTR);
            if (hcut >= 5) {
              for (ll = 1; ll <= hcut5Cnt; ll++) {
                free_dvector(hcut5AugmTest[ll], 1, nSize);
              }
              free_new_vvector(hcut5AugmTest, 1, hcut5Cnt, NRUTIL_DPTR);
            }
          }
        }
      }
    }
    free_new_vvector(obj -> fxArray, 1, hcutCnt, NRUTIL_DPTR);
  }
  free_gblock(obj, (size_t) sizeof(AugmentationObjCommon));
}
void freeAugmentationObjCommonTestOnly(AugmentationObjCommon *obj) {
  uint hcut;
  uint hcutCnt;
  uint nSize;
  double **hcut2AugmTest, **hcut3AugmTest, **hcut4AugmTest, **hcut5AugmTest;
  uint hcut2Cnt, hcut3Cnt, hcut4Cnt, hcut5Cnt;
  uint ll;
  hcut     = obj -> hcut;
  hcutCnt  = obj -> hcutCnt;
  nSize = obj -> nSize;
  hcut2Cnt = obj -> hcut2Cnt;
  hcut3Cnt = obj -> hcut3Cnt;
  hcut4Cnt = obj -> hcut4Cnt;
  hcut5Cnt = obj -> hcut5Cnt;
  if (obj -> fxArray != NULL) {
    hcut2AugmTest = obj -> hcut2AugmTest;
    hcut3AugmTest = obj -> hcut3AugmTest;
    hcut4AugmTest = obj -> hcut4AugmTest;
    hcut5AugmTest = obj -> hcut5AugmTest;
    if ((hcut == 0) || (hcut >= 1)) {
      if (hcut >= 2) {
        for (ll = 1; ll <= hcut2Cnt; ll++) {
          free_dvector(hcut2AugmTest[ll], 1, nSize);
        }
        free_new_vvector(hcut2AugmTest, 1, hcut2Cnt, NRUTIL_DPTR);
        obj -> hcut2AugmTest = NULL;
        if (hcut >= 3) {
          for (ll = 1; ll <= hcut3Cnt; ll++) {
            free_dvector(hcut3AugmTest[ll], 1, nSize);
          }
          free_new_vvector(hcut3AugmTest, 1, hcut3Cnt, NRUTIL_DPTR);
          obj -> hcut2AugmTest = NULL;
          if (hcut >= 4) {
            for (ll = 1; ll <= hcut4Cnt; ll++) {
              free_dvector(hcut4AugmTest[ll], 1, nSize);
            }
            free_new_vvector(hcut4AugmTest, 1, hcut4Cnt, NRUTIL_DPTR);
            obj -> hcut4AugmTest = NULL;
            if (hcut >= 5) {
              for (ll = 1; ll <= hcut5Cnt; ll++) {
                free_dvector(hcut5AugmTest[ll], 1, nSize);
              }
              free_new_vvector(hcut5AugmTest, 1, hcut5Cnt, NRUTIL_DPTR);
              obj -> hcut5AugmTest = NULL;                    
            }
          }
        }
      }
    }
    free_new_vvector(obj -> fxArray, 1, hcutCnt, NRUTIL_DPTR);
    obj -> fxArray = NULL;
  }
}
