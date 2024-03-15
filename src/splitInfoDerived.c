
// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***
#include           "shared/globalCore.h"
#include           "shared/externalCore.h"
#include           "global.h"
#include           "external.h"

// *** THIS HEADER IS AUTO GENERATED. DO NOT EDIT IT ***

      
    

#include "splitInfoDerived.h"
#include "shared/nrutil.h"
SplitInfoDerived *makeSplitInfoDerived(AugmentationObjCommon *augmComm) {
  SplitInfoDerived *info = (SplitInfoDerived*) gblock((size_t) sizeof(SplitInfoDerived));
  initSplitInfoMax((SplitInfoMax*) info, augmComm -> nSize);
  info -> hcut = augmComm -> hcut;
  info -> hcutCnt = augmComm -> hcutCnt;
  info -> beta = NULL;
  info -> yStar = RF_nativeNaN;
  info -> yBar = RF_nativeNaN;
  info -> bsf = 0;
  return info;
}
void freeSplitInfoDerived(SplitInfoDerived *info, AugmentationObjCommon *augmComm) {
  deinitSplitInfoMax((SplitInfoMax*) info);
  if (info -> beta != NULL) {
    free_dvector(info -> beta, 1, info -> hcutCnt + 1);
    info -> beta = NULL;
  }
  free_gblock(info, (size_t) sizeof(SplitInfoDerived));
}
