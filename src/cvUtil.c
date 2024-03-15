
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "cvUtil.h"
#include "shared/nrutil.h"
#include "shared/error.h"
FoldParms *makeFoldParmsObject(uint threadID, uint n, uint nfolds) {
  if ((nfolds > n) || (nfolds == 0) || (nfolds == 1)) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  nfolds is zero, one or greater than n:  %10d vs. %10d", nfolds, n);
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
  FoldParms *obj = (FoldParms *) gblock((size_t) sizeof(FoldParms));
  obj -> n = n;
  obj -> nfolds = nfolds;
  obj -> oobSizeAlloc = (uint) ceil((double) n / nfolds);
  obj -> ibgSizeAlloc = n - (obj -> oobSizeAlloc) + 1;
  obj -> nfv = makeFolds(threadID, n, nfolds);
  return obj;
}
void freeFoldParmsObject(FoldParms *obj) { 
  free_uivector(obj -> nfv, 1, obj -> n);
  free_gblock(obj, (size_t) sizeof(FoldParms));
}
uint *makeFolds(uint threadID, uint n, uint nfolds) {
  uint *nfvInterim;
  uint i, j;
  if ((nfolds > n) || (nfolds == 0) || (nfolds == 1)) {
    RF_nativeError("\nRF-SRC:  *** ERROR *** ");
    RF_nativeError("\nRF-SRC:  nfolds is zero, one or greater than n:  %10d vs. %10d", nfolds, n);
    RF_nativeError("\nRF-SRC:  Please Contact Technical Support.");
    RF_nativeExit();
  }
  nfvInterim = uivector(1, n);
  i = 0;
  j = 0;
  while (i < n) {
    for (j = 1; (j <= nfolds) && (i < n); j++) {
      i ++;
      nfvInterim[i] = j;
    }
  }
  uint *index = uivector(1, n);
  uint sworSize;
  uint sworSlot;
  sworSize = n;
  for (j = 1; j <= n; j++) {
    sworSlot = (uint) ceil(ran1A(threadID) * (sworSize * 1.0));
    index[j] = nfvInterim[sworSlot];
    nfvInterim[sworSlot] = nfvInterim[sworSize];
    sworSize --;
  }
  free_uivector (nfvInterim, 1, n);
  return index;
}
Fold *makeFoldObject(uint id, FoldParms *fpObj) {
  uint i;
  Fold *obj = (Fold *) gblock((size_t) sizeof(Fold));
  obj -> id = id;
  obj -> oobIndx = uivector (1, fpObj -> oobSizeAlloc);
  obj -> ibgIndx = uivector (1, fpObj -> ibgSizeAlloc);
  obj -> oobSize = obj -> ibgSize = 0;
  for (i = 1; i <= fpObj -> n; i++) {
    if (fpObj -> nfv[i] == id) {
      (obj -> oobSize) ++;
      (obj -> oobIndx)[obj -> oobSize] = i;
    }
    else {
      (obj -> ibgSize) ++;
      (obj -> ibgIndx)[obj -> ibgSize] = i;
    }
  }
  return obj;
}
void freeFoldObject(FoldParms *fpObj, Fold *obj) {
  free_uivector(obj -> oobIndx, 1, fpObj -> oobSizeAlloc);
  free_uivector(obj -> ibgIndx, 1, fpObj -> ibgSizeAlloc);
  free_gblock(obj, (size_t) sizeof(Fold));
}
