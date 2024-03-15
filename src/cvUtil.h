#ifndef RF_CV_UTIL_H
#define RF_CV_UTIL_H
typedef struct foldParms FoldParms;
struct foldParms {
  uint n;
  uint nfolds;
  uint ibgSizeAlloc;
  uint oobSizeAlloc;
  uint *nfv;
};
typedef struct fold Fold;
struct fold {
  uint id;
  uint ibgSize;
  uint oobSize;
  uint *ibgIndx;
  uint *oobIndx;
};
FoldParms *makeFoldParmsObject(uint threadID, uint n, uint nfolds);
void freeFoldParmsObject(FoldParms *obj);
uint *makeFolds(uint threadID, uint n, uint nfolds);
Fold *makeFoldObject(uint id, FoldParms *fpObj);
void freeFoldObject(FoldParms *fpObj, Fold *obj);
#endif
